#include "core-AI\SnapshotAnimation.h"
#include "core-AI\BoneSRTAnimation.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

const float SnapshotAnimation::ANIMATION_SAMPLING_FREQUENCY = 1.0f / 24.0f;

///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( SnapshotAnimation, tsa, AM_BINARY );
   PROPERTY( float, m_playbackFrequency );
   PROPERTY( float, m_duration );
   PROPERTY( uint, m_bonesCount );
   PROPERTY( Array< Transform >, m_poseTracks );
   PROPERTY( Array< Transform >, m_motionTrack );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

SnapshotAnimation::SnapshotAnimation( const FilePath& resourceName )
   : Resource( resourceName )
   , m_playbackFrequency( ANIMATION_SAMPLING_FREQUENCY )
   , m_duration( 0.0f )
   , m_bonesCount( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

SnapshotAnimation::~SnapshotAnimation()
{
   clear();
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::replaceContents( Resource& rhs )
{
   SnapshotAnimation& rhsSnapshotAnimation = static_cast< SnapshotAnimation& >( rhs );

   m_playbackFrequency = rhsSnapshotAnimation.m_playbackFrequency;
   m_duration = rhsSnapshotAnimation.m_duration;
   m_bonesCount = rhsSnapshotAnimation.m_bonesCount;
   m_poseTracks = rhsSnapshotAnimation.m_poseTracks;
   m_motionTrack = rhsSnapshotAnimation.m_motionTrack;
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::samplePose( float trackTime, Transform* outBoneLocalTransforms, uint bonesCount ) const
{
   if ( m_duration <= 0.0f )
   {
      return;
   }

   float clampedTrackTime = clamp( trackTime, 0.0f, m_duration );

   uint lastFrameIdx = m_motionTrack.size() - 1;
   uint startFrameIdx = min2( (uint)ceilf( clampedTrackTime / m_playbackFrequency ), lastFrameIdx );
   uint endFrameIdx = min2( startFrameIdx + 1, lastFrameIdx );

   float timeIntoFrame = clampedTrackTime - (float)startFrameIdx * m_playbackFrequency;

   FastFloat lerpDist;
   lerpDist.setFromFloat( timeIntoFrame / m_playbackFrequency );

   uint updatedBonesCount = min2( bonesCount, m_bonesCount );
   uint startTransformIdx = startFrameIdx * m_bonesCount;
   uint endTransformIdx = endFrameIdx * m_bonesCount;
   for ( uint boneIdx = 0; boneIdx < updatedBonesCount; ++boneIdx, ++startTransformIdx, ++endTransformIdx )
   {
      Transform& boneLocalTransform = outBoneLocalTransforms[boneIdx];

      const Transform& startTransform = m_poseTracks[startTransformIdx];
      const Transform& endTransform = m_poseTracks[endTransformIdx];

      boneLocalTransform.setNlerp( startTransform, endTransform, lerpDist );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::sampleMotion( float startTime, float endTime, Transform& outMotionTransform ) const
{
   float clampedStartTime = clamp( startTime, 0.0f, m_duration );
   float clampedEndTime = clamp( endTime, 0.0f, m_duration );

   if ( m_duration <= 0.0f || clampedStartTime == clampedEndTime )
   {
      outMotionTransform = Transform::IDENTITY;
      return;
   }

   if ( clampedStartTime < clampedEndTime )
   {
      // there's no loop
      Transform t1, t2;
      sampleMotion( clampedStartTime, t1 );
      sampleMotion( clampedEndTime, t2 );
      outMotionTransform.setMulInverse( t2, t1 );
   }
   else // endTime < startTime
   {
      // there is a loop
      Transform t1, t2, t3, t4;
      sampleMotion( clampedStartTime, t1 );
      sampleMotion( m_duration, t2 );
      sampleMotion( clampedEndTime, t3 );

      t4.setMulInverse( t2, t1 );
      outMotionTransform.setMul( t3, t4 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::sampleMotion( float trackTime, Transform& outMotionTransform ) const
{
   uint lastFrameIdx = m_motionTrack.size() - 1;
   uint startFrameIdx = min2( ( uint ) ceilf( trackTime / m_playbackFrequency ), lastFrameIdx );
   uint endFrameIdx = min2( startFrameIdx + 1, lastFrameIdx );

   float timeIntoFrame = trackTime - ( float ) startFrameIdx * m_playbackFrequency;

   FastFloat lerpDist;
   lerpDist.setFromFloat( timeIntoFrame / m_playbackFrequency );

   const Transform& startTransform = m_motionTrack[startFrameIdx];
   const Transform& endTransform = m_motionTrack[endFrameIdx];

   outMotionTransform.setNlerp( startTransform, endTransform, lerpDist );
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::build( SnapshotAnimation& animation, uint bonesCount, BoneSRTAnimation* poseKeysArr, BoneSRTAnimation& motionKeys )
{
   animation.m_bonesCount = bonesCount;
   animation.m_playbackFrequency = ANIMATION_SAMPLING_FREQUENCY;

   // first - determine the duration of the animation
   animation.m_duration = max2( 0.0f, motionKeys.m_duration );
   for ( uint boneIdx = 0; boneIdx < bonesCount; ++boneIdx )
   {
      animation.m_duration = max2( animation.m_duration, poseKeysArr[boneIdx].m_duration );
   }

   uint framesCount = (uint)ceilf( animation.m_duration / animation.m_playbackFrequency ) + 1;

   // allocate enough room in the arrays
   animation.m_motionTrack.resize( framesCount );

   uint poseEntriesCount = bonesCount * framesCount;
   animation.m_poseTracks.resize( poseEntriesCount );

   // sample poses and the motion track
   uint poseIdx = 0;
   float sampledTrackTime = 0.0f;
   for ( uint frameIdx = 0; frameIdx < framesCount; ++frameIdx, sampledTrackTime += animation.m_playbackFrequency )
   {
      for ( uint boneIdx = 0; boneIdx < bonesCount; ++boneIdx )
      {
         Transform& poseTransform = animation.m_poseTracks[poseIdx];
         ++poseIdx;

         BoneSRTAnimation& poseTrack = poseKeysArr[boneIdx];

         poseTrack.sample( sampledTrackTime, poseTransform );
      }

      Transform& motionTransform = animation.m_motionTrack[frameIdx];
      motionKeys.sample( sampledTrackTime, motionTransform );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::build( SnapshotAnimation& animation, float playbackSpeed, uint framesCount, uint bonesCount, Transform* poseKeysArr, Transform* motionKeysArr )
{
   animation.m_bonesCount = bonesCount;
   animation.m_duration = (float)framesCount / playbackSpeed;
   animation.m_playbackFrequency = 1.0f / playbackSpeed;

   // allocate enough room in the arrays
   animation.m_motionTrack.resize( framesCount );

   uint poseEntriesCount = bonesCount * framesCount;
   animation.m_poseTracks.resize( poseEntriesCount );

   if ( poseKeysArr )
   {
      uint poseIdx = 0;
      for ( uint poseIdx = 0; poseIdx < poseEntriesCount; ++poseIdx )
      {
         animation.m_poseTracks[poseIdx] = poseKeysArr[poseIdx];
      }
   }

   if ( motionKeysArr )
   {
      for ( uint frameIdx = 0; frameIdx < framesCount; ++frameIdx )
      {
         animation.m_motionTrack[frameIdx] = motionKeysArr[frameIdx];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SnapshotAnimation::clear()
{
   m_duration = 0.0f;
   m_bonesCount = 0;
   m_poseTracks.clear();
   m_motionTrack.clear();
}

///////////////////////////////////////////////////////////////////////////////
