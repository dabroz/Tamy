/// @file   core-AI/BlendTreeSyncProfile.h
/// @brief  synchronization data hub
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeEvent;
class BlendTreePlayer;
class BlendTreeNode;

///////////////////////////////////////////////////////////////////////////////

#define MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS     16
#define MAX_SIMULTANEOUSLY_SYNCHRONIZED_NODES       32

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure gathers and merges synchronization data from all active nodes
 * during the 'generateSyncInfo' step.
 */
class BlendTreeSyncProfile
{
   DECLARE_ALLOCATOR( BlendTreeSyncProfile, AM_DEFAULT );

public:
   uint                    m_eventsCount;
   BlendTreeEvent*         m_btEvent[MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS];
   float                   m_progress[MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS];

public:
   /**
    * Constructor.
    */
   BlendTreeSyncProfile();

   /**
    * Submits info about a new synchronization point.
    *
    * @param syncPoint
    * @param progress         how much of the sync point duration has already been consumed ( progress = 1 - syncPointDuration ).
    * @return index under which this sync point was stored ( or -1 if we ran out of space )
    */
   int submit( BlendTreeEvent* syncPoint, float progress );

   /**
    * Clears the synchronization settings.
    */
   void reset();

   /**
    * Returns the index of the sync point under which it's registered in the profile.
    *
    * @param syncPoint
    */
   int getSyncPointIdx( BlendTreeEvent* syncPoint ) const;
};

///////////////////////////////////////////////////////////////////////////////

class BlendTreeNodeSyncProfile
{
   DECLARE_ALLOCATOR( BlendTreeNodeSyncProfile, AM_DEFAULT );

public:
   const BlendTreeNode*             m_parentNode;

   float                            m_timeRemaining[MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS];

   uint                             m_nodesCount;
   const BlendTreeNode*             m_contributingNodePtrs[MAX_SIMULTANEOUSLY_SYNCHRONIZED_NODES];
   float                            m_playbackSpeed[MAX_SIMULTANEOUSLY_SYNCHRONIZED_NODES];

private:
   char                             m_syncPointIdx[MAX_SIMULTANEOUSLY_SYNCHRONIZED_NODES];
   float                            m_totalBlendWeight[MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS];
   const BlendTreeNodeSyncProfile*  m_nodeSyncData[MAX_SIMULTANEOUSLY_SYNCHRONIZED_NODES];

public:
   /**
    * Constructor.
    *
    * @param parentNode
    */
   BlendTreeNodeSyncProfile( const BlendTreeNode* parentNode );

   /**
    * Resets the contents of this sync structure.
    */
   void reset();

   /**
    * Sets the value for the specified sync point, with blend weight == 1.0f.
    *
    * @param idx
    * @param timeRemaining       in seconds
    */
   void setSyncPoint( int idx, float timeRemaining );

   /**
    * Merges the contents of two sync structure.
    *
    * @param rhs
    * @param blendWeight
    */
   void mergeWith( const BlendTreeNodeSyncProfile* rhs, float blendWeight );

   /**
    * Calculates merged synchronization times and playback velocities.
    */
   void commit();

   /**
    * Applies velocity changes to contributing nodes.
    *
    * @param player
    */
   void applyVelocityChanges( BlendTreePlayer* player );
};

///////////////////////////////////////////////////////////////////////////////
