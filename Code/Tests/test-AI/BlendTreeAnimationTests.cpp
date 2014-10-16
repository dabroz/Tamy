#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"


///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeAnimation, animationEvents )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // create a dummy animation
   SnapshotAnimation animation;
   animation.m_duration = 2.0f;

   // define a blend tree
   BlendTree tree;
   BlendTreeEvent* btEvent = new BlendTreeEvent();
   {
      // define the skeleton
      tree.setSkeleton( &skeleton );

      // define tree events
      tree.addEvent( btEvent );

      // create a mock animation


      BlendTreeAnimation* animationNode = new BlendTreeAnimation( "Anim", &animation );
      tree.getRoot().add( animationNode );

      // define events
      animationNode->addEvent( new BlendTreeAnimationEvent( btEvent, 0.75f ) );
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

   // test if the event is triggered
   {
      // keep in mind that the events are received in the next frame

      // frame 1 - 1 sec passes - no events triggered in the frame before ( since there was none )
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !player->wasEventTriggered( btEvent ) );

      // frame 2 - 2 sec passes, animation loops - an event was triggered, we should hear about it the next frame
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !player->wasEventTriggered( btEvent ) );

      // frame 3 - 1 sec passes - no events triggered, but we just received a report that an event was triggered last frame
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( player->wasEventTriggered( btEvent ) );

      // frame 4 - 2 sec passes - no events triggered in the previous frame, one was triggered again now though ( will be there the next frame )
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !player->wasEventTriggered( btEvent ) );

      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( player->wasEventTriggered( btEvent ) );
   }
}

///////////////////////////////////////////////////////////////////////////////


TEST( BlendTreeAnimation, generatingSynchronizationInfo )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // create a dummy animation
   SnapshotAnimation animation;
   animation.m_duration = 2.0f;

   // define a blend tree
   BlendTree tree;
   BlendTreeEvent* btEvent = new BlendTreeEvent();
   BlendTreeAnimation* animationNode = NULL;
   {
      // define the skeleton
      tree.setSkeleton( &skeleton );

      // define tree events
      tree.addEvent( btEvent );

      // create a mock animation
      animationNode = new BlendTreeAnimation( "Anim", &animation );
      tree.getRoot().add( animationNode );

      // define sync points
      animationNode->addSyncPoint( new BlendTreeSyncPoint( btEvent, 0.75f ) );
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

   // test what sync events are propagated
   {
      animWorld.tickAnimations( 1.0f );
      // the beginning of the track, the event is located at 1.5s, so we have 1.5s remaining ( sync point lasts 2s, so the progress is 0.25% )
      // ( info is generated at the beginning of the frame, before we account the elapsed time )
      {
         const BlendTreeSyncProfile& syncData = player->getSyncData();
         CPPUNIT_ASSERT_EQUAL( (uint)1, syncData.m_eventsCount );
         CPPUNIT_ASSERT_EQUAL( btEvent, syncData.m_btEvent[0] );
         CPPUNIT_ASSERT_EQUAL( 0.25f, syncData.m_progress[0] ); 
      }

      animWorld.tickAnimations( 0.7f ); // 1s has passed
      {
         const BlendTreeSyncProfile& syncData = player->getSyncData();
         CPPUNIT_ASSERT_EQUAL( (uint)1, syncData.m_eventsCount );
         CPPUNIT_ASSERT_EQUAL( btEvent, syncData.m_btEvent[0] );
         CPPUNIT_ASSERT_EQUAL( 0.75f, syncData.m_progress[0] );
      }

      animWorld.tickAnimations( 1.0f ); // 1.7s has passed
      {
         const BlendTreeSyncProfile& syncData = player->getSyncData();
         CPPUNIT_ASSERT_EQUAL( (uint)1, syncData.m_eventsCount );
         CPPUNIT_ASSERT_EQUAL( btEvent, syncData.m_btEvent[0] );

         // since we're looping around, and there's 0.3f left till the end of the track,
         // plus another 1.5s till the event - that gives us 1.8s total until the event is hit
         CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1f, syncData.m_progress[0], 1e-3f );
      }
   } 

}

///////////////////////////////////////////////////////////////////////////////
