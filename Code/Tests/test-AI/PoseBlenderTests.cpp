#include "core-TestFramework\TestFramework.h"
#include "core-AI\PoseBlendingUtils.h"


///////////////////////////////////////////////////////////////////////////////

TEST( PoseBlendingUtils, blendingTwoPoses )
{
   // setup data
   const uint bonesCount = 2;
   Transform sourcePose[bonesCount];
   Transform targetPose[bonesCount];
   Transform blendedPose[bonesCount];

   sourcePose[0].m_translation.set( 1, 1, 1 );
   sourcePose[1].m_translation.set( 2, 2, 2 );

   targetPose[0].m_translation.set( 3, 3, 3 );
   targetPose[1].m_translation.set( 4, 4, 4 );

   // test the blend
   PoseBlendingUtils::blend( Float_0, sourcePose, targetPose, bonesCount, blendedPose );
   CPPUNIT_ASSERT( sourcePose[0] == blendedPose[0] );
   CPPUNIT_ASSERT( sourcePose[1] == blendedPose[1] );

   PoseBlendingUtils::blend( Float_1, sourcePose, targetPose, bonesCount, blendedPose );
   CPPUNIT_ASSERT( targetPose[0] == blendedPose[0] );
   CPPUNIT_ASSERT( targetPose[1] == blendedPose[1] );

   PoseBlendingUtils::blend( FastFloat::fromFloat( -0.1f ), sourcePose, targetPose, bonesCount, blendedPose );
   CPPUNIT_ASSERT( sourcePose[0] == blendedPose[0] );
   CPPUNIT_ASSERT( sourcePose[1] == blendedPose[1] );

   PoseBlendingUtils::blend( FastFloat::fromFloat( 1.1f ), sourcePose, targetPose, bonesCount, blendedPose );
   CPPUNIT_ASSERT( targetPose[0] == blendedPose[0] );
   CPPUNIT_ASSERT( targetPose[1] == blendedPose[1] );

   PoseBlendingUtils::blend( FastFloat::fromFloat( 0.5f ), sourcePose, targetPose, bonesCount, blendedPose );
   {
      Transform expectedPose[bonesCount];
      expectedPose[0].m_translation.set( 2, 2, 2 );
      expectedPose[1].m_translation.set( 3, 3, 3 );

      CPPUNIT_ASSERT( expectedPose[0] == blendedPose[0] );
      CPPUNIT_ASSERT( expectedPose[1] == blendedPose[1] );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( PoseBlendingUtils, synchronizingTwoAnimations )
{
   const int animationsCount = 2;
   TimeBasedSynchroData syncData[animationsCount];
   syncData[0].m_timeLeftUntilEvent = 1.0f;
   syncData[1].m_timeLeftUntilEvent = 2.0f;
   
   syncData[0].m_blendWeight = 1.0f;
   syncData[1].m_blendWeight = 0.0f;
   PoseBlendingUtils::synchronize( syncData, animationsCount );
   CPPUNIT_ASSERT_EQUAL( 1.0f, syncData[0].m_playbackSpeed );
   CPPUNIT_ASSERT_EQUAL( 2.0f, syncData[1].m_playbackSpeed );

   syncData[0].m_blendWeight = 0.0f;
   syncData[1].m_blendWeight = 1.0f;
   PoseBlendingUtils::synchronize( syncData, animationsCount );
   CPPUNIT_ASSERT_EQUAL( 0.5f, syncData[0].m_playbackSpeed );
   CPPUNIT_ASSERT_EQUAL( 1.0f, syncData[1].m_playbackSpeed );

   syncData[0].m_blendWeight = 0.5f;
   syncData[1].m_blendWeight = 0.5f;
   PoseBlendingUtils::synchronize( syncData, animationsCount );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6f, syncData[0].m_playbackSpeed , 0.1f );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.3f, syncData[1].m_playbackSpeed, 0.1f );
}

///////////////////////////////////////////////////////////////////////////////
