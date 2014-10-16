#include "core-AI\BlendTreeAnimationTrack.h"
#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimationTrack::BlendTreeAnimationTrack( const Array< BlendTreeSyncPoint* >& syncPoints, float duration, bool looped )
   : m_head( NULL )
   , m_tail( NULL )
   , m_trackTime( 0.0f )
   , m_duration( duration )
   , m_looped( looped )
   , m_activeSyncPoint( NULL )
   , m_completedSyncPoint( NULL )
{
   uint count = syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* syncPoint = syncPoints[i];
      CompiledSyncPoint* compiledPoint = new CompiledSyncPoint( syncPoint, syncPoint->getTime() * duration );

      if ( !m_head )
      {
         // this is the first element
         m_head = compiledPoint;
      }

      // plug the element into the list
      compiledPoint->m_prevPoint = m_tail;
      if ( m_tail )
      {
         m_tail->m_nextPoint = compiledPoint;
      }
      m_tail = compiledPoint;

      // verify that the points are sorted
      if ( m_tail->m_prevPoint )
      {
         CONDITIONAL_LOG( m_tail->m_prevPoint->m_endTime > m_tail->m_endTime, "Sync points are not sorted" );
      }

      // calculate event's duration
      CompiledSyncPoint* prevCompiledPoint = compiledPoint->m_prevPoint;
      if ( prevCompiledPoint )
      {
         compiledPoint->m_duration = compiledPoint->m_endTime - prevCompiledPoint->m_endTime;
      }
      else
      {
         compiledPoint->m_duration = compiledPoint->m_endTime;
      }
   }

   if ( !m_head )
   {
      // There are no sync point events defined.
      // Since the code expects there to be a sync event entry at any time, we need to provide one in that case
      m_head = new CompiledSyncPoint( NULL, m_duration );
      m_head->m_duration = m_duration;
      m_tail = m_head;
   }
   else
   {
      if ( m_looped )
      {
         // if the animation is looped, the first event is considered to loop 
         // around the back of the animation - so we need to simulate that by
         // inserting a duplicate event to the end of the events list

         m_head->m_duration = duration + ( m_head->m_endTime - m_tail->m_endTime );

         CompiledSyncPoint* loopedSyncPoint = new CompiledSyncPoint( m_head->m_syncPoint, m_head->m_endTime + duration );
         loopedSyncPoint->m_duration = m_head->m_duration;
         loopedSyncPoint->m_nextPoint = m_head;
         m_head->m_prevPoint = loopedSyncPoint;

         m_tail->m_nextPoint = loopedSyncPoint;
         loopedSyncPoint->m_prevPoint = m_tail;
         m_tail = loopedSyncPoint;
      }
      else if ( m_tail && m_tail->m_endTime < m_duration )
      {
         // The animation is not looped - this means that if there might be a 'sync point'-less gap at the
         // end of it, or
         // Since the code expects there to be a sync event entry at any time, we need to provide one in that case         
         CompiledSyncPoint* fillerSyncPoint = new CompiledSyncPoint( NULL, m_duration );
         fillerSyncPoint->m_duration = m_duration - m_tail->m_endTime;

         m_tail->m_nextPoint = fillerSyncPoint;
         fillerSyncPoint->m_prevPoint = m_tail;
         m_tail = fillerSyncPoint;
      }
   }

   m_activeSyncPoint = m_head;
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimationTrack::~BlendTreeAnimationTrack()
{
   CompiledSyncPoint* removedElem = m_head;

   while( true )
   {
      CompiledSyncPoint* tmp = removedElem;
      removedElem = removedElem->m_nextPoint;

      bool isLast = tmp == m_tail;
      delete tmp;

      if ( isLast )
      {
         break;
      }
   }

   m_head = NULL;
   m_tail = NULL;
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeAnimationTrack::jumpToFirstMatch( const BlendTreeSyncProfile& treeSyncProfile )
{

   for ( CompiledSyncPoint* checkedPoint = m_head; ; checkedPoint = checkedPoint->m_nextPoint )
   {
      // check if any of the tree sync points matches
      for ( uint i = 0; i < treeSyncProfile.m_eventsCount; ++i )
      {
         if ( checkedPoint->m_syncPoint && checkedPoint->m_syncPoint->getBlendTreeEvent() == treeSyncProfile.m_btEvent[i] )
         {
            // found it - it might be a looping one though, so check if the time doesn't exceed the timeline duration
            float timeRemaining = checkedPoint->m_duration * ( 1.0f - treeSyncProfile.m_progress[i] );
            float estimatedTrackTime = checkedPoint->m_endTime - timeRemaining;
            if ( estimatedTrackTime < 0.0f )
            {
               // this is a looped animation, and we're analyzing the part of the looped event that
               // touches the beginning of the track - keep on analyzing
               ASSERT( checkedPoint == m_head && m_looped );
               continue;
            }
            else
            {
               // we found it
               m_activeSyncPoint = checkedPoint;
               m_trackTime = estimatedTrackTime;
               return estimatedTrackTime;
            }
         }
      }

      if ( checkedPoint == m_tail )
      {
         // that was the last element
         break;
      }
   }

   // there was no event that matched - start 
   m_activeSyncPoint = m_head;
   m_trackTime = 0.0f;
   return 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeAnimationTrack::update( float timeElapsed )
{
   float newTrackTime = m_trackTime + timeElapsed;
   bool didAnimationLoopAround = false;
   if ( m_looped )
   {
      float loopedTrackTime = fmod( newTrackTime, m_duration );
      didAnimationLoopAround = newTrackTime != loopedTrackTime;
      newTrackTime = loopedTrackTime;
   }
   else
   {
      newTrackTime = min2< float >( newTrackTime, m_duration );
   }
   m_trackTime = newTrackTime;

   // move to the new sync point
   m_completedSyncPoint = NULL;
   for ( CompiledSyncPoint* checkedPoint = m_activeSyncPoint; checkedPoint != NULL && checkedPoint != m_activeSyncPoint->m_prevPoint; checkedPoint = checkedPoint->m_nextPoint )
   {
      const float startTime = checkedPoint->m_endTime - checkedPoint->m_duration;
      if ( startTime <= m_trackTime && m_trackTime < checkedPoint->m_endTime )
      {
         if ( m_activeSyncPoint->m_syncPoint != checkedPoint->m_syncPoint )
         {
            m_completedSyncPoint = m_activeSyncPoint->m_syncPoint->getBlendTreeEvent();
         }
         m_activeSyncPoint = checkedPoint;
         return didAnimationLoopAround;
      }
   }

   CompiledSyncPoint* checkedPoint = m_activeSyncPoint->m_prevPoint;
   if ( m_looped && checkedPoint )
   {
      // check one more point that we left out because we didn't
      const float startTime = checkedPoint->m_endTime - checkedPoint->m_duration;
      if ( startTime <= m_trackTime && m_trackTime < checkedPoint->m_endTime )
      {
         if ( m_activeSyncPoint->m_syncPoint != checkedPoint->m_syncPoint )
         {
            m_completedSyncPoint = m_activeSyncPoint->m_syncPoint->getBlendTreeEvent();
         }
         m_activeSyncPoint = checkedPoint;
         return didAnimationLoopAround;
      }
   }

   ASSERT_MSG( !m_looped, "The track has gone out of any known sync point range. This is pssible only in case of non-looping animations. Check the code, 'cause it's got a bug somewhere" );
   return didAnimationLoopAround;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimationTrack::updateTreeProfile( BlendTreeSyncProfile& outTreeSyncProfile ) const
{
   if ( m_activeSyncPoint->m_syncPoint != NULL )
   {
      // don't update the profile unless we're actually running a sync point
      float timeRemaining = m_activeSyncPoint->m_endTime - m_trackTime;
      float progress = 1.0f - ( timeRemaining / m_activeSyncPoint->m_duration );
      ASSERT( progress >= 0.0f && progress <= 1.0f );
         
      outTreeSyncProfile.submit( m_activeSyncPoint->m_syncPoint->getBlendTreeEvent(), progress );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimationTrack::synchronizeTo( const BlendTreeSyncProfile& treeSyncProfile, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
{
   // find an entry in the tree profile that corresponds to the current sync point
   for ( CompiledSyncPoint* checkedPoint = m_activeSyncPoint; checkedPoint != NULL && checkedPoint != m_activeSyncPoint->m_prevPoint; checkedPoint = checkedPoint->m_nextPoint )
   {
      for ( uint i = 0; i < treeSyncProfile.m_eventsCount; ++i )
      {
         if ( checkedPoint->m_syncPoint && treeSyncProfile.m_btEvent[i] == checkedPoint->m_syncPoint->getBlendTreeEvent() )
         {
            // got it
            float timeRemaining = checkedPoint->m_endTime - m_trackTime;
            if ( timeRemaining < 0.0f )
            {
               ASSERT( m_looped );
               timeRemaining += m_duration;
            }
            outNodeSyncProfile.setSyncPoint( i, timeRemaining );

            return;
         }
      }
   }

   CompiledSyncPoint* checkedPoint = m_activeSyncPoint->m_prevPoint;
   if ( m_looped && checkedPoint )
   {
      for ( uint i = 0; i < treeSyncProfile.m_eventsCount; ++i )
      {
         if ( checkedPoint->m_syncPoint && treeSyncProfile.m_btEvent[i] == checkedPoint->m_syncPoint->getBlendTreeEvent() )
         {
            // got it
            float timeRemaining = checkedPoint->m_endTime - m_trackTime;
            if ( timeRemaining < 0.0f )
            {
               ASSERT( m_looped );
               timeRemaining += m_duration;
            }

            outNodeSyncProfile.setSyncPoint( i, timeRemaining );

            return;
         }
      }
   }

   // if this line of code was reached, then no applicable sync event was found - don't synchronize the node then
}

///////////////////////////////////////////////////////////////////////////////
