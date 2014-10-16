#include "core-AI\BehTreeComposite.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeListener.h"
#include "core\List.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BehTreeComposite );
   PARENT( BehTreeNode );
   PROPERTY( Array< BehTreeNode* >,  m_nodes );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeComposite::~BehTreeComposite()
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* node = m_nodes[i];
      if ( node )
      {
         node->removeReference();
      }
   }
   m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::add( BehTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   uint insertionPos = m_nodes.size();
   m_nodes.push_back( node );

   if ( m_tree )
   {
      node->setHostTree( m_tree );

      // notify listeners
      for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BehaviorTreeListener* listener = *it;
         listener->onNodeAdded( this, insertionPos, node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::insert( int index, BehTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   index = clamp<int>( index, 0, m_nodes.size() );
   m_nodes.insert( index, node );
   node->setHostTree( m_tree );

   // notify listeners
   for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
   {
      BehaviorTreeListener* listener = *it;
      listener->onNodeAdded( this, index, node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::remove( BehTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         // found it

         // first - notify listeners
         for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
         {
            BehaviorTreeListener* listener = *it;
            listener->onNodeRemoved( this, node );
         }

         // next - remove the node
         checkedNode->setHostTree( NULL );
         checkedNode->removeReference();
         m_nodes.remove( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::clear()
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* node = m_nodes[i];
      
      // first - notify listeners
      for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BehaviorTreeListener* listener = *it;
         listener->onNodeRemoved( this, node );
      }

      // next - remove the node
      node->removeReference();
   }

   m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::createLayout( BehaviorTreeRunner& runner ) const
{
   // create the layout of the children
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* node = m_nodes[i];
      node->createLayout( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::pullStructure( BehaviorTreeListener* listener )
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* childNode = m_nodes[i];
      listener->onNodeAdded( this, i, childNode );
      childNode->pullStructure( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeComposite::onHostTreeSet( BehaviorTree* tree )
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* childNode = m_nodes[i];
      childNode->setHostTree( tree );
   }
}

///////////////////////////////////////////////////////////////////////////////
