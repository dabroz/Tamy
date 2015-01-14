#include "SkeletonMapperEditor.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\SkeletonMapperBuilder.h"

// custom widgets
#include "MappedSkeletonFrame.h"
#include "MappingTable.h"
#include "ChainSkeletonTree.h"
#include "ToolsWindow.h"
#include "tamyeditor.h"

// qt widgets
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QListWidget>
#include <QtWidgets\QAction>
#include <QtWidgets\QSplitter>
#include <QtCore\QMimeData>
#include <QtGui\qevent.h>

// resource mime data
#include "FSNodeMimeData.h"

// tools
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

SkeletonMapperEditor::SkeletonMapperEditor( SkeletonMapper& mapper )
   : m_skeletonMapper( mapper )
   , m_sourceTree( NULL )
   , m_targetTree( NULL )
   , m_actionSave( NULL )
   , m_buildMapping( NULL )
   , m_addMapping( NULL )
   , m_removeMapping( NULL )
   , m_constructNameBasedMapping( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperEditor::~SkeletonMapperEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::onInitialize()
{
   // create the layout
   QVBoxLayout* mainLayout = new QVBoxLayout( this );
   {
      mainLayout->setSpacing( 0 );
      mainLayout->setMargin( 0 );
      setLayout( mainLayout );
   }

   {
      QToolBar* toolbar = new QToolBar( this );
      mainLayout->addWidget( toolbar );

      QSplitter* horizSplitter = new QSplitter( Qt::Horizontal, this );
      mainLayout->addWidget( horizSplitter );
      {
         createMappedSkeletonsFrame( horizSplitter );

         QSplitter* vertSplitter = new QSplitter( Qt::Vertical, horizSplitter );
         {
            horizSplitter->addWidget( vertSplitter );
            createMappingTable( vertSplitter );
            createSkeletonChainsFrame( vertSplitter );
         }
      }

      defineToolbarActions( toolbar );
   }

   mappingSelectionChanged();
   syncEditorToResource();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::defineToolbarActions( QToolBar* toolbar )
{
   // save resource action
   {
      m_actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), toolbar );
      toolbar->addAction( m_actionSave );
      connect( m_actionSave, SIGNAL( triggered() ), this, SLOT( saveResource() ) );
   }

   toolbar->addSeparator();

   // add mapping action
   {
      m_addMapping = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/plus.png" ) ), tr( "Add mapping" ), toolbar );
      toolbar->addAction( m_addMapping );
      connect( m_addMapping, SIGNAL( triggered() ), m_mappingTable, SLOT( addMapping() ) );
   }

   // remove mapping action
   {
      m_removeMapping = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/minus.png" ) ), tr( "Remove mapping" ), toolbar );
      toolbar->addAction( m_removeMapping );
      connect( m_removeMapping, SIGNAL( triggered() ), m_mappingTable, SLOT( removeMapping() ) );
   }

   // build mapping action
   {
      m_buildMapping = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/play.png" ) ), tr( "Build mapping" ), toolbar );
      toolbar->addAction( m_buildMapping );
      connect( m_buildMapping, SIGNAL( triggered() ), this, SLOT( buildMapping() ) );
   }

   toolbar->addSeparator();

   // construct name based mapping action
   {
      m_constructNameBasedMapping = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/debugMenu.png" ) ), tr( "Construct name based mapping" ), toolbar );
      toolbar->addAction( m_constructNameBasedMapping );
      connect( m_constructNameBasedMapping, SIGNAL( triggered() ), this, SLOT( constructNameBasedMapping() ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createMappedSkeletonsFrame( QSplitter* splitter )
{
   QFrame* frame = new QFrame( splitter );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   frame->setLayout( layout );
   splitter->addWidget( frame );

   // source skeleton tree
   m_sourceTree = new MappedSkeletonFrame( frame, true );
   m_targetTree = new MappedSkeletonFrame( frame, false );
   layout->addWidget( m_sourceTree );
   layout->addWidget( m_targetTree );

   connect( m_sourceTree, SIGNAL( skeletonChanged() ), this, SLOT( onSkeletonChanged() ) );
   connect( m_targetTree, SIGNAL( skeletonChanged() ), this, SLOT( onSkeletonChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createMappingTable( QSplitter* splitter )
{
   m_mappingTable = new MappingTable( splitter );
   splitter->addWidget( m_mappingTable );

   connect( m_mappingTable, SIGNAL( itemSelectionChanged() ), this, SLOT( mappingSelectionChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createSkeletonChainsFrame( QSplitter* splitter )
{
   QFrame* frame = new QFrame( splitter );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   frame->setLayout( layout );
   splitter->addWidget( frame );

   m_sourceChainsTree = new ChainSkeletonTree( frame );
   m_sourceChainsTree->setHeaderLabel( tr( "Source chains" ) );
   layout->addWidget( m_sourceChainsTree );

   m_targetChainsTree = new ChainSkeletonTree( frame );
   m_targetChainsTree->setHeaderLabel( tr( "Target chains" ) );
   layout->addWidget( m_targetChainsTree );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      m_skeletonMapper.saveResource();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::saveResource()
{
   syncResourceToEditor();
   m_skeletonMapper.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::buildMapping()
{
   if ( !m_sourceTree->getSkeleton() || !m_targetTree->getSkeleton() )
   {
      // nothing to check
      return;
   }

   m_mappingTable->resetErrorFlag();
   m_mappingTable->verifyChain( SourceBoneChain, m_sourceTree->getSkeleton() );
   m_mappingTable->verifyChain( TargetBoneChain, m_targetTree->getSkeleton() );
   if ( m_mappingTable->getErrorFlag() )
   {
      // there were errors - don't proceed with the build
      return;
   }

   // build the chain mapping skeletons
   m_mappingTable->buildMapping( m_sourceTree->getSkeleton(), m_targetTree->getSkeleton() );

   if ( m_mappingTable->getErrorFlag() )
   {
      // there were some errors during the build stage - reset the build results
      m_mappingTable->clearBuildResults();
      return;
   }

   // all went well - show the compiled chain skeletons
   m_sourceChainsTree->setSkeleton( m_mappingTable->m_sourceChainSkeleton );
   m_targetChainsTree->setSkeleton( m_mappingTable->m_targetChainSkeleton );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::onSkeletonChanged()
{
   // the skeleton has changed - reset previous mapping settings
   m_mappingTable->clear();
   m_sourceChainsTree->clear();
   m_targetChainsTree->clear();

   // enable the add mapping button if both skeletons are defined, and disable it when
   // either one or both are missing
   const bool enableActions = ( m_sourceTree->getSkeleton() != NULL && m_targetTree->getSkeleton() != NULL );
   m_addMapping->setEnabled( enableActions );
   m_buildMapping->setEnabled( enableActions );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::constructNameBasedMapping()
{
   const Skeleton* sourceSkeleton = m_sourceTree->getSkeleton();
   const Skeleton* targetSkeleton = m_targetTree->getSkeleton();
   if ( !sourceSkeleton || !targetSkeleton )
   {
      return;
   }

   // identify bones with exact same names we should map between
   Array< int > boneMapping;
   bool result = identifyBonesWithMatchingNames( sourceSkeleton, targetSkeleton, boneMapping );
   if ( !result )
   {
      return;
   }

   // create chains
   Array< SkeletonBoneChain* > sourceChains;
   Array< SkeletonBoneChain* > targetChains;
   const uint boneMappingsCount = boneMapping.size();
   for ( uint targetBoneIdx = 0; targetBoneIdx < boneMappingsCount; ++targetBoneIdx )
   {
      int sourceBoneIdx = boneMapping[targetBoneIdx];
      if ( sourceBoneIdx < 0 )
      {
         continue;
      }

      sourceChains.push_back( new SkeletonBoneChain( sourceSkeleton, sourceSkeleton->m_boneNames[sourceBoneIdx], sourceBoneIdx, sourceBoneIdx ) );
      targetChains.push_back( new SkeletonBoneChain( targetSkeleton, targetSkeleton->m_boneNames[targetBoneIdx], targetBoneIdx, targetBoneIdx ) );
   }

   // try expanding the created chains to cover unmapped bones as well
   const uint chainsCount = sourceChains.size();
   SkeletonTree* sourceSkeletonTree = SkeletonMapperUtils::buildSkeletonTree( sourceSkeleton );
   SkeletonTree* targetSkeletonTree = SkeletonMapperUtils::buildSkeletonTree( targetSkeleton );
   for ( uint i = 0; i < chainsCount; ++i )
   {
      SkeletonMapperUtils::expandChain( i, sourceChains, sourceSkeletonTree );
   }
   for ( uint i = 0; i < chainsCount; ++i )
   {
      SkeletonMapperUtils::expandChain( i, targetChains, targetSkeletonTree );
   }
   
   // persist the results in the widget
   for ( uint i = 0; i < chainsCount; ++i )
   {
      SkeletonBoneChain* sourceChain = sourceChains[i];
      SkeletonBoneChain* targetChain = targetChains[i];

      const std::string& firstSourceBone = sourceSkeleton->m_boneNames[sourceChain->m_firstBoneIdx];
      const std::string& lastSourceBone = sourceSkeleton->m_boneNames[sourceChain->m_lastBoneIdx]; 
      const std::string& firstTargetBone = targetSkeleton->m_boneNames[targetChain->m_firstBoneIdx]; 
      const std::string& lastTargetBone = targetSkeleton->m_boneNames[targetChain->m_lastBoneIdx];

      m_mappingTable->addEntry( firstSourceBone, lastSourceBone, firstTargetBone, lastTargetBone );
   }

   // cleanup
   delete sourceSkeletonTree;
   delete targetSkeletonTree;
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete sourceChains[i];
   }
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete targetChains[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonMapperEditor::identifyBonesWithMatchingNames( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton, Array< int >& outMappings ) const
{
   const uint sourceBonesCount = sourceSkeleton->getBoneCount();
   const uint targetBonesCount = targetSkeleton->getBoneCount();
   outMappings.resize( targetBonesCount, -1 );

   Array< bool > mappedSourceBones( sourceBonesCount );
   mappedSourceBones.resize( sourceBonesCount, false );

   for ( uint targetBoneIdx = 0; targetBoneIdx < targetBonesCount; ++targetBoneIdx )
   {
      const std::string& targetBoneName = targetSkeleton->m_boneNames[targetBoneIdx];
      for ( uint sourceBoneIdx = 0; sourceBoneIdx < sourceBonesCount; ++sourceBoneIdx )
      {
         const std::string& sourceBoneName = sourceSkeleton->m_boneNames[sourceBoneIdx];
         if ( sourceBoneName == targetBoneName )
         {
            if ( mappedSourceBones[sourceBoneIdx] )
            {
               // this source chain has already been mapped onto - throw an error
               ToolsWindow* toolsWindow = TamyEditor::getInstance().getToolsWindow();

               char errMsg[512];
               sprintf_s( errMsg, "Source chain %s ( chain idx %d ) was found to be mapped onto by 2 target chains", sourceBoneName.c_str(), sourceBoneIdx );
               toolsWindow->displayMessage( "Name-based skeleton mapper construction error", errMsg );

               return false;
            }

            outMappings[targetBoneIdx] = sourceBoneIdx;
            mappedSourceBones[sourceBoneIdx] = true;
            break;
         }
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::mappingSelectionChanged()
{
   // we want to enable the 'remove mapping' button only when a mapping is selected
   bool enableButton = !m_mappingTable->selectedItems().isEmpty();
   m_removeMapping->setEnabled( enableButton );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::syncEditorToResource()
{
   m_sourceTree->setSkeleton( m_skeletonMapper.m_sourceSkeleton );
   m_targetTree->setSkeleton( m_skeletonMapper.m_targetSkeleton );
   m_mappingTable->setMapping( m_skeletonMapper );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::syncResourceToEditor()
{
   SkeletonMapperBuilder builder( m_skeletonMapper );
   builder.setSkeletons( m_sourceTree->getSkeleton(), m_targetTree->getSkeleton() );

   const uint mappingsCount = m_mappingTable->topLevelItemCount();
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      QTreeWidgetItem* item = m_mappingTable->topLevelItem( i );
      std::string sourceChainName = item->text( BoneMappingCol_SourceChainName ).toStdString();
      std::string sourceChainStart = item->text( BoneMappingCol_SourceChainStart ).toStdString();
      std::string sourceChainEnd = item->text( BoneMappingCol_SourceChainEnd ).toStdString();
      std::string targetChainName = item->text( BoneMappingCol_TargetChainName ).toStdString();
      std::string targetChainStart = item->text( BoneMappingCol_TargetChainStart ).toStdString();
      std::string targetChainEnd = item->text( BoneMappingCol_TargetChainEnd ).toStdString();
      builder.addSourceChain( sourceChainName.c_str(), sourceChainStart.c_str(), sourceChainEnd.c_str() );
      builder.addTargetChain( targetChainName.c_str(), targetChainStart.c_str(), targetChainEnd.c_str() );
      builder.mapChain( sourceChainName.c_str(), targetChainName.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////
