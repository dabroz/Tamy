#include "BTIComposite.h"
#include "core-AI\BehTreeComposite.h"


///////////////////////////////////////////////////////////////////////////////

BTIComposite::BTIComposite( BehTreeComposite& composite )
   : m_composite( composite )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode* BTIComposite::getNode()
{
   return &m_composite;
}

///////////////////////////////////////////////////////////////////////////////

void BTIComposite::addNode( BehTreeNode* newNode )
{
   if ( newNode )
   {
      m_composite.add( newNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTIComposite::insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert )
{
   for ( List< BehTreeNode* >::const_iterator it = nodesToInsert.begin(); !it.isEnd(); ++it )
   {
      BehTreeNode* node = *it;

      m_composite.insert( insertionIdx, node );
      ++insertionIdx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTIComposite::removeNode( BehTreeWidgetItem* removedChildItem )
{
   if ( !removedChildItem )
   {
      return;
   }

   BehTreeNode* removedNode = removedChildItem->getNode();
   m_composite.remove( removedNode );
}

///////////////////////////////////////////////////////////////////////////////

void BTIComposite::clear()
{
   m_composite.clear();
}

///////////////////////////////////////////////////////////////////////////////
