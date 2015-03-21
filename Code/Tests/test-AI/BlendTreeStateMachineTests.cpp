#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"



///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class BlendTreeMockNode : public BlendTreeNode
   {
      DECLARE_ALLOCATOR( BlendTreeMockNode, AM_ALIGNED_16 );

   public:
      mutable float           m_currTime;
      bool                    m_synchronizeToMe;

   private:
      bool&                   m_isActive;
      bool&                   m_wasPoseSampled;

   public:
      BlendTreeMockNode( bool& isActivate, bool& wasPoseSampled )
         : m_currTime( 0.0f )
         , m_synchronizeToMe( false )
         , m_isActive( isActivate )
         , m_wasPoseSampled( wasPoseSampled )
      {
      }

      void onActivateNode( BlendTreePlayer* player ) const override
      {
         m_isActive = true;
      }

      void onDeactivateNode( BlendTreePlayer* player ) const  override
      {
         m_isActive = false;
      }

      void onGenerateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const override
      {
         if ( m_synchronizeToMe )
         {
            outSyncData.submit( (BlendTreeEvent*)1, m_currTime );
         }
      }

      void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const override
      {
         if ( syncData.m_eventsCount > 0 )
         {
            m_currTime = syncData.m_progress[0];
         }
         else
         {
            m_currTime = 0.0f;
         }

         outNodeSyncProfile.setSyncPoint( 0, 20.0f - m_currTime );
      }

      void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const override
      {
         m_currTime += timeDelta;
         m_wasPoseSampled = true;
      }
   };

   // -------------------------------------------------------------------------

   class BlendTreeSyncableNodeMock : public BlendTreeNode
   {
      DECLARE_ALLOCATOR( BlendTreeSyncableNodeMock, AM_ALIGNED_16 );

   public:
      mutable float           m_currTime;
      float                   m_duration;
      BlendTreeEvent*         m_syncEvent;

   public:
      BlendTreeSyncableNodeMock( float duration )
         : m_currTime( 0.0f )
         , m_duration( duration )
         , m_syncEvent( NULL )
      {
      }

      void onGenerateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const
      {
         int syncPointIdx = outSyncData.submit( m_syncEvent, m_currTime );
      }

      void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
      {
         if ( syncData.m_eventsCount > 0 )
         {
            m_currTime = syncData.m_progress[0];
         }
         else
         {
            m_currTime = 0.0f;
         }

         int syncPointIdx = syncData.getSyncPointIdx( m_syncEvent );

         outNodeSyncProfile.setSyncPoint( syncPointIdx, m_duration - m_currTime );
      }

      float getRemainingSyncPointTime( BlendTreePlayer* player ) const
      {
         return m_duration - m_currTime;
      }
   };

   // -------------------------------------------------------------------------

   class BlendTreeMockTrigger : public BlendTreeTransitionTrigger
   {
      DECLARE_ALLOCATOR( BlendTreeMockTrigger, AM_ALIGNED_16 );

   private:
      int      m_desiredTriggerValue;
      int&     m_triggerValue;

   public:
      BlendTreeMockTrigger( int desiredTriggerValue, int& triggerValue )
         : m_desiredTriggerValue( desiredTriggerValue )
         , m_triggerValue( triggerValue )
      {}

   public:
      bool testActivation( BlendTreePlayer* player ) const
      {
         return m_desiredTriggerValue == m_triggerValue;
      }
   };

   // -------------------------------------------------------------------------

   class BlendTreeEffectMock : public BlendTreeTransitionEffect
   {
      DECLARE_ALLOCATOR( BlendTreeEffectMock, AM_ALIGNED_16 );

   public:
      float                m_targetAnimStartTime;

   private:
      bool*                m_isActive;
      float                m_duration;

   public:
      BlendTreeEffectMock( float duration, bool* isActive = NULL )
         : m_targetAnimStartTime( 0.0f )
         , m_isActive( isActive )
         , m_duration( duration )
      {}

      void initializeLayout( BlendTreePlayer* player ) const override
      {
         BlendTreeTransitionEffect::initializeLayout( player );
         setDuration( player, m_duration );
      }

      void activateEffect( BlendTreePlayer* player, const BlendTreeNode* startNode, const BlendTreeNode* endNode ) const override
      {
         BlendTreeTransitionEffect::activateEffect( player, startNode, endNode );

         if ( m_isActive )
         {
            *m_isActive = true;
         }
      }

   
      void deactivateEffect( BlendTreePlayer* player ) const override
      {
         BlendTreeTransitionEffect::deactivateEffect( player );

         if ( m_isActive )
         {
            *m_isActive = false;
         }
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeStateMachine, switchingStates )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeState = 0;
   bool nodeActive[] = { false, false };
   bool wasPoseSampled[] = { false, false };
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();
      BlendTreeMockNode* state1 = new BlendTreeMockNode( nodeActive[0], wasPoseSampled[0] );
      BlendTreeMockNode* state2 = new BlendTreeMockNode( nodeActive[1], wasPoseSampled[1] );
      rootStateMachine.add( state1 ); // state 1 that sets pose 1
      rootStateMachine.add( state2 ); // state 2 that sets pose 2

      BlendTreeStateTransition* transition = new BlendTreeStateTransition();
      transition->setConnection( state1, state2 );
      transition->setTransitionTrigger( new BlendTreeMockTrigger(  1, activeState ) );
      rootStateMachine.addTransition( transition );
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
      // state machine runs state 0
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
      
      // switch to state 1 - it's possible since there's a transition there that allows us to do it
      activeState = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;

      // try switching to state 0 back - it will fail since we didn't define such a transition
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeStateMachine, transitionActivation )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeState = 0;
   bool nodeActive[] = { false, false };
   bool wasPoseSampled[] = { false, false };
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();
      BlendTreeMockNode* state1 = new BlendTreeMockNode( nodeActive[0], wasPoseSampled[0] );
      BlendTreeMockNode* state2 = new BlendTreeMockNode( nodeActive[1], wasPoseSampled[1] );
      rootStateMachine.add( state1 ); // state 1 that sets pose 1
      rootStateMachine.add( state2 ); // state 2 that sets pose 2

      // transition 0 --> 1
      BlendTreeStateTransition* transition_0_1 = new BlendTreeStateTransition();
      transition_0_1->setConnection( state1, state2 );
      transition_0_1->setTransitionTrigger( new BlendTreeMockTrigger( 1, activeState ) );
      rootStateMachine.addTransition( transition_0_1 );

      // transition 1 --> 0
      BlendTreeStateTransition* transition_1_0 = new BlendTreeStateTransition();
      transition_1_0->setConnection( state2, state1 );
      transition_1_0->setTransitionTrigger( new BlendTreeMockTrigger( 0, activeState ) );
      rootStateMachine.addTransition( transition_1_0 );

      // both effects takes 2.5s
      transition_0_1->setEffect( new BlendTreeEffectMock( 2.5f ) );
      transition_1_0->setEffect( new BlendTreeEffectMock( 2.5f ) );
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

   // transition from state 0 to state 1 - no interruptions
   {
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( !wasPoseSampled[1] );  wasPoseSampled[1] = false;

      // we're starting the transition - both nodes become active
      activeState = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;

      // the transition still takes place
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;

      // and now it's over ( but the pose still was sampled as the transition was coming to an end )
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;

      // now that the transition has completed, we're only running state 1
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;
   }

   // now transition back from state 1 to state 0, but this time interrupt the transition
   {
      // we're starting the transition - both nodes become active
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;

      // interrupt the transition by stopping the simulation - make sure it gets cleaned up
      animWorld.play( false );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeStateMachine, transitionEffectActivation )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeState = 0;
   bool isEffectActive = false;
   bool nodeActive[] = { false, false };
   bool wasPoseSampled[] = { false, false };
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();
      BlendTreeMockNode* state1 = new BlendTreeMockNode( nodeActive[0], wasPoseSampled[0] );
      BlendTreeMockNode* state2 = new BlendTreeMockNode( nodeActive[1], wasPoseSampled[1] );
      rootStateMachine.add( state1 ); // state 1 that sets pose 1
      rootStateMachine.add( state2 ); // state 2 that sets pose 2

      BlendTreeStateTransition* transition = new BlendTreeStateTransition();
      transition->setConnection( state1, state2 );
      transition->setTransitionTrigger( new BlendTreeMockTrigger(  1, activeState ) );
      rootStateMachine.addTransition( transition );

      // the effect takes 2.5s
      transition->setEffect( new BlendTreeEffectMock( 2.5f, &isEffectActive ) );
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
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !isEffectActive );

      // we're starting the transition
      activeState = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( isEffectActive );

      // the transition still takes place
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( isEffectActive );

      // and now it's over
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !isEffectActive );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeStateMachine, activatedStateSynchronization )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeState = 0;
   bool nodeActive[] = { false, false };
   bool wasPoseSampled[] = { false, false };
   BlendTreeMockNode* state1 = NULL;
   BlendTreeMockNode* state2 = NULL;
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();
      state1 = new BlendTreeMockNode( nodeActive[0], wasPoseSampled[0] ); state1->m_synchronizeToMe = true;
      state2 = new BlendTreeMockNode( nodeActive[1], wasPoseSampled[1] ); state2->m_synchronizeToMe = true;
      rootStateMachine.add( state1 ); // state 1 that sets pose 1
      rootStateMachine.add( state2 ); // state 2 that sets pose 2

      // transition 0 --> 1 - no transition effect
      BlendTreeStateTransition* transition_0_1 = new BlendTreeStateTransition();
      transition_0_1->setConnection( state1, state2 );
      transition_0_1->setTransitionTrigger( new BlendTreeMockTrigger( 1, activeState ) );
      rootStateMachine.addTransition( transition_0_1 );


      // transition 1 --> 0 - there's a transition effect in use here
      BlendTreeStateTransition* transition_1_0 = new BlendTreeStateTransition();
      transition_1_0->setConnection( state2, state1 );
      transition_1_0->setTransitionTrigger( new BlendTreeMockTrigger( 0, activeState ) );
      rootStateMachine.addTransition( transition_1_0 );

      transition_1_0->setEffect( new BlendTreeEffectMock( 1.0f ) );
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

   // transition from state 0 to state 1 - no transition effect ( an immediate transition )
   {
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 1.0f, state1->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 0.0f, state2->m_currTime );

      // we're starting the transition - both nodes become active
      activeState = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 2.0f, state1->m_currTime ); // deactivated, but it was still sampled this frame
      CPPUNIT_ASSERT_EQUAL( 2.0f, state2->m_currTime ); // activated at 1st second of the other clip, and then added another second that passed during this frame

      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 2.0f, state1->m_currTime ); // inactive and not sampled
      CPPUNIT_ASSERT_EQUAL( 3.0f, state2->m_currTime ); // activate
   }

   // let's head back - this transition uses an effect
   {
      activeState = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 4.0f, state1->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 4.0f, state2->m_currTime );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeStateMachine, synchronizingtwoStates )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   BlendTreeEvent syncEvent1;
   BlendTreeEvent syncEvent2;
   BlendTreeSyncableNodeMock* state1 = NULL;
   BlendTreeSyncableNodeMock* state2 = NULL;
   int activeState = 0;
   {
      tree.setSkeleton( &skeleton );

      const float anim1Duration = 1.0f;
      const float anim2Duration = 2.0f;

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();
      state1 = new BlendTreeSyncableNodeMock( anim1Duration );
      state2 = new BlendTreeSyncableNodeMock( anim2Duration );
      rootStateMachine.add( state1 ); // state 1 that sets pose 1
      rootStateMachine.add( state2 ); // state 2 that sets pose 2

      // transition 0 --> 1
      BlendTreeStateTransition* transition_0_1 = new BlendTreeStateTransition();
      transition_0_1->setConnection( state1, state2 );
      transition_0_1->setTransitionTrigger( new BlendTreeMockTrigger( 1, activeState ) );
      rootStateMachine.addTransition( transition_0_1 );
      transition_0_1->setEffect( new BTTEBlend( 1.0f ) );

      // transition 1 --> 0
      BlendTreeStateTransition* transition_1_0 = new BlendTreeStateTransition();
      transition_1_0->setConnection( state2, state1 );
      transition_1_0->setTransitionTrigger( new BlendTreeMockTrigger( 0, activeState ) );
      rootStateMachine.addTransition( transition_1_0 );
      transition_1_0->setEffect( new BTTEBlend( 1.0f ) );
   }

   // create an animated entity
   Entity* entity = new Entity();
   BlendTreePlayer* player = NULL;
   SkeletonComponent* skeletonComponent = NULL;
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

   // test case 1: two nodes that produce the same sync event
   {
      // configure sync events in use
      state1->m_syncEvent = &syncEvent1;
      state2->m_syncEvent = &syncEvent1;

      // activate a transition
      activeState = 1;
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state1->getPlaybackSpeed( player ), 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 2.0f, state2->getPlaybackSpeed( player ), 0.1f );

      // transition's over
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6f, state1->getPlaybackSpeed( player ), 0.1f ); // this one's no longer playing
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.3f, state2->getPlaybackSpeed( player ), 0.1f );

      // let's see what happens next - just to be sure that the playback speed will remain constant
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6f, state1->getPlaybackSpeed( player ), 0.1f ); // this one's no longer playing
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state2->getPlaybackSpeed( player ), 0.1f );
   }

   // test case 2: nodes produce totally different sync events - they won't be synchronized
   {
      // configure sync events in use
      state1->m_syncEvent = &syncEvent1;
      state2->m_syncEvent = &syncEvent2;

      // activate a transition
      activeState = 0;
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state1->getPlaybackSpeed( player ), 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state2->getPlaybackSpeed( player ), 0.1f );

      // transition's over
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state1->getPlaybackSpeed( player ), 0.1f ); // this one's no longer playing
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state2->getPlaybackSpeed( player ), 0.1f );

      // let's see what happens next - just to be sure that the playback speed will remain constant
      animWorld.tickAnimations( 0.5f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state1->getPlaybackSpeed( player ), 0.1f ); // this one's no longer playing
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, state2->getPlaybackSpeed( player ), 0.1f );
   }
}

///////////////////////////////////////////////////////////////////////////////
