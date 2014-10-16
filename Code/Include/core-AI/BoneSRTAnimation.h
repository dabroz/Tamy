/// @file   core-AI\BoneSRTAnimation.h
/// @brief  animation data stream for a single bone
#pragma once

#include "core-AI\AnimationTimeline.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

class BoneSRTAnimationPlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Animation data stream for a single bone.
 */
class BoneSRTAnimation
{
   DECLARE_ALLOCATOR( BoneSRTAnimation, AM_DEFAULT );

private:
   struct VecLerp
   {
      DECLARE_ALLOCATOR( VecLerp, AM_DEFAULT );

   public:
      void operator()( Vector& outVec, const Vector& start, const Vector& end, const FastFloat& percentage ) const
      {
         outVec.setLerp( start, end, percentage );
      }
   };

   struct QuatLerp
   {
      DECLARE_ALLOCATOR( QuatLerp, AM_DEFAULT );

   public:
      void operator()( Quaternion& outQuat, const Quaternion& start, const Quaternion& end, const FastFloat& percentage ) const
      {
         outQuat.setSlerp( start, end, percentage );
      }
   };

public:
   float                                              m_duration;

private:
   AnimationTimeline< Quaternion, QuatLerp >          m_orientation;
   AnimationTimeline< Vector, VecLerp >               m_translation;

   uint                                               m_orientationKeyIdx;
   uint                                               m_translationKeyIdx;

public:
   /**
    * Constructor.
    */
   BoneSRTAnimation();

   // -------------------------------------------------------------------------
   // Track sampling
   // -------------------------------------------------------------------------
   /**
    * Samples a transform at the specified point of track.
    *
    * @param trackTime
    * @param outTransform
    */
   void sample( float trackTime, Transform& outTransform );

   // -------------------------------------------------------------------------
   // Track definition
   // -------------------------------------------------------------------------
   /**
    * Adds an animation orientation key definition at the specified time.
    *
    * @param frameTime
    * @param orientation
    */
   void addOrientationKey( float time, const Quaternion& orientation );

   /**
    * Adds an animation translation key definition at the specified time.
    *
    * @param frameTime
    * @param translation
    */
   void addTranslationKey( float time, const Vector& translation );

private:
   /**
    * Returns the translation value at the specified time.
    *
    * @param lastCheckedKeyIdx
    * @param time
    * @param outTranslation
    *
    * @return  'true' if a value was found, 'false' otherwise
    */
   bool getTranslation( unsigned int& lastCheckedKeyIdx, float time, Vector& outTranslation ) const;

   /**
    * Returns the orientation value at the specified time.
    *
    * @param lastCheckedKeyIdx
    * @param time
    * @param outOrientation
    *
    * @return  'true' if a value was found, 'false' otherwise
    */
   bool getOrientation( unsigned int& lastCheckedKeyIdx, float time, Quaternion& outOrientation ) const;

private:
   void updateDuration();
};

///////////////////////////////////////////////////////////////////////////////
