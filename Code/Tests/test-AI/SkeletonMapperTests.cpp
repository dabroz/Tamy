#include "core-TestFramework\TestFramework.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonPoseTool.h"


///////////////////////////////////////////////////////////////////////////////

#define TEST_BONE( pose, boneName, rotationAxis, angle, translation )  { \
      bool result = pose.testBoneModel( boneName, rotationAxis, angle, translation ); \
      CPPUNIT_NS::Asserter::failIf( !result, CPPUNIT_NS::Message( pose.getErrorMsg() ), CPPUNIT_SOURCELINE() ); \
   }

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, sameBindPose )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );
   
   poseA.start().rotateAndTranslate( "boneA", Vector_OX, DEG2RAD( 45.0f ), Vector( 1.0f, 2.0f, -3.0f ) ).end();
   mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 45.0f ), Vector( 1.0f, 2.0f, -3.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, differentBindRotation )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // Let's make the bone in skeleton A turn 45 degrees about OX axis.
   poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( 45.0f ) ).end();
   mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -45.0f ), Vector_ZERO );
   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -45.0f ), Vector_ZERO );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, differentBindOffset )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 5.0f, 0.0f, 0.0f ) ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // Let's make the bone in skeleton A move by (-5,0,0)
   // The mapped bone should move by the same amount and and up at pos (-5,0,0)
   poseA.start().translate( "boneA", Vector( -5.0f, 0.0f, 0.0f ) ).end();
   mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, differentBindRotation_poseTranslation )
{
   Skeleton skeletonA, skeletonB;
   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO ).buildSkeleton();
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO ).buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // Let's make the bone in skeleton A move by (-5,0,0), and rotate the bone 90 deg OX.
   // This should yield similar rotation in the mapped bone
   poseA.start().rotateAndTranslate( "boneA", Vector_OX, DEG2RAD( 90.0f ), Vector( -5.0f, 0.0f, 0.0f ) ).end();
   mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

   TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( -5.0f, 0.0f, 0.0f ) );
   TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( -5.0f, 0.0f, 0.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, boneChain )
{
   // all bones in skeleton A 
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) )
      .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
      .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // case 1
   {
      // rotate the child bone 90 deg OX
      poseA.start().rotate( "boneB", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 1.0f, 0.0f ) ); // the position of the bone doesn't change, since it's relative to the parent bone, which hasn't changed its orientation


      // pose B check
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
   }

   // case 2
   {
      // rotate the parent bone 90 deg OX
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );


      // pose B check
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, oneToManyMapping )
{
   // all bones in skeleton A 
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO )
         .bone( "boneC", "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) )
      .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .bone( "boneC", "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
      .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // case 1
   {
      // rotate the child bone 90 deg OX
      poseA.start().rotate( "boneC", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 2.0f, 0.0f ) ); // the child bone rotates, but doesn't change its position

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 2.0f, 0.0f ) ); // boneC has rotated 90 deg OX
   }

   // case 2
   {
      // rotate the parent bone 90 deg OX
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) );

      // pose B check:
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, manyToOneMapping )
{
   // all bones in skeleton A 
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .bone( "boneC", "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneC", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // case 1
   {
      // rotate the child bone 90 deg OX
      poseA.start().rotate( "boneC", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 0.0f, 2.0f ) ); // the child bone rotates, but doesn't change its position

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 0.0f, 2.0f ) ); // boneC has rotated 90 deg OX
   }

   // case 2
   {
      // rotate the parent bone 90 deg OX
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check:
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, manyToManyMapping )
{
   // all bones in skeleton A 
   Skeleton skeletonA;
   SkeletonPoseTool poseA( skeletonA );
   {
      poseA.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneB", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .bone( "boneC", "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .buildSkeleton();
   }

   Skeleton skeletonB;
   SkeletonPoseTool poseB( skeletonB );
   {
      poseB.startSkeleton( "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO )
         .bone( "boneD", "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) )
         .bone( "boneC", "boneD", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 2.0f ) )
         .buildSkeleton();
   }

   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   // case 1
   {
      // rotate the child bone 90 deg OX
      poseA.start().rotate( "boneC", Vector_OX, DEG2RAD( 90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 0.0f, 2.0f ) ); // the child bone rotates, but doesn't change its position

      // pose B check - boneA and bone B hasn't moved, and mapping should reflect that
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( 0.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneD", Vector_OX, DEG2RAD( 0.0f ), Vector( 0.0f, 0.0f, 1.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 0.0f, 2.0f ) ); // boneC has rotated 90 deg OX
   }

   // case 2
   {
      // rotate the parent bone 90 deg OX
      poseA.start().rotate( "boneA", Vector_OX, DEG2RAD( -90.0f ) ).end();
      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // pose A check
      TEST_BONE( poseA, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseA, "boneB", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseA, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );

      // pose B check:
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );
      TEST_BONE( poseB, "boneD", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseB, "boneC", Vector_OX, DEG2RAD( -90.0f ), Vector( 0.0f, 2.0f, 0.0f ) );
   }
}


///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonMapper, differentBoneOrder )
{
   // the order of bones: boneA -> boneB
   Skeleton skeletonA;
   {
      Transform boneTransform;
      boneTransform.set( Quaternion::IDENTITY, Vector_ZERO );
      skeletonA.addBone( "boneA", boneTransform, -1, 1.0f );

      boneTransform.set( Quaternion::IDENTITY, Vector( 0.0f, 1.0f, 0.0f ) );
      skeletonA.addBone( "boneB", boneTransform, 0, 1.0f );

      skeletonA.buildSkeleton();
   }

   // the order of bones: boneB -> boneA
   Skeleton skeletonB;
   {
      Transform boneTransform;
      boneTransform.set( Vector_OX, DEG2RAD( 180.0f ), Vector( 0.0f, 1.0f, 0.0f ) ); // the bone is rotated, pointing down in model space towards where boneA of skeletonA would be located
      skeletonB.addBone( "boneB", boneTransform, -1, 1.0f );

      boneTransform.set( Quaternion::IDENTITY, Vector( 0.0f, 1.0f, 0.0f ) );
      skeletonB.addBone( "boneA", boneTransform, 0, 1.0f );

      skeletonB.buildSkeleton();
   }


   SkeletonMapper mapper;
   mapper.defineMapping( &skeletonA, &skeletonB, Lookup_ByName );

   SkeletonPoseTool poseA( skeletonA );
   SkeletonPoseTool poseB( skeletonB );

   // case 1
   {
      // skeletonA: rotate boneB bone 90 deg OX, leaving boneA unchanged
      poseA.start()
         .rotate( "boneB", Vector_OX, DEG2RAD( 90.0f ) )
      .end();


      mapper.calcPoseLocalSpace( poseA.getLocal(), poseB.accessLocal() );

      // skeletonB: in this case the skeleton is connected the other way around, and, in addition, the root
      // bone is oriented in the opposite direction.
      // So what we should see is that:
      //   - skeletonB:boneB should rotate 90 deg OX in model space in response to skeletonA:boneB's rotation
      //   - skeletonB:boneA should rotate -90 deg OX in order to preserve its model space orientation, just because skeletonA:boneA bone hasn't change its
      TEST_BONE( poseB, "boneB", Vector_OX, DEG2RAD( 90.0f ), Vector( 0.0f, 1.0f, 0.0f ) );
      TEST_BONE( poseB, "boneA", Vector_OX, DEG2RAD( -90.0f ), Vector_ZERO );

   }
}

///////////////////////////////////////////////////////////////////////////////
