#include "SkeletonMapperEditor.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonMapper.h"

// custom widgets
#include "ResourceDropArea.h"

// qt widgets
#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QListWidget>
#include <QtWidgets\QAction>
#include <QtWidgets\QSplitter>

// resource mime data
#include "FSNodeMimeData.h"

// tools
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

SkeletonMapperEditor::SkeletonMapperEditor( SkeletonMapper& mapper )
   : m_skeletonMapper( mapper )
   , m_sourceSkeletonPath( NULL )
   , m_targetSkeletonPath( NULL )
   , m_sourceTree( NULL )
   , m_targetTree( NULL )
   , m_actionSave( NULL )
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
      createToolbar( mainLayout );

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
   }

   updateSkeletonViews();
   updateMappingTable();
   updateChainsView();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createToolbar( QLayout* layout )
{
   QToolBar* toolbar = new QToolBar( this );
   layout->addWidget( toolbar );

   // save resource action
   {
      m_actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), toolbar );
      toolbar->addAction( m_actionSave );
      connect( m_actionSave, SIGNAL( triggered() ), this, SLOT( saveResource() ) );
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

   createSkeletonResourceFrame( layout, m_sourceTree, m_sourceSkeletonPath );
   createSkeletonResourceFrame( layout, m_targetTree, m_targetSkeletonPath );

   connect( m_sourceSkeletonPath, SIGNAL( pathChanged( const FilePath& ) ), this, SLOT( sourceSkeletonChanged( const FilePath& ) ) );
   connect( m_targetSkeletonPath, SIGNAL( pathChanged( const FilePath& ) ), this, SLOT( targetSkeletonChanged( const FilePath& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createSkeletonResourceFrame( QLayout* parentLayout, QTreeWidget*& outTreeWidget, ResourceDropArea*& outSkelketonPathWidget )
{
   QFrame* frame = new QFrame( parentLayout->widget() );
   QVBoxLayout* layout = new QVBoxLayout( frame );
   frame->setLayout( layout );
   parentLayout->addWidget( frame );

   outSkelketonPathWidget = new ResourceDropArea( frame );
   layout->addWidget( outSkelketonPathWidget );

   outTreeWidget = new QTreeWidget( frame );
   outTreeWidget->setHeaderLabel( tr("") );
   layout->addWidget( outTreeWidget, 1 );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::createMappingTable( QSplitter* splitter )
{
   m_mappingTable = new QTreeWidget( splitter );
   splitter->addWidget( m_mappingTable );

   QStringList headerLabels;
   headerLabels.push_back( tr( "Source chain name" ) );
   headerLabels.push_back( tr( "Source chain start" ) );
   headerLabels.push_back( tr( "Source chain end" ) );
   headerLabels.push_back( tr( "Target chain name" ) );
   headerLabels.push_back( tr( "Target chain start" ) );
   headerLabels.push_back( tr( "Target chain end" ) );
   m_mappingTable->setHeaderLabels( headerLabels );
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

void SkeletonMapperEditor::initSkeletonTree( QTreeWidget* treeWidget, const Skeleton* skeleton ) const
{
   if ( !skeleton )
   {
      return;
   }

   const uint boneCount = skeleton->getBoneCount();
   for ( uint i = 0; i < boneCount; ++i )
   {
      const int boneIdx = skeleton->m_bonesUpdateOrder[i];
      QTreeWidgetItem* newBoneItem = new QTreeWidgetItem();
      newBoneItem->setText( 0, skeleton->m_boneNames[boneIdx].c_str() );

      const int parentBoneIdx = skeleton->m_boneParentIndices[boneIdx];
      if ( parentBoneIdx >= 0 )
      {
         const char* parentBoneName = skeleton->m_boneNames[parentBoneIdx].c_str();

         QTreeWidgetItem* parentItem = findBoneItem( treeWidget, parentBoneName );
         ASSERT_MSG( parentItem != NULL, "The parent bone item hasn't been created yet - that means this algorithm is flawed" );

         if ( parentItem )
         {
            parentItem->addChild( newBoneItem );
         }
      }
      else
      {
         // this is the root item
         treeWidget->addTopLevelItem( newBoneItem );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* SkeletonMapperEditor::findBoneItem( QTreeWidget* treeWidget, const QString& boneName ) const
{
   List< QTreeWidgetItem* > items;

   {
      const uint count = treeWidget->topLevelItemCount();
      for ( uint i = 0; i < count; ++i )
      {
         items.pushBack( treeWidget->topLevelItem( i ) );
      }
   }

   while ( !items.empty() )
   {
      QTreeWidgetItem* analyzedItem = items.front();
      items.popFront();

      const QString& nodeName = analyzedItem->text( 0 );
      std::string x = nodeName.toStdString();
      if ( nodeName == boneName )
      {
         return analyzedItem;
      }

      // add the children to the queue
      const int childrenCount = analyzedItem->childCount();
      for ( uint i = 0; i < childrenCount; ++i )
      {
         items.pushBack( analyzedItem->child( i ) );
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::saveResource()
{
   m_skeletonMapper.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::buildMapping()
{
   std::string errorMessages;
   m_skeletonMapper.buildMapperUsingBoneNames( m_skeletonMapper.m_sourceSkeleton, m_skeletonMapper.m_targetSkeleton, errorMessages );
   
   updateSkeletonViews();
   updateMappingTable();
   updateChainsView();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::sourceSkeletonChanged( const FilePath& path )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Skeleton* newSourceSkeleton = resMgr.create< Skeleton >( path, true );

   m_skeletonMapper.defineMapping( newSourceSkeleton, m_skeletonMapper.m_targetSkeleton );

   updateSkeletonViews();
   updateMappingTable();
   updateChainsView();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::targetSkeletonChanged( const FilePath& path )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Skeleton* newTargetSkeleton = resMgr.create< Skeleton >( path, true );

   m_skeletonMapper.defineMapping( m_skeletonMapper.m_sourceSkeleton, newTargetSkeleton );

   updateSkeletonViews();
   updateMappingTable();
   updateChainsView();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::updateSkeletonViews()
{
   // fill the widgets with data
   m_sourceSkeletonPath->setFilePath( m_skeletonMapper.m_sourceSkeleton ? m_skeletonMapper.m_sourceSkeleton->getFilePath() : FilePath() );
   m_targetSkeletonPath->setFilePath( m_skeletonMapper.m_targetSkeleton ? m_skeletonMapper.m_targetSkeleton->getFilePath() : FilePath() );

   m_sourceTree->clear();
   initSkeletonTree( m_sourceTree, m_skeletonMapper.m_sourceSkeleton );

   m_targetTree->clear();
   initSkeletonTree( m_targetTree, m_skeletonMapper.m_targetSkeleton );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::updateMappingTable()
{
   m_mappingTable->clear();

   const Skeleton* sourceChainSkeleton = m_skeletonMapper.getSourceChainSkeleton();
   const Skeleton* targetChainSkeleton = m_skeletonMapper.getTargetChainSkeleton();

   const uint targetChainsCount = m_skeletonMapper.getTargetChainsCount();
   for ( uint targetChainIdx = 0; targetChainIdx < targetChainsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_skeletonMapper.getMappingForChain( targetChainIdx );
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      const SkeletonBoneChain* targetChain = m_skeletonMapper.getTargetChain( targetChainIdx );
      const SkeletonBoneChain* sourceChain = m_skeletonMapper.getSourceChain( sourceChainIdx );

      ASSERT( targetChain && sourceChain && sourceChain != targetChain );

      const char* sourceChainStart = m_skeletonMapper.m_sourceSkeleton->m_boneNames[sourceChain->m_firstBoneIdx].c_str();
      const char* sourceChainEnd = m_skeletonMapper.m_sourceSkeleton->m_boneNames[sourceChain->m_lastBoneIdx].c_str();
      const char* targetChainStart = m_skeletonMapper.m_targetSkeleton->m_boneNames[targetChain->m_firstBoneIdx].c_str();
      const char* targetChainEnd = m_skeletonMapper.m_targetSkeleton->m_boneNames[targetChain->m_lastBoneIdx].c_str();

      QStringList boneLabels;
      boneLabels.push_back( sourceChainSkeleton->m_boneNames[sourceChainIdx].c_str() );
      boneLabels.push_back( sourceChainStart );
      boneLabels.push_back( sourceChainEnd );
      boneLabels.push_back( targetChainSkeleton->m_boneNames[targetChainIdx].c_str() );
      boneLabels.push_back( targetChainStart );
      boneLabels.push_back( targetChainEnd );
      m_mappingTable->addTopLevelItem( new QTreeWidgetItem() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperEditor::updateChainsView()
{
   m_sourceChainsTree->clear();
   initSkeletonTree( m_sourceChainsTree, m_skeletonMapper.getSourceChainSkeleton() );

   m_targetChainsTree->clear();
   initSkeletonTree( m_targetChainsTree, m_skeletonMapper.getTargetChainSkeleton() );

}

///////////////////////////////////////////////////////////////////////////////
