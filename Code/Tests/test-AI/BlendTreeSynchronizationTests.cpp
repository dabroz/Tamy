#include "core-TestFramework\TestFramework.h"
#include "core-AI\BlendTreeAnimationTrack.h"
#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeSyncProfile.h"


///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, noSyncPoints )
{
   BlendTreeEvent event1;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;
   treeSyncProfile.submit( &event1, 0.0f );

   const float duration = 4.0f;
   Array< BlendTreeSyncPoint* > syncPoints;
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, false );

   // try finding a good starting point for the node
   CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime ); // track time hasn't changed

   // try creating a node sync profile
   BlendTreeNodeSyncProfile nodeSyncProfile( NULL );
   timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
   CPPUNIT_ASSERT_EQUAL( (uint)0, nodeSyncProfile.m_nodesCount );
   CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime );

   // try updating the track
   bool didAnimationLoopAround = timetrackManager.update( 1.0f );
   CPPUNIT_ASSERT( !didAnimationLoopAround );
   CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );

   // last but not least - try updating the tree sync profile ( use a new one, so that we can clearly see the changes )
   BlendTreeSyncProfile updatedTreeSyncProfile;
   timetrackManager.updateTreeProfile( updatedTreeSyncProfile );
   CPPUNIT_ASSERT_EQUAL( (uint)0, updatedTreeSyncProfile.m_eventsCount );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, nonLoopingAnimationWithOneEvent )
{
   BlendTreeEvent event1;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;
   treeSyncProfile.submit( &event1, 0.0f );

   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.5f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, false );

   bool didAnimationLoopAround = false;
   BlendTreeNodeSyncProfile nodeSyncProfile( NULL );

   // frame 0
   {
      // try finding a good starting point for the node
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime ); // track time hasn't changed

      // try creating a node sync profile
      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.m_trackTime );
   }

   // frame 1
   {
      // last but not least - try updating the tree sync profile ( use a new one, so that we can clearly see the changes )
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, treeSyncProfile.m_eventsCount );
      CPPUNIT_ASSERT_EQUAL( &event1, treeSyncProfile.m_btEvent[0] );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, treeSyncProfile.m_progress[0], 0.1f );

      // at this point there's only our node that generates sync info, so we're testing if it'll be able to
      // recreate its own sync profile from the sync data ( we'll test the scenario where we get a different sync profile altogether 
      // later on )
      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );
   }

   // frame 2
   {
      // we entered the "no sync points" ground
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, treeSyncProfile.m_eventsCount );


      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.m_trackTime );
   }

   // frame 3
   {
      // reaching the end of the track ( no looping there )
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, treeSyncProfile.m_eventsCount );


      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 2.0f, timetrackManager.m_trackTime );
   }

   // frame 4
   {
      // we're past the end of the track, and since the track doesn't loop around - nothing happens
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, treeSyncProfile.m_eventsCount );


      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)0, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 2.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 2.0f, timetrackManager.m_trackTime );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, loopingAnimationWithOneEvent )
{
   BlendTreeEvent event1;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;
   treeSyncProfile.submit( &event1, 0.0f );

   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.5f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, true );

   bool didAnimationLoopAround = false;
   BlendTreeNodeSyncProfile nodeSyncProfile( NULL );

   // frame 0
   {
      // try finding a good starting point for the node - since our sync point now loops around the back,
      // progress 0.0f  corresponds to it's start/end point on the track ( since there's only one point there )
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime ); // track time hasn't changed

      // try creating a node sync profile
      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.m_trackTime );
   }

   // frame 1
   {
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, treeSyncProfile.m_eventsCount );
      CPPUNIT_ASSERT_EQUAL( &event1, treeSyncProfile.m_btEvent[0] );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.25f, treeSyncProfile.m_progress[0], 0.1f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime );  // track looped around
   }

   // frame 2
   {
      // we entered the "no sync points" ground
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, treeSyncProfile.m_eventsCount );
      CPPUNIT_ASSERT_EQUAL( &event1, treeSyncProfile.m_btEvent[0] );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, treeSyncProfile.m_progress[0], 0.1f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.m_trackTime );
   }

   // frame 3
   {
      // reaching the end of the track, but it's gonna loop around
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, treeSyncProfile.m_eventsCount );
      CPPUNIT_ASSERT_EQUAL( &event1, treeSyncProfile.m_btEvent[0] );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75f, treeSyncProfile.m_progress[0], 0.1f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.m_trackTime );

      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround ); 
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );
   }

   // frame 4
   {
      // we're back at the start - same events and everything as in frame 0
      treeSyncProfile.reset();
      timetrackManager.updateTreeProfile( treeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, treeSyncProfile.m_eventsCount );
      CPPUNIT_ASSERT_EQUAL( &event1, treeSyncProfile.m_btEvent[0] );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, treeSyncProfile.m_progress[0], 0.1f );


      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );
      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.m_trackTime );

      // try updating the track
      didAnimationLoopAround = timetrackManager.update( 0.5f );
      CPPUNIT_ASSERT( !didAnimationLoopAround );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.m_trackTime );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, jumpToFirstMatch_nonLoopingAnimation )
{
   BlendTreeEvent event1;
   BlendTreeEvent event2;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;
   
   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.25f );
   BlendTreeSyncPoint syncPoint2( &event2, 0.75f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   syncPoints.push_back( &syncPoint2 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, false );

   // case 1
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 2
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.5f );
      CPPUNIT_ASSERT_EQUAL( 0.25f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 3
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 1.0f );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 4
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 5
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.5f );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 6
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 1.0f );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, jumpToFirstMatch_loopingAnimation )
{
   BlendTreeEvent event1;
   BlendTreeEvent event2;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;

   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.25f );
   BlendTreeSyncPoint syncPoint2( &event2, 0.75f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   syncPoints.push_back( &syncPoint2 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, true );

   // case 1
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 2
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.5f );
      CPPUNIT_ASSERT_EQUAL( 0.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 3
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 1.0f );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 4
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );
      CPPUNIT_ASSERT_EQUAL( 0.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 5
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.5f );
      CPPUNIT_ASSERT_EQUAL( 1.0f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }

   // case 6
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 1.0f );
      CPPUNIT_ASSERT_EQUAL( 1.5f, timetrackManager.jumpToFirstMatch( treeSyncProfile ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, synchronizeTo_nonLoopingAnimation )
{
   BlendTreeEvent event1;
   BlendTreeEvent event2;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;

   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.25f );
   BlendTreeSyncPoint syncPoint2( &event2, 0.75f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   syncPoints.push_back( &syncPoint2 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, false );

   BlendTreeNodeSyncProfile nodeSyncProfile( NULL );

   // case 1 - syncing to event1, track at 0.0f
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 2 - syncing to event1, track at 0.25f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.5f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.25f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 3
   // now we're gonna test a case where we somehow got out of sync - we expect the returned remaining
   // time will be equal the actual remaining time until the specified sync event is hit
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.5f ); // even though we're still 0.5 way through event1, the tree wants us to synchronize
                                               // to event2, which is currently at 0.5 its progress

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.25f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 4 - syncing to event2, track at 0.5f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );
      
      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 5 - syncing to event2, track at 0.75f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 6
   // And again - the tree wants us to synchronize to another event. This time it's event1.
   // The problem is that event2 we;'re currently playing is the last event ( the track doesn't loop around )
   // so we can't synchronize
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)0, nodeSyncProfile.m_nodesCount );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimationTrack, synchronizeTo_loopingAnimation )
{
   BlendTreeEvent event1;
   BlendTreeEvent event2;

   // create a tree sync profile
   BlendTreeSyncProfile treeSyncProfile;

   const float duration = 2.0f;
   BlendTreeSyncPoint syncPoint1( &event1, 0.25f );
   BlendTreeSyncPoint syncPoint2( &event2, 0.75f );

   Array< BlendTreeSyncPoint* > syncPoints;
   syncPoints.push_back( &syncPoint1 );
   syncPoints.push_back( &syncPoint2 );
   BlendTreeAnimationTrack timetrackManager( syncPoints, duration, true );

   BlendTreeNodeSyncProfile nodeSyncProfile( NULL );

   // case 1 - syncing to event1, track at 0.0f
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 2 - syncing to event1, track at 0.25f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.5f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.25f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 3
   // now we're gonna test a case where we somehow got out of sync - we expect the returned remaining
   // time will be equal the actual remaining time until the specified sync event is hit
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.5f ); // even though we're still 0.5 way through event1, the tree wants us to synchronize
      // to event2, which is currently at 0.5 its progress

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.25f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 4 - syncing to event2, track at 0.5f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 5 - syncing to event2, track at 0.75f
   {
      timetrackManager.update( 0.25f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 6 - here's where it's different with respect to a track that doesn't loop.
   // We're requesting a sync to another event - in this case on that loops around the back.
   // Before - this event didn't exist, now - it loops around the back, so we can easily sample 
   // a time it would take to reach it
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.75f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 7 - syncing to event1, track at 1.75f
   // moving on, we're entering the domain of the event that was looped around the back
   {
      timetrackManager.update( 1.0f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.75f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 8 - now see if we can sync to event2 from this place ( 1.75s on time track ). 
   // To reach it, we'd need to loop around the back and iterate to the next event - so see
   // if our algo is capable of fulfilling this goal
   {
      treeSyncProfile.reset();
      treeSyncProfile.submit( &event2, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.75f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }

   // case 9 - back to regular scheduled programming.
   // Syncing to event1, track at 0.25f ( looped around the back )
   // moving on, we're entering the domain of the event that was looped around the back
   {
      timetrackManager.update( 0.5f );

      treeSyncProfile.reset();
      treeSyncProfile.submit( &event1, 0.0f );

      nodeSyncProfile.reset();
      timetrackManager.synchronizeTo( treeSyncProfile, nodeSyncProfile );

      CPPUNIT_ASSERT_EQUAL( (uint)1, nodeSyncProfile.m_nodesCount );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.25f, nodeSyncProfile.m_timeRemaining[0], 0.1f );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeSyncProfile, mergingSyncData )
{
   BlendTreeSyncProfile syncData;

   //  node    events
   // ---------------------------
   // node 0 - 0.5% of L
   // node 1 - 0.6% of A
   // node 2 - 0.3% of L ( out of sync with node 0 )
   // node 3 - 0.6% of A ( in sync with node 1 )

   // tree setup
   //
   // root           { 0.5% of L, 0.6% of A }
   //    blender_1
   //       node0
   //       node1
   //    blender_2
   //       node2
   //       node3

   BlendTreeEvent syncPoint_L;
   BlendTreeEvent syncPoint_A;

   // let's simulate how the update would go

   // blender_1
   syncData.submit( &syncPoint_L, 0.5f );
   syncData.submit( &syncPoint_A, 0.6f );

   // blender_2
   syncData.submit( &syncPoint_L, 0.3f );
   syncData.submit( &syncPoint_A, 0.6f );

   // and test the results
   CPPUNIT_ASSERT_EQUAL( (uint)2, syncData.m_eventsCount );

   CPPUNIT_ASSERT_EQUAL( &syncPoint_L, syncData.m_btEvent[0] );
   CPPUNIT_ASSERT_EQUAL( 0.5f, syncData.m_progress[0] );

   CPPUNIT_ASSERT_EQUAL( &syncPoint_A, syncData.m_btEvent[1] );
   CPPUNIT_ASSERT_EQUAL( 0.6f, syncData.m_progress[1] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeNodeSyncProfile, mergingData )
{
   BlendTreeNodeSyncProfile dataForNode1( NULL );
   BlendTreeNodeSyncProfile dataForNode2( NULL );
   BlendTreeNodeSyncProfile mergedData( NULL );

   // same event ( normalized blend weights )
   {
      dataForNode1.reset();
      dataForNode2.reset();
      mergedData.reset();

      dataForNode1.setSyncPoint( 0, 1.0f );
      dataForNode2.setSyncPoint( 0, 2.0f );

      mergedData.mergeWith( &dataForNode1, 0.5f );
      mergedData.mergeWith( &dataForNode2, 0.5f );
      mergedData.commit();

      CPPUNIT_ASSERT_EQUAL( 1.5f, mergedData.m_timeRemaining[0] );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6f, mergedData.m_playbackSpeed[0], 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.3f, mergedData.m_playbackSpeed[1], 0.1f );
   }

   // same event ( unnormalized blend weights )
   {
      dataForNode1.reset();
      dataForNode2.reset();
      mergedData.reset();

      dataForNode1.setSyncPoint( 0, 1.0f );
      dataForNode2.setSyncPoint( 0, 2.0f );

      mergedData.mergeWith( &dataForNode1, 1.0f );
      mergedData.mergeWith( &dataForNode2, 1.0f );
      mergedData.commit();

      CPPUNIT_ASSERT_EQUAL( 1.5f, mergedData.m_timeRemaining[0] );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6f, mergedData.m_playbackSpeed[0], 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.3f, mergedData.m_playbackSpeed[1], 0.1f );
   }

   // different events ( normalized blend weights )
   {
      dataForNode1.reset();
      dataForNode2.reset();
      mergedData.reset();

      dataForNode1.setSyncPoint( 0, 1.0f );
      dataForNode2.setSyncPoint( 1, 2.0f );

      mergedData.mergeWith( &dataForNode1, 1.0f );
      mergedData.mergeWith( &dataForNode2, 1.0f );
      mergedData.commit();

      CPPUNIT_ASSERT_EQUAL( 1.0f, mergedData.m_timeRemaining[0] );
      CPPUNIT_ASSERT_EQUAL( 2.0f, mergedData.m_timeRemaining[1] );

      // since there are two events, but we can only synchronize using one, we're selecting the first one
      // and synchronizing to it
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, mergedData.m_playbackSpeed[0], 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, mergedData.m_playbackSpeed[1], 0.1f ); // both nodes end up running at speed = 1.0f
   }
}

///////////////////////////////////////////////////////////////////////////////
