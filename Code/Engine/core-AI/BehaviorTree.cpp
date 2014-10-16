#include "core-AI\BehaviorTree.h"
#include "core-AI\BehTreeSequence.h"
#include "core-AI\BehTreeVariable.h"
#include "core-AI\BehaviorTreeListener.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( BehaviorTree, tbt, AM_BINARY );
   PROPERTY( BehTreeComposite*, m_root );
   PROPERTY( Array< BehaviorTreeVariable* >, m_variables );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

BehaviorTree::BehaviorTree( const FilePath& path )
   : Resource( path )
   , m_root( NULL )
{
   Thread& currThread = TSingleton< ThreadSystem >::getInstance().getCurrentThread();
   if ( !currThread.m_serializationInProgress )
   {
      m_root = new BehTreeSequence();
      m_root->setName( "Root" );
      m_root->setHostTree( this );    
   }
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTree::~BehaviorTree()
{
   m_listeners.clear();

   m_root->removeReference();
   m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::attachListener( BehaviorTreeListener* listener )
{
   // check for duplicates
   for ( List< BehaviorTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BehaviorTreeListener* checkedListener = *it;
      if ( checkedListener == listener )
      {
         // it's a duplicate
         return;
      }
   }

   m_listeners.pushBack( listener );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::detachListener( BehaviorTreeListener* listener )
{
   for ( List< BehaviorTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BehaviorTreeListener* checkedListener = *it;
      if ( checkedListener == listener )
      {
         it.markForRemoval();
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::pullStructure( BehaviorTreeListener* listener )
{
   m_root->pullStructure( listener );

   // inform the listener about variables
   uint count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehaviorTreeVariable* var = m_variables[i];
      listener->onVariableAdded( var );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::addVariable( BehaviorTreeVariable* btVariable )
{
   if ( !btVariable )
   {
      return;
   }
   uint variableIdx = m_variables.size();
   m_variables.push_back( btVariable );

   // notify listeners that a new event has been added
   for ( List< BehaviorTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BehaviorTreeListener* listener = *it;
      listener->onVariableAdded( btVariable );
   }

   // assign the event an index
   btVariable->setIndex( variableIdx );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::removeVariable( BehaviorTreeVariable* btVariable )
{
   if ( !btVariable )
   {
      return;
   }

   uint count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehaviorTreeVariable* checkedVariable = m_variables[i];
      if ( checkedVariable == btVariable )
      {
         // notify listeners that a new event has been added
         for ( List< BehaviorTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            BehaviorTreeListener* listener = *it;
            listener->onVariableRemoved( btVariable );
         }

         btVariable->removeReference();
         m_variables.remove( i );

         // re-index the remaining variables
         indexVariables( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::indexVariables( uint startIdx )
{
   uint count = m_variables.size();
   for ( uint i = startIdx; i < count; ++i )
   {
      BehaviorTreeVariable* btVariable = m_variables[i];
      btVariable->setIndex( i );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::replaceContents( Resource& rhs )
{
   BehaviorTree& rhsBehaviorTree = static_cast< BehaviorTree& >( rhs );

   m_root = rhsBehaviorTree.m_root;
   m_variables = rhsBehaviorTree.m_variables;

   // we've just taken the ownership of the deserialized data, so we need to
   // erase this from the rhs entity so that they don't get deleted in a sec
   rhsBehaviorTree.m_root = NULL;
   rhsBehaviorTree.m_variables.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::onResourceLoaded( ResourcesManager& resMgr )
{
   indexVariables( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::createLayout( BehaviorTreeRunner* player )
{
   m_root->createLayout( *player );

   uint count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehaviorTreeVariable* btVariable = m_variables[i];
      btVariable->initializeLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTree::destroyLayout( BehaviorTreeRunner* player )
{
}

///////////////////////////////////////////////////////////////////////////////
