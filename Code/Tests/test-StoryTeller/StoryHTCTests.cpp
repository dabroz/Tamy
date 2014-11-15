#include "core-TestFramework\TestFramework.h"
#include "core\Log.h"

// core
#include <string>
#include "core\MemoryRouter.h"
#include "core\Graph.h"
#include "core\List.h"
#include "core\ListUtils.h"

class StoryEvent;

class StoryWorldState
{
public:
   virtual ~StoryWorldState() {}

   virtual void addActor( int role ) = 0;

   virtual  void changeActorsRole( int startRole, int endRole ) = 0;
};

class StoryEvent
{
private:
   std::string                      m_desc;
   List< int >                      m_rolesToAdd;

public:
   ~StoryEvent()
   {
   }

   void apply( StoryWorldState& state ) const
   {
      // introduce the actors
      for ( List< int >::const_iterator it = m_rolesToAdd.begin(); !it.isEnd(); ++it )
      {
         state.addActor( *it );
      }
   }

   void setDesc( const std::string& desc )
   {
      m_desc = desc;
   }

   const std::string& getDesc() const
   {
      return m_desc;
   }

   void addRole( int role )
   {
      m_rolesToAdd.pushBack( role );
   }
};

struct ActorRoleTransition
{
   int      m_startRole;
   int      m_endRole;

   ActorRoleTransition( int startRole, int endRole )
      : m_startRole( startRole )
      , m_endRole( endRole )
   {
   }
};

class StoryEdge
{
private:
   List< ActorRoleTransition >        m_actorRoleTransitions;

public:
   void changeActorRole( int startRole, int endRole )
   {
      m_actorRoleTransitions.pushBack( ActorRoleTransition( startRole, endRole ) );
   }

   void apply( StoryWorldState& state ) const
   {
      for ( List< ActorRoleTransition >::const_iterator it = m_actorRoleTransitions.begin(); !it.isEnd(); ++it )
      {
         const ActorRoleTransition& transition = *it;
         state.changeActorsRole( transition.m_startRole, transition.m_endRole );
      }
   }
};

class StoryTeller
{
private:
   const Graph< StoryEvent* >&   m_storyGraph;

   const StoryEdge*              m_activeStoryEdge;
   int                           m_activeEventIdx;

public:
   StoryTeller( const Graph< StoryEvent* >& storyGraph, int startEventIdx )
      : m_storyGraph( storyGraph )
      , m_activeStoryEdge( NULL )
      , m_activeEventIdx( startEventIdx )
   {
   }

   ~StoryTeller()
   {
   }

   bool step( StoryWorldState& state )
   {
      if ( m_activeEventIdx < 0 )
      {
         // simulation is over
         return false;
      }

      executeStoryEvent( state );

      selectNextEvent();

      return true;
   }

private:
   void executeStoryEvent( StoryWorldState& state )
   {
      // change actor roles
      if ( m_activeStoryEdge )
      {
         m_activeStoryEdge->apply( state );
         m_activeStoryEdge = NULL;
      }

      // apply the event to the state
      StoryEvent* currEvent = m_storyGraph.getNode( m_activeEventIdx );
      currEvent->apply( state );
   }

   void selectNextEvent()
   {
      const Array< GraphEdge* >& outboundEdges = m_storyGraph.getEdges( m_activeEventIdx );
      const uint edgesCount = outboundEdges.size();
      if ( edgesCount == 0 )
      {
         // there are no events that can follow
         m_activeStoryEdge = NULL;
         m_activeEventIdx = -1;
      }
      else
      {
         const int randomEdgeIdx = rand() % edgesCount;
         const GraphEdge* randomEdge = outboundEdges[randomEdgeIdx];
         m_activeStoryEdge = ( const StoryEdge* ) randomEdge->m_userData;
         m_activeEventIdx = randomEdge->m_endNodeIdx;
      }
   }
};

// ------------------------------------------------------------------ 

enum MockActors
{
   Thief,
   Detective,
   Perpetrator,
   MissingBrother,
   ConcernedSister,
   Person,

   NumActors
};

static const char* g_actorIds[NumActors] = {
   "Thief",
   "Detective",
   "Perpetrator",
   "MissingBrother",
   "ConcernedSister",
   "Person",
};

class MockStoryWorldState : public StoryWorldState
{
public:
   std::string       m_log;

   List< int >       m_actors;

public:

   virtual void addActor( int role ) override
   {
      List< int >::iterator it = ListUtils::find( m_actors, role );
      if ( it.isEnd() )
      {
         m_actors.pushBack( role );

         static char tmpStr[256];
         sprintf_s( tmpStr, "Actor %s has entered the scene\n", g_actorIds[role] );

         m_log += tmpStr;
      }
   }

   virtual void changeActorsRole( int startRole, int endRole ) override
   {
      List< int >::iterator startRoleIt = ListUtils::find( m_actors, startRole );
      if ( startRoleIt.isEnd() )
      {
         // start role doesn't exist
         return;
      }

      List< int >::iterator endRoleIt = ListUtils::find( m_actors, endRole );
      if ( !endRoleIt.isEnd() )
      {
         // end role already exists
         return;
      }

      startRoleIt.markForRemoval();
      startRoleIt.processElementsRemoval();

      m_actors.pushBack( endRole );

      static char tmpStr[256];
      sprintf_s( tmpStr, "Actor %s became %s\n", g_actorIds[startRole], g_actorIds[endRole] );

      m_log += tmpStr;
   }

};

///////////////////////////////////////////////////////////////////////////////

TEST( StoryHTC, simple )
{
   StoryEvent theftEvent;
   {
      theftEvent.setDesc( "Theft" );
      theftEvent.addRole( Thief );
   }

   StoryEvent identityChangeEvent;
   {
      identityChangeEvent.setDesc( "Identity change" );
      identityChangeEvent.addRole( Person );
   }

   StoryEvent relationsSeveredEvent;
   {
      relationsSeveredEvent.setDesc( "All relations severed" );
      relationsSeveredEvent.addRole( MissingBrother );
      relationsSeveredEvent.addRole( ConcernedSister );
   }

   StoryEvent investigateEvent;
   {
      investigateEvent.setDesc( "Investigate" );
      investigateEvent.addRole( Perpetrator );
      investigateEvent.addRole( Detective );
   }

   Graph< StoryEvent* > storyGraph;
   int theft = storyGraph.addNode( &theftEvent );
   int relationsSevered = storyGraph.addNode( &relationsSeveredEvent );
   int identityChange = storyGraph.addNode( &identityChangeEvent );
   int investigate = storyGraph.addNode( &investigateEvent );

   GraphEdge* theftToIdentityChange = storyGraph.connect( theft, identityChange );
   {
      StoryEdge* edge = new StoryEdge();
      edge->changeActorRole( Thief, Person );
      theftToIdentityChange->m_userData = edge;
   }

   GraphEdge* identityChangeToRelationsSevered = storyGraph.connect( identityChange, relationsSevered );
   {
      StoryEdge* edge = new StoryEdge();
      edge->changeActorRole( Person, MissingBrother );
      identityChangeToRelationsSevered->m_userData = edge;
   }

   GraphEdge* theftToInvestigation = storyGraph.connect( theft, investigate );
   {
      StoryEdge* edge = new StoryEdge();
      edge->changeActorRole( Thief, Perpetrator );
      theftToInvestigation->m_userData = edge;
   }

   // run the simulation
   StoryTeller storyTeller( storyGraph, theft );
   MockStoryWorldState state;
   while ( storyTeller.step( state ) )
   {
   }

   LOG( state.m_log.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
