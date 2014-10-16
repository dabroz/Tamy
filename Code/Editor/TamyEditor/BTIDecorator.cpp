#include "BTIDecorator.h"
#include "core-AI\BehTreeDecorator.h"
#include "core-AI\BehTreeSequence.h"


///////////////////////////////////////////////////////////////////////////////

BTIDecorator::BTIDecorator( BehTreeDecorator& decorator )
   : m_decorator( decorator )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode* BTIDecorator::getNode()
{
   return &m_decorator;
}

///////////////////////////////////////////////////////////////////////////////

void BTIDecorator::addNode( BehTreeNode* newNode )
{
   if ( newNode )
   {
      m_decorator.setDecoratedNode( newNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTIDecorator::insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert )
{
   BehTreeNode* newNode = NULL;

   if ( nodesToInsert.size() == 1 )
   {
      newNode = nodesToInsert.front();
   }
   else
   {
      // wrap the existing node in a sequence
      BehTreeSequence* sequence = new BehTreeSequence();
      for ( List< BehTreeNode* >::const_iterator it = nodesToInsert.begin(); !it.isEnd(); ++it )
      {
         BehTreeNode* node = *it;
         sequence->add( node );
      }

      newNode = sequence;
   }

   // replace the previously wrapped node with the new one
   m_decorator.setDecoratedNode( newNode );
}

///////////////////////////////////////////////////////////////////////////////

void BTIDecorator::removeNode( BehTreeWidgetItem* removedChildItem )
{
   if ( !removedChildItem )
   {
      return;
   }

   BehTreeNode* removedNode = removedChildItem->getNode();
   ASSERT_MSG( removedNode == m_decorator.getDecoratedNode(), "This is not the embedded node" );

   m_decorator.setDecoratedNode( NULL );
}

///////////////////////////////////////////////////////////////////////////////

void BTIDecorator::clear()
{
   m_decorator.setDecoratedNode( NULL );
}

///////////////////////////////////////////////////////////////////////////////
