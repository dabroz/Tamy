#include "ext-StoryTeller\SAActivateChapter.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\StoryChapterStart.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SAActivateChapter );
   PARENT( StoryAction );
   PROPERTY_EDIT_REFERENCE( "Chapter", StoryChapter*, m_chapter );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SAActivateChapter::SAActivateChapter( StoryChapter* chapter )
   : m_chapter( chapter )
   , m_out( NULL )
{
   registerReferences();

   if ( !IS_BEING_SERIALIZED() )
   {
      defineInput( new SAVoidInput( "In" ) );

      m_out = new SAVoidOutput( "Out" );
      defineOutput( m_out );
   }
}

///////////////////////////////////////////////////////////////////////////////

SAActivateChapter::SAActivateChapter( const SAActivateChapter& rhs )
   : StoryAction( rhs )
   , m_chapter( rhs.m_chapter )
   , m_out( NULL )
{
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

void SAActivateChapter::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

void SAActivateChapter::onCreateLayout( StoryPlayer& player ) const
{
   RuntimeDataBuffer& data = player.data();
   data.registerVar( m_nodesToActivate );
   data.registerVar( m_activeNodes );
   data.registerVar( m_graph );
   data.registerVar( m_spawnedEnvironment );

   // create the layout of the children
   uint count = m_chapter->m_actions.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryAction* action = m_chapter->m_actions[i];
      action->createLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SAActivateChapter::onDestroyLayout( StoryPlayer& player ) const
{
   // create the layout of the children
   uint count = m_chapter->m_actions.size();
   for ( uint i = 0; i < count; ++i )
   {
      StoryAction* action = m_chapter->m_actions[i];
      action->destroyLayout( player );
   }

   RuntimeDataBuffer& data = player.data();
   data.unregisterVar( m_spawnedEnvironment );
   data.unregisterVar( m_graph );
   data.unregisterVar( m_activeNodes );
   data.unregisterVar( m_nodesToActivate );
}

///////////////////////////////////////////////////////////////////////////////

void SAActivateChapter::initialize( StoryPlayer& player ) const
{
   RuntimeDataBuffer& data = player.data();

   // instantiate the graph
   StoryChapterGraph* graph = new StoryChapterGraph();
   data[m_graph] = graph;
   m_chapter->buildGraph( *graph );

   // put the start action on the list
   NodesList* nodesToActivate = new NodesList();
   data[m_nodesToActivate] = nodesToActivate;

   NodesList* activeNodes = new NodesList();
   data[m_activeNodes] = activeNodes;

   // the following assumes we have only 1 start node, that's never going to be removed
   // and will always be the first element on the nodes list.
   nodesToActivate->pushBack( graph->getNode(0) );

   // spawn the environment
   StoryNodeInstance* spawnedEnvironment = m_chapter->instantiate();
   data[m_spawnedEnvironment] = spawnedEnvironment;
   if ( spawnedEnvironment )
   {
      spawnedEnvironment->addReference();
      player.gameWorld().addChild( spawnedEnvironment );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SAActivateChapter::deinitialize( StoryPlayer& player )  const
{
   RuntimeDataBuffer& data = player.data();

   // despawn the environment
   StoryNodeInstance* spawnedEnvironment = data[m_spawnedEnvironment];
   data[m_spawnedEnvironment] = NULL;
   if ( spawnedEnvironment )
   {
      spawnedEnvironment->remove();
      spawnedEnvironment->removeReference();
      spawnedEnvironment = NULL;
   }

   // deactivate nodes
   NodesList* activeNodes = data[m_activeNodes];
   if ( activeNodes )
   {
      for ( NodesList::iterator it = activeNodes->begin(); !it.isEnd(); ++it )
      {
         StoryAction* action = *it;
         action->deinitialize( player );
      }
   }
   delete activeNodes;
   data[m_activeNodes] = NULL;

   NodesList* nodesToActivate = data[m_nodesToActivate];
   delete nodesToActivate;
   data[m_nodesToActivate] = NULL;

   StoryChapterGraph* graph = data[m_graph];
   delete graph;
   data[m_graph] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool SAActivateChapter::execute( StoryPlayer& player, List< StoryAction* >& outActionsToActivate ) const
{
   RuntimeDataBuffer& data = player.data();
   NodesList* nodesToActivate = data[m_nodesToActivate];
   NodesList* activeNodes = data[m_activeNodes];

   // activate new actions
   for ( NodesList::iterator it = nodesToActivate->begin(); !it.isEnd(); ++it )
   {
      StoryAction* action = *it;
      action->initialize( player );
      activeNodes->pushBack( action );
   }
   nodesToActivate->clear();

   // run active actions
   StoryChapterGraph* graph = data[m_graph];
   for ( NodesList::iterator it = activeNodes->begin(); !it.isEnd(); ++it )
   {
      StoryAction* action = *it;

      bool hasFinished = action->execute( player, *nodesToActivate );
      if ( hasFinished )
      {
         action->deinitialize( player );
         it.markForRemoval();
      } 
   }

   if ( nodesToActivate->empty() && activeNodes->empty() )
   {
      m_out->collectConnectedNodes( outActionsToActivate ); 
      return true;
   }
   else
   {
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
