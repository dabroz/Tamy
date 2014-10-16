/// @file   core-AI/SnapshotAnimation.h
/// @brief  animation clip that animates SkeletonEntities
#pragma once

#include "core\Resource.h"
#include "core\Array.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

class BoneSRTAnimation;
struct Vector;
struct Quaternion;

///////////////////////////////////////////////////////////////////////////////

class SnapshotAnimation : public Resource
{
   DECLARE_ALLOCATOR( SnapshotAnimation, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   static const float                     ANIMATION_SAMPLING_FREQUENCY;

   float                                  m_playbackFrequency;
   float                                  m_duration;          // duration of an animation clip ( in seconds )
   uint                                   m_bonesCount;

   Array< Transform >                     m_poseTracks;
   Array< Transform >                     m_motionTrack;

public:
   /**
    * Constructor.
    *
    * @param resourceName
    */
   SnapshotAnimation( const FilePath& resourceName = FilePath() );
   ~SnapshotAnimation();

   // -------------------------------------------------------------------------
   // Data output
   // -------------------------------------------------------------------------

   /**
    * Calculates bone's local matrix for the specified point in time.
    *
    * @param trackTime
    * @param outBoneLocalTransforms
    * @param bonesCount
    */
   void samplePose( float trackTime, Transform* outBoneLocalTransforms, uint bonesCount ) const;

   /**
    * Calculates delta motion for the specified period of time.
    *
    * @param startTime
    * @param endTime
    * @param outMotionTransform
    */
   void sampleMotion( float startTime, float endTime, Transform& outMotionTransform ) const;

   // -------------------------------------------------------------------------
   // Animation definition
   // -------------------------------------------------------------------------
   /**
    * Use this method to create a resource based on the specified animation data.
    *
    * Animation tracks will be sampled at the frequency 24 frames/sec
    *
    * @param animation           animation instance to initialize
    * @param bonesCount          how many entries does the poseKeysArr have - how many bones in the skeleton it animates
    * @param poseKeysArr         an array of transform tracks, each dedicated to a single bone
    * @param motionKeys          motion transforms track
    */
   static void build( SnapshotAnimation& animation, uint bonesCount, BoneSRTAnimation* poseKeysArr, BoneSRTAnimation& motionKeys );

   /**
    * Use this method to create a resource based on a raw animation data that consists of an array of transforms for particular bones, sampled every
    * frame, and an array of motion track transforms, also sampled every frame.
    *
    * @param animation           animation instance to initialize
    * @param playbackSpeed       how many frames should be played per second
    * @param framesCount         how many frames does the animation hold
    * @param bonesCount          how many bones the pose has
    * @param poseKeysArr         an array of transform tracks, the size == framesCount * bonesCount. They are stored by poses - [ [pose_frame_0], [pose_frame_1], ..., [pose_frame_N] ] 
    * @param motionKeys          motion transforms track, the size == framesCount
    */
   static void build( SnapshotAnimation& animation, float playbackSpeed, uint framesCount, uint bonesCount, Transform* poseKeysArr, Transform* motionKeysArr );

   /**
    * Resets the animation contents.
    */
   void clear();

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );

private:
   void sampleMotion( float trackTime, Transform& outMotionTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////
