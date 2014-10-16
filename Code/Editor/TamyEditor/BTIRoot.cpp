#include "BTIRoot.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehTreeComposite.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"


///////////////////////////////////////////////////////////////////////////////

BTIRoot::BTIRoot( QTreeWidget* parentTreeWidget, BehaviorTree& tree )
   : m_tree( tree )
{
   parentTreeWidget->addTopLevelItem( this );

   setIcon( 0, QIcon( ":/TamyEditor/Resources/Icons/Engine/BehTreeRootIcon.png" ) );
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode* BTIRoot::getNode()
{
   return &m_tree.getRoot();
}

///////////////////////////////////////////////////////////////////////////////

void BTIRoot::addNode( BehTreeNode* newNode )
{
   if ( newNode )
   {
      m_tree.getRoot().add( newNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTIRoot::insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert )
{
   BehTreeComposite& root = m_tree.getRoot();

   for ( List< BehTreeNode* >::const_iterator it = nodesToInsert.begin(); !it.isEnd(); ++it )
   {
      BehTreeNode* node = *it;

      root.insert( insertionIdx, node );
      ++insertionIdx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTIRoot::removeNode( BehTreeWidgetItem* removedChildItem )
{
   if ( !removedChildItem )
   {
      return;
   }

   BehTreeNode* removedNode = removedChildItem->getNode();
   m_tree.getRoot().remove( removedNode );
}

///////////////////////////////////////////////////////////////////////////////

void BTIRoot::clear()
{
   m_tree.getRoot().clear();
}

///////////////////////////////////////////////////////////////////////////////
