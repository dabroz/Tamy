#include "BDTIComposite.h"
#include "core-AI\BlendTreeComposite.h"


///////////////////////////////////////////////////////////////////////////////

BDTIComposite::BDTIComposite( BlendTreeComposite& composite )
   : m_composite( composite )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeNode* BDTIComposite::getNode()
{
   return &m_composite;
}

///////////////////////////////////////////////////////////////////////////////

void BDTIComposite::addNode( BlendTreeNode* newNode )
{
   if ( newNode )
   {
      m_composite.add( newNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BDTIComposite::insertNodes( int insertionIdx, const List< BlendTreeNode* >& nodesToInsert )
{
   for ( List< BlendTreeNode* >::const_iterator it = nodesToInsert.begin(); !it.isEnd(); ++it )
   {
      BlendTreeNode* node = *it;

      m_composite.insert( insertionIdx, node );
      ++insertionIdx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BDTIComposite::removeNode( BlendTreeWidgetItem* removedChildItem )
{
   if ( !removedChildItem )
   {
      return;
   }

   BlendTreeNode* removedNode = removedChildItem->getNode();
   m_composite.remove( removedNode );
}

///////////////////////////////////////////////////////////////////////////////

void BDTIComposite::clear()
{
   m_composite.clear();
}

///////////////////////////////////////////////////////////////////////////////
