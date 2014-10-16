#include "core-AI\PoseBlendingUtils.h"
#include "core\Algorithms.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void PoseBlendingUtils::blend( const FastFloat& blendWeight, const Transform* sourcePose, const Transform* targetPose, uint bonesCount, Transform* outBlendedPose )
{
   FastFloat clampedBlendWeight;
   clampedBlendWeight.setClamped( blendWeight, Float_0, Float_1 );

   for ( uint i = 0; i < bonesCount; ++i )
   {
      const Transform& sourceTransform = sourcePose[i];
      const Transform& targetTransform = targetPose[i];
      Transform& blendedTransform = outBlendedPose[i];
      blendedTransform.setSlerp( sourceTransform, targetTransform, clampedBlendWeight );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PoseBlendingUtils::synchronize( TimeBasedSynchroData* arrSyncInfos, uint infosCount )
{
   // calculate the length of the virtual clip
   float virtualClipLength = 0;
   for ( uint i = 0; i < infosCount; ++i )
   {
      TimeBasedSynchroData& info = arrSyncInfos[i];
      virtualClipLength += info.m_timeLeftUntilEvent * info.m_blendWeight; 
   }
   
   if ( virtualClipLength == 0.0f )
   {
      // the solution doesn't exist
      return;
   }

   // use that length to calculate playback speeds of individual clips
   // so that their respective lengths would match it
   for ( uint i = 0; i < infosCount; ++i )
   {
      TimeBasedSynchroData& info = arrSyncInfos[i];
      info.m_playbackSpeed = info.m_timeLeftUntilEvent / virtualClipLength;
   }
}

///////////////////////////////////////////////////////////////////////////////
