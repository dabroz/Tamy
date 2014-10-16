/// @file   core-AI/SyncPointsList.h
/// @brief  a runtime list of sync points management
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\types.h"


//////////////////////////////////////////////////////////////////////////////

class BlendTreeSyncPoint;
class BlendTreeEvent;
class BlendTreeSyncProfile;
class BlendTreeNodeSyncProfile;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tool that organizes events in a list that can be used at runtime to quickly
 * look up which event comes next, or when a desired event will take place.
 */
class BlendTreeAnimationTrack
{
   DECLARE_ALLOCATOR( BlendTreeAnimationTrack, AM_DEFAULT );

private:
   struct CompiledSyncPoint
   {
      DECLARE_ALLOCATOR( CompiledSyncPoint, AM_DEFAULT );

      BlendTreeSyncPoint*     m_syncPoint;
      float                   m_endTime;           // specified in animation time
      float                   m_duration;          // specified in animation time

      CompiledSyncPoint*      m_prevPoint;
      CompiledSyncPoint*      m_nextPoint;

      /**
       * Constructor.
       *
       * @param syncPoint     synchronization point
       * @param entTime       when does the sync point end ( in animation time )
       */
      CompiledSyncPoint( BlendTreeSyncPoint* syncPoint, float endTime )
         : m_syncPoint( syncPoint )
         , m_endTime( endTime )
         , m_duration( 0.0f )
         , m_prevPoint( NULL )
         , m_nextPoint( NULL )
      {}
   };

public:
   float                      m_trackTime;
   BlendTreeEvent*            m_completedSyncPoint;

private:
   CompiledSyncPoint*         m_head;
   CompiledSyncPoint*         m_tail;
   
   float                      m_duration;
   bool                       m_looped;

   CompiledSyncPoint*         m_activeSyncPoint;

public:
   /**
    * Constructor.
    * Compiles a list of sync points into its runtime form.
    *
    * @param syncPoints    - we assume that they are sorted by time
    * @param duration
    * @param looped
    */
   BlendTreeAnimationTrack( const Array< BlendTreeSyncPoint* >& syncPoints, float duration, bool looped );
   ~BlendTreeAnimationTrack();

   /**
    * Finds the first matching point ( starting from the beginning of the animation ) and jumps there, 
    * returning the track time value.
    *
    * @param treeSyncProfile
    * @param new track time
    */
   float jumpToFirstMatch( const BlendTreeSyncProfile& treeSyncProfile );

   /**
    * Updates the tracker.
    *
    * @param timeElapsed
    * @return 'true' if the animation track looped around
    */
   bool update( float timeElapsed );

   /**
    * Updates the tree profile with the current sync point details.
    *
    * @param outTreeSyncProf
    */
   void updateTreeProfile( BlendTreeSyncProfile& outTreeSyncProfile ) const;

   /**
    * Takes the tree profile, and without changing the track position, generates a synchronization
    * profile for the node that will be used to determine the playback speed the node should play
    * with to stay in sync with the tree.
    *
    * @param treeSyncProfile
    * @param outNodeSyncProf
    */
   void synchronizeTo( const BlendTreeSyncProfile& treeSyncProfile, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const;
};

///////////////////////////////////////////////////////////////////////////////
