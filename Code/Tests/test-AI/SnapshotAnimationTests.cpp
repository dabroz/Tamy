#include "core-TestFramework\TestFramework.h"
#include "core-AI\BoneSRTAnimation.h"
#include "core-AI\SnapshotAnimation.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TEST( SnapshotAnimation, build )
{
   // build a test animation
   const uint bonesCount = 2;
   BoneSRTAnimation* keyframes = new BoneSRTAnimation[bonesCount];
   BoneSRTAnimation motion;
   {
      // frame 0
      keyframes[0].addTranslationKey( 0.0f, Vector_ZERO );
      keyframes[1].addTranslationKey( 0.0f, Vector_ZERO );
      motion.addTranslationKey( 0.0f, Vector_ZERO );
      
      keyframes[0].addTranslationKey( 0.75f, Vector( 3.75f, 0, 0 ) );
      keyframes[1].addTranslationKey( 1.0f, Vector( 0, 5, 0 ) );
      motion.addTranslationKey( 0.8f, Vector( 0, 0, 4 ) );
   }


   SnapshotAnimation animation;
   SnapshotAnimation::build( animation, bonesCount, keyframes, motion );
   {
      CPPUNIT_ASSERT_EQUAL( 1.0f, animation.m_duration );
      CPPUNIT_ASSERT_EQUAL( bonesCount, animation.m_bonesCount );
      CPPUNIT_ASSERT_EQUAL( (uint)50, animation.m_poseTracks.size() ); // the animation is 1s long and we were sampling at 24 frames/sec, so we have 25 key frames per bone, which gives 50 frames for a skeleton that consists of 2 bones
      CPPUNIT_ASSERT_EQUAL( (uint)25, animation.m_motionTrack.size() ); // and 25 keyframes in the motion track
   }

   // test the animation
   {
      Transform boneTransforms[2];
      Transform motionTransform;

      {
         animation.samplePose( 0.0f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 0.0f, motionTransform );

         COMPARE_VEC( Vector_ZERO, boneTransforms[0].m_translation );
         COMPARE_VEC( Vector_ZERO, boneTransforms[1].m_translation );
         COMPARE_VEC( Vector_ZERO, motionTransform.m_translation );
      }

      {
         animation.samplePose( 0.25f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 0.25f, motionTransform );

         COMPARE_VEC( Vector( 1.25f, 0, 0 ), boneTransforms[0].m_translation );
         COMPARE_VEC( Vector( 0, 1.25f, 0 ), boneTransforms[1].m_translation );
         COMPARE_VEC( Vector( 0, 0, 1.25f ), motionTransform.m_translation );
      }

      {
         animation.samplePose( 0.5f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 0.5f, motionTransform );

         COMPARE_VEC( Vector( 2.5f, 0, 0 ), boneTransforms[0].m_translation );
         COMPARE_VEC( Vector( 0, 2.5f, 0 ), boneTransforms[1].m_translation );
         COMPARE_VEC( Vector( 0, 0, 2.5f ), motionTransform.m_translation );
      }

      {
         animation.samplePose( 0.75f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 0.75f, motionTransform );

         COMPARE_VEC( Vector( 3.75f, 0, 0 ), boneTransforms[0].m_translation );
         COMPARE_VEC( Vector( 0, 3.75f, 0 ), boneTransforms[1].m_translation );
         COMPARE_VEC( Vector( 0, 0, 3.75f ), motionTransform.m_translation );
      }

      {
         animation.samplePose( 1.0f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 1.0f, motionTransform );

         COMPARE_VEC( Vector( 3.75f, 0, 0 ), boneTransforms[0].m_translation );
         COMPARE_VEC( Vector( 0, 5, 0 ), boneTransforms[1].m_translation );
         COMPARE_VEC( Vector( 0, 0, 4 ), motionTransform.m_translation );
      }

      // test outside of the boundaries
      {
         animation.samplePose( -0.1f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, -0.1f, motionTransform );

         COMPARE_VEC( Vector_ZERO, boneTransforms[0].m_translation );
         COMPARE_VEC( Vector_ZERO, boneTransforms[1].m_translation );
         COMPARE_VEC( Vector_ZERO, motionTransform.m_translation );
      }

      {
         animation.samplePose( 1.1f, boneTransforms, 2 );
         animation.sampleMotion( 0.0f, 1.1f, motionTransform );

         COMPARE_VEC( Vector( 3.75f, 0, 0 ), boneTransforms[0].m_translation );
         COMPARE_VEC( Vector( 0, 5, 0 ), boneTransforms[1].m_translation );
         COMPARE_VEC( Vector( 0, 0, 4 ), motionTransform.m_translation );
      }
   }

   // cleanup
   delete [] keyframes;
}

///////////////////////////////////////////////////////////////////////////////
