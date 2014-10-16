#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryListener.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryChapterStart.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core-MVC\Prefab.h"
#include "core\List.h"
#include "core\Algorithms.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StoryChapter );
   PARENT( StoryNode );
   PROPERTY( Array< StoryNode* >, m_storyNodes );
   PROPERTY( std::vector< StoryAction* >, m_actions );
   PROPERTY_EDIT( "Environment", Prefab*, m_environment );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryChapter::StoryChapter()
   : m_environment( NULL )
{
   setGraphBuilderNodes( &m_actions );

   if ( IS_BEING_SERIALIZED() == false )
   {
      // this node is being genuinely created, so initialize the graph
      StoryGraphTransaction transaction( *this );
      transaction.addNode( new StoryChapterStart() );

      transaction.commit();
   }
}

///////////////////////////////////////////////////////////////////////////////

StoryChapter::~StoryChapter()
{
   // delete nodes
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* node = m_storyNodes[i];
      if ( node )
      {
         node->removeReference();
      }
   }
   m_storyNodes.clear();

   // delete actions
   count = m_actions.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryAction* action = m_actions[i];
      if ( action )
      {
         action->removeReference();
      }
   }
   m_actions.clear();

   // environment resource will be removed by the resources manager
   m_environment = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::onObjectLoaded()
{
   setGraphBuilderNodes( &m_actions );
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::add( StoryNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* checkedNode = m_storyNodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   uint insertionPos = m_storyNodes.size();
   m_storyNodes.push_back( node );

   if ( m_story )
   {
      node->setHostStory( m_story );

      // notify listeners
      for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
      {
         StoryListener* listener = *it;
         listener->onNodeAdded( this, insertionPos, node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::insert( int index, StoryNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* checkedNode = m_storyNodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   index = clamp<int>( index, 0, m_storyNodes.size() );
   m_storyNodes.insert( index, node );
   node->setHostStory( m_story );

   // notify listeners
   for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
   {
      StoryListener* listener = *it;
      listener->onNodeAdded( this, index, node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::remove( StoryNode* node )
{
   if ( !node )
   {
      return;
   }

   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* checkedNode = m_storyNodes[i];
      if ( checkedNode == node )
      {
         // found it

         // first - notify listeners
         for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
         {
            StoryListener* listener = *it;
            listener->onNodeRemoved( this, node );
         }

         // last - remove the node
         checkedNode->setHostStory( NULL );
         checkedNode->removeReference();
         m_storyNodes.remove( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::clear()
{
   // delete nodes
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* node = m_storyNodes[i];

      // first - notify listeners
      for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
      {
         StoryListener* listener = *it;
         listener->onNodeRemoved( this, node );
      }

      // next - remove the node
      node->removeReference();
   }
   m_storyNodes.clear();

   // delete actions
   count = m_actions.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryAction* action = m_actions[i];
      action->removeReference();
   }
   m_actions.clear();
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::pullStructure( StoryListener* listener )
{
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* childNode = m_storyNodes[i];
      listener->onNodeAdded( this, i, childNode );
      childNode->pullStructure( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapter::onHostStorySet( Story* story )
{
   uint count = m_storyNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryNode* childNode = m_storyNodes[i];
      childNode->setHostStory( story );
   }
}

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance* StoryChapter::instantiate()
{
   // instantiate entities from this chapter
   StoryNodeInstance* envModel = NULL;
   if ( m_environment )
   {
      envModel = new StoryNodeInstance( m_environment, this, getName().c_str() );
   }

   return envModel;
}

///////////////////////////////////////////////////////////////////////////////
