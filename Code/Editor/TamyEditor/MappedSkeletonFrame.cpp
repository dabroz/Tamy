#include "MappedSkeletonFrame.h"

// animation
#include "core-AI\Skeleton.h"

// custom widgets
#include "ResourceDropArea.h"

// qt widgets
#include <QtCore\QMimeData>
#include <QtGui\qevent.h>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>

// tools
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

MappedSkeletonTree::MappedSkeletonTree( QWidget* parent, bool isSource )
   : QTreeWidget( parent )
   , m_isSource( isSource )
   , m_skeleton( NULL )
{
   QStringList headerLabels;
   headerLabels.push_back( "Bone name" );
   headerLabels.push_back( "Bone index" );
   setHeaderLabels( headerLabels );

   setDragEnabled( true );
   setAcceptDrops( false );
}

///////////////////////////////////////////////////////////////////////////////

MappedSkeletonTree::~MappedSkeletonTree()
{
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* MappedSkeletonTree::mimeData( const QList<QTreeWidgetItem *> items ) const
{
   QMimeData* data = QTreeWidget::mimeData( items );

   if (  !items.isEmpty() )
   {
      QByteArray encodedData;
      QDataStream stream( &encodedData, QIODevice::WriteOnly );

      stream << m_isSource;
      stream << items.front()->text( 0 );

      data->setData( "TamyEditor/MappingTreeWidget", encodedData );
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////

void MappedSkeletonTree::dragMoveEvent( QDragMoveEvent* event )
{
   event->ignore();
}

///////////////////////////////////////////////////////////////////////////////

void MappedSkeletonTree::dropEvent( QDropEvent* event )
{
   event->ignore();
}

///////////////////////////////////////////////////////////////////////////////

void MappedSkeletonTree::setSkeleton( const Skeleton* skeleton )
{
   clear();

   m_skeleton = skeleton;
   if ( !skeleton )
   {
      return;
   }

   QString boneIdxStr;
   const uint boneCount = m_skeleton->getBoneCount();
   for ( uint i = 0; i < boneCount; ++i )
   {
      const int boneIdx = m_skeleton->m_bonesUpdateOrder[i];

      boneIdxStr.sprintf( "%d", boneIdx );

      QStringList desc;
      desc.push_back( skeleton->m_boneNames[boneIdx].c_str() );
      desc.push_back( boneIdxStr );
      QTreeWidgetItem* newBoneItem = new QTreeWidgetItem( desc );
     
      const int parentBoneIdx = skeleton->m_boneParentIndices[boneIdx];
      if ( parentBoneIdx >= 0 )
      {
         const char* parentBoneName = skeleton->m_boneNames[parentBoneIdx].c_str();

         QTreeWidgetItem* parentItem = findBoneItem( parentBoneName );
         ASSERT_MSG( parentItem != NULL, "The parent bone item hasn't been created yet - that means this algorithm is flawed" );

         if ( parentItem )
         {
            parentItem->addChild( newBoneItem );
         }
      }
      else
      {
         // this is the root item
         addTopLevelItem( newBoneItem );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* MappedSkeletonTree::findBoneItem( const QString& boneName ) const
{
   List< QTreeWidgetItem* > items;

   {
      const uint count = topLevelItemCount();
      for ( uint i = 0; i < count; ++i )
      {
         items.pushBack( topLevelItem( i ) );
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
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MappedSkeletonFrame::MappedSkeletonFrame( QWidget* parent, bool isSource )
   : QFrame( parent )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   m_skeletonPath = new ResourceDropArea( this );
   layout->addWidget( m_skeletonPath );
   connect( m_skeletonPath, SIGNAL( pathChanged( const FilePath& ) ), this, SLOT( skeletonPathChanged( const FilePath& ) ) );


   m_skeletonTree = new MappedSkeletonTree( this, isSource );
   layout->addWidget( m_skeletonTree, 1 );
}

///////////////////////////////////////////////////////////////////////////////

void MappedSkeletonFrame::setSkeleton( const Skeleton* skeleton )
{
   m_skeletonTree->setSkeleton( skeleton );
   m_skeletonPath->setFilePath( skeleton ? skeleton->getFilePath() : FilePath() );
}

///////////////////////////////////////////////////////////////////////////////

void MappedSkeletonFrame::skeletonPathChanged( const FilePath& path )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   m_skeletonTree->setSkeleton( resMgr.create< Skeleton >( path, true ) );

   emit skeletonChanged();
}

///////////////////////////////////////////////////////////////////////////////
