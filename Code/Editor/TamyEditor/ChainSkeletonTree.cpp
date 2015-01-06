#include "ChainSkeletonTree.h"

// animation
#include "core-AI\Skeleton.h"

// tools
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

ChainSkeletonTree::ChainSkeletonTree( QWidget* parent )
   : QTreeWidget( parent )
{}

///////////////////////////////////////////////////////////////////////////////

void ChainSkeletonTree::setSkeleton( const Skeleton* skeleton )
{
   clear();
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

QTreeWidgetItem* ChainSkeletonTree::findBoneItem( const QString& boneName ) const
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
