#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class BlendTreeMockNode : public BlendTreeNode
   {
      DECLARE_ALLOCATOR( BlendTreeMockNode, AM_ALIGNED_16 );

   private:
      Matrix*        m_targetPose;
      uint           m_bonesCount;

   public:
      BlendTreeMockNode( Matrix* targetPose, uint bonesCount )
      {
         m_bonesCount = bonesCount;
         m_targetPose = new Matrix[bonesCount];
         for ( uint i = 0; i < bonesCount; ++i )
         {
            m_targetPose[i] = targetPose[i];
         }
      }

      ~BlendTreeMockNode()
      {
         delete [] m_targetPose;
         m_targetPose = NULL;

         m_bonesCount = 0;
      }

      void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const override
      {
         for ( uint i = 0; i < m_bonesCount; ++i )
         {
            outGeneratedPose[i].set( m_targetPose[i] );
         }
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreePlayer, samplingAPose )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;
   skeleton.addBone( "Root", Matrix::IDENTITY, -1, 1.0f );
   skeleton.addBone( "Hand", Matrix::IDENTITY, 0, 1.0f );

   // define a blend tree
   BlendTree tree;
   Matrix testPose[2];
   {
      tree.setSkeleton( &skeleton );

      testPose[0].setTranslation( Vector( 1.0f, 1.0f, 1.0f ) );
      testPose[1].setTranslation( Vector( 2.0f, 2.0f, 2.0f ) );
      tree.getRoot().add( new BlendTreeMockNode( testPose, 2 ) );
   }

   // create an animated entity
   Entity* entity = new Entity();
   SkeletonComponent* skeletonComponent = NULL;
   {
      skeletonComponent = new SkeletonComponent();
      skeletonComponent->setSkeleton( &skeleton );
      entity->addChild( skeletonComponent );

      BlendTreePlayer* player = new BlendTreePlayer();
      player->setBlendTree( tree );
      entity->addChild( player );
   }

   Model scene;
   scene.addChild( entity );

   // attach an animation world
   AnimationWorld animWorld;
   animWorld.play( true );
   scene.attachListener( &animWorld );

   // test if the poses are being assembled
   {
      animWorld.tickAnimations( 1.0f );

      // test the pose
      COMPARE_MTX( skeletonComponent->m_boneLocalMtx[0], testPose[0] );
      COMPARE_MTX( skeletonComponent->m_boneLocalMtx[1], testPose[1] );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreePlayer, triggeringEvents)
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;
   skeleton.addBone( "Root", Matrix::IDENTITY, -1, 1.0f );
   skeleton.addBone( "Hand", Matrix::IDENTITY, 0, 1.0f );

   // define a blend tree
   BlendTree tree;
   Matrix testPose[2];
   BlendTreeEvent* testEvent = NULL;
   {
      tree.setSkeleton( &skeleton );

      // define an event
      testEvent = new BlendTreeEvent();
      tree.addEvent( testEvent );
   }

   // create an animated entity
   Entity* entity = new Entity();
   SkeletonComponent* skeletonComponent = NULL;
   BlendTreePlayer* player = NULL;
   {
      skeletonComponent = new SkeletonComponent();
      skeletonComponent->setSkeleton( &skeleton );
      entity->addChild( skeletonComponent );

      player = new BlendTreePlayer();
      player->setBlendTree( tree );
      entity->addChild( player );
   }

   Model scene;
   scene.addChild( entity );

   // attach an animation world
   AnimationWorld animWorld;
   animWorld.play( true );
   scene.attachListener( &animWorld );

   {
      // check that the event isn't triggered by default
      bool wasTriggered = player->wasEventTriggered( testEvent );
      CPPUNIT_ASSERT( !wasTriggered );

      // Events that are triggered in frame 0 will be 'visible' in frame 1.
      // So when we advance to the next frame now, having not triggered any events,
      // we shouldn't see any of them triggered here as well
      animWorld.tickAnimations( 1.0f );
      wasTriggered = player->wasEventTriggered( testEvent );
      CPPUNIT_ASSERT( !wasTriggered );

      // trigger an event
      player->triggerEvent( testEvent );

      // and advance to the next frame - this time we should see that an event was triggered
      animWorld.tickAnimations( 1.0f );
      wasTriggered = player->wasEventTriggered( testEvent );
      CPPUNIT_ASSERT( wasTriggered );

      // and the next frame, the event is gone
      animWorld.tickAnimations( 1.0f );
      wasTriggered = player->wasEventTriggered( testEvent );
      CPPUNIT_ASSERT( !wasTriggered );

      // and it stays that way in the future frames ( unless we trigger it again of course, which we don't )
      animWorld.tickAnimations( 1.0f );
      wasTriggered = player->wasEventTriggered( testEvent );
      CPPUNIT_ASSERT( !wasTriggered );
   }

}

///////////////////////////////////////////////////////////////////////////////
