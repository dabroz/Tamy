#include "core-AI\BehTreeDecorator.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeListener.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BehTreeDecorator );
   PARENT( BehTreeNode );
   PROPERTY( BehTreeNode*,  m_childNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeDecorator::BehTreeDecorator()
   : m_childNode( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeDecorator::~BehTreeDecorator()
{
   if ( m_childNode )
   {
      m_childNode->removeReference();
      m_childNode = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::setDecoratedNode( BehTreeNode* child )
{
   if ( m_childNode )
   {
      // notify the listeners about the previous node being removed
      for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BehaviorTreeListener* listener = *it;
         listener->onNodeRemoved( this, m_childNode );
      }

      m_childNode->setHostTree( NULL );
      m_childNode->removeReference();
   }

   m_childNode = child;

   if ( m_childNode && m_tree )
   {
      // set the host tree 
      m_childNode->setHostTree( m_tree );

      // notify the listeners about the new node that's just been added
      for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BehaviorTreeListener* listener = *it;
         listener->onNodeAdded( this, 0, m_childNode );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::initialize( BehaviorTreeRunner& runner ) const
{
   if ( m_childNode )
   {
      m_childNode->initialize( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::deinitialize( BehaviorTreeRunner& runner ) const
{
   if ( m_childNode )
   {
      m_childNode->deinitialize( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::createLayout( BehaviorTreeRunner& runner ) const
{
   if ( m_childNode )
   {
      m_childNode->createLayout( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::pullStructure( BehaviorTreeListener* listener )
{
   if ( m_childNode )
   {
      listener->onNodeAdded( this, 0, m_childNode );
      m_childNode->pullStructure( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeDecorator::onHostTreeSet( BehaviorTree* tree )
{
   if ( m_childNode )
   {
      m_childNode->setHostTree( tree );
   }
}

///////////////////////////////////////////////////////////////////////////////
