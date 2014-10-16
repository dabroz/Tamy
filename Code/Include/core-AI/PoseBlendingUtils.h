/// @file   core-AI/PoseBlendingUtils.h
/// @brief  a set of utilities used to blend animation clips together
#pragma once

#include "core\Transform.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

struct TimeBasedSynchroData
{
   DECLARE_ALLOCATOR( TimeBasedSynchroData, AM_DEFAULT );

   // input
   float             m_timeLeftUntilEvent;
   float             m_blendWeight;

   // output
   float             m_playbackSpeed;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A set of utilities used to blend animation clips together.
 */
class PoseBlendingUtils
{
public:
   /**
    * Blends to poses together.
    *
    * @param blendWeight      a value in range ( 0 .. 1 )
    * @param sourcePose
    * @param targetPose
    * @param bonesCount       number of bones in each pose - they have to be the same
    * @param outBlendedPose   resulting pose will be stored here
    */
   static void blend( const FastFloat& blendWeight, const Transform* sourcePose, const Transform* targetPose, uint bonesCount, Transform* outBlendedPose );

   /**
    * Calculates a synchronization solution that will allow to line up clips so that they hit the specified
    * event at the same time.
    */
   static void synchronize( TimeBasedSynchroData* arrSyncInfos, uint infosCount );
};

///////////////////////////////////////////////////////////////////////////////

