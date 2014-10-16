#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-AI\BlendTreeListener.h"
#include "core-AI\Skeleton.h"
#include "core\ListUtils.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( BlendTree, tblt, AM_BINARY );
   PROPERTY_EDIT( "Skeleton", Skeleton*, m_skeleton );
   PROPERTY( Array< BlendTreeEvent* >, m_events );
   PROPERTY( Array< BlendTreeVariable* >, m_variables );
   PROPERTY( BlendTreeStateMachine*, m_root );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

BlendTree::BlendTree( const FilePath& path )
   : Resource( path )
   , m_skeleton( NULL )
   , m_root( NULL )
{
   Thread& currThread = TSingleton< ThreadSystem >::getInstance().getCurrentThread();
   if ( !currThread.m_serializationInProgress )
   {
      m_root = new BlendTreeStateMachine();
      m_root->setName( "Root" );
      m_root->setHostTree( this );    
   }
}

///////////////////////////////////////////////////////////////////////////////

BlendTree::~BlendTree()
{
   uint count = m_events.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeEvent* btEvent = m_events[i];
      btEvent->removeReference();
   }
   m_events.clear();

   count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeVariable* btVariable = m_variables[i];
      btVariable->removeReference();
   }
   m_variables.clear();

   m_listeners.clear();

   m_skeleton = NULL;

   m_root->removeReference();
   m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::attachListener( BlendTreeListener* listener )
{
   List< BlendTreeListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::detachListener( BlendTreeListener* listener )
{
   List< BlendTreeListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::pullStructure( BlendTreeListener* listener )
{
   m_root->pullStructure( listener );

   // notify about existing events
   uint count = m_events.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeEvent* btEvent = m_events[i];
      listener->onEventAdded( btEvent );
   }

   // notify about existing variables
   count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeVariable* btVariable = m_variables[i];
      listener->onVariableAdded( btVariable );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::setSkeleton( Skeleton* skeleton )
{
   NOTIFY_PROPERTY_CHANGE( m_skeleton );

   m_skeleton = skeleton;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::addEvent( BlendTreeEvent* btEvent )
{
   if ( !btEvent )
   {
      return;
   }
   uint eventIdx = m_events.size();
   m_events.push_back( btEvent );

   // notify listeners that a new event has been added
   for ( List< BlendTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreeListener* listener = *it;
      listener->onEventAdded( btEvent );
   }

   // assign the event an index
   btEvent->setIndex( eventIdx );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::removeEvent( BlendTreeEvent* btEvent )
{
   if ( !btEvent )
   {
      return;
   }

   uint count = m_events.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeEvent* checkedEvent = m_events[i];
      if ( checkedEvent == btEvent )
      {
         // notify listeners that a new event has been added
         for ( List< BlendTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeListener* listener = *it;
            listener->onEventRemoved( btEvent );
         }

         btEvent->removeReference();
         m_events.remove( i );

         // re-index the remaining events
         indexEvents( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::indexEvents( uint startIdx )
{
   uint count = m_events.size();
   for ( uint i = startIdx; i < count; ++i )
   {
      BlendTreeEvent* btEvent = m_events[i];
      btEvent->setIndex( i );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::addVariable( BlendTreeVariable* btVariable )
{
   if ( !btVariable )
   {
      return;
   }
   uint variableIdx = m_variables.size();
   m_variables.push_back( btVariable );

   // notify listeners that a new event has been added
   for ( List< BlendTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreeListener* listener = *it;
      listener->onVariableAdded( btVariable );
   }

   // assign the event an index
   btVariable->setIndex( variableIdx );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::removeVariable( BlendTreeVariable* btVariable )
{
   if ( !btVariable )
   {
      return;
   }

   uint count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeVariable* checkedVariable = m_variables[i];
      if ( checkedVariable == btVariable )
      {
         // notify listeners that a new event has been added
         for ( List< BlendTreeListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeListener* listener = *it;
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

void BlendTree::indexVariables( uint startIdx )
{
   uint count = m_variables.size();
   for ( uint i = startIdx; i < count; ++i )
   {
      BlendTreeVariable* btVariable = m_variables[i];
      btVariable->setIndex( i );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::replaceContents( Resource& rhs )
{
   BlendTree& rhsBlendTree = static_cast< BlendTree& >( rhs );

   m_skeleton = rhsBlendTree.m_skeleton;
   m_events = rhsBlendTree.m_events;
   m_variables = rhsBlendTree.m_variables;
   m_root = rhsBlendTree.m_root;

   // we've just taken the ownership of the deserialized data, so we need to
   // erase this from the rhs entity so that they don't get deleted in a sec
   rhsBlendTree.m_skeleton = NULL;
   rhsBlendTree.m_root = NULL;
   rhsBlendTree.m_events.clear();
   rhsBlendTree.m_variables.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::onResourceLoaded( ResourcesManager& resMgr )
{
   indexEvents( 0 );
   indexVariables( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::initializeLayout( BlendTreePlayer* player ) const
{
   // initialize variables
   uint varsCount = m_variables.size();
   for ( uint i = 0; i < varsCount; ++i )
   {
      BlendTreeVariable* btVariable = m_variables[i];
      btVariable->initializeLayout( player );
   }

   // initialize the state machine
   m_root->initializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTree::deinitializeLayout( BlendTreePlayer* player ) const
{
   // deinitialize the state machine
   m_root->deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////
