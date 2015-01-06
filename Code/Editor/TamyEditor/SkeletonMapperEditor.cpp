#include "SkeletonMapperEditor.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonMapper.h"

// custom widgets
#include "MappedSkeletonFrame.h"
#include "MappingTable.h"

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

   m_sourceChainsTree = new QTreeWidget( frame );
   m_sourceChainsTree->setHeaderLabel( tr( "Source chains" ) );
   layout->addWidget( m_sourceChainsTree );

   m_targetChainsTree = new QTreeWidget( frame );
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
   }
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
   m_skeletonMapper.setSkeletons( m_sourceTree->getSkeleton(), m_targetTree->getSkeleton() );

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
      m_skeletonMapper.addSourceChain( sourceChainName.c_str(), sourceChainStart.c_str(), sourceChainEnd.c_str() );
      m_skeletonMapper.addTargetChain( targetChainName.c_str(), targetChainStart.c_str(), targetChainEnd.c_str() );
      m_skeletonMapper.mapChain( sourceChainName.c_str(), targetChainName.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////
