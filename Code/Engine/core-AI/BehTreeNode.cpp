#include "core-AI\BehTreeNode.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeListener.h"
#include "core\List.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BehTreeNode );
   PARENT( ReflectionObject );
   PROPERTY( BehaviorTree*, m_tree );
   PROPERTY_EDIT( "Node name", std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::BehTreeNode()
   : m_tree( NULL )
   , m_name( "New node" )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeNode::setHostTree( BehaviorTree* tree )
{
   ASSERT_MSG( ( tree != NULL && m_tree == NULL ) || ( tree == NULL && m_tree != NULL ) , "This node is already a part of a behavior tree" );

   m_tree = tree;
   onHostTreeSet( tree );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeNode::setName( const char* nodeName )
{
   if ( nodeName )
   {
      m_name = nodeName;
   }
   else
   {
      m_name = "";
   }

   // notify listeners
   if ( m_tree )
   {
      for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BehaviorTreeListener* listener = *it;
         listener->onNodeChanged( this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeNode::onPropertyChanged( ReflectionProperty& property )
{
   // notify listeners
   for ( List< BehaviorTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
   {
      BehaviorTreeListener* listener = *it;
      listener->onNodeChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////
