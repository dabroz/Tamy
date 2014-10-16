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
      bool&                   m_wasSampled;

   public:
      BlendTreeMockNode( bool& isActive, bool& wasSampled )
         : m_currTime( 0.0f )
         , m_synchronizeToMe( false )
         , m_isActive( isActive )
         , m_wasSampled( wasSampled )
      {
      }

      void onActivateNode( BlendTreePlayer* player ) const override
      {
         m_isActive = true;
      }

      void onDeactivateNode( BlendTreePlayer* player ) const override
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

      void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const override
      {
         m_currTime += timeDelta;
         m_wasSampled = true;
      }
   };

   // -------------------------------------------------------------------------

   class BlendTreeEffectMock : public BlendTreeTransitionEffect
   {
      DECLARE_ALLOCATOR( BlendTreeEffectMock, AM_ALIGNED_16 );

   private:
      bool*                m_isActive;
      float                m_duration;

   public:
      BlendTreeEffectMock( float duration, bool* isActive = NULL )
         : m_isActive( isActive )
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

   // -------------------------------------------------------------------------

   class BlendTreeSelectorSwitchMock : public BlendTreeSelectorSwitch
   {
   private:
      int&          m_desiredNodeIdx;

   public:
      BlendTreeSelectorSwitchMock( int& desiredNodeIdx )
         : m_desiredNodeIdx( desiredNodeIdx )
      {}

      int getNodeIdx( BlendTreePlayer* player ) const
      {
         return m_desiredNodeIdx;
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeSelector, switchingNodes )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeNodeIdx = 0;
   bool nodeActivated[] = { false, false };
   bool wasNodeSampled[] = { false, false };
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();

      BlendTreeSelector* selector = new BlendTreeSelector(); 
      rootStateMachine.add( selector );
      selector->setSwitch( new BlendTreeSelectorSwitchMock( activeNodeIdx ) );

      BlendTreeMockNode* node1 = new BlendTreeMockNode( nodeActivated[0], wasNodeSampled[0] );
      BlendTreeMockNode* node2 = new BlendTreeMockNode( nodeActivated[1], wasNodeSampled[1] );
      selector->add( node1 );
      selector->add( node2 );
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

   // test which nodes become active
   {
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;

      // activeNodeIdx machine runs node 0
      activeNodeIdx = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;

      // switch to node 1
      activeNodeIdx = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;

      // try switching to state 0 back - it will fail since we didn't define such a transition
      activeNodeIdx = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;

      // switch the simulation off - both nodes should become inactive
      animWorld.play( false );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeSelector, transitionCantBeInterrupted )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeNodeIdx = 0;
   bool nodeActivated[] = { false, false, false };
   bool wasNodeSampled[] = { false, false, false };
   bool isEffectActive = false;
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();

      BlendTreeSelector* selector = new BlendTreeSelector(); 
      rootStateMachine.add( selector );
      selector->setSwitch( new BlendTreeSelectorSwitchMock( activeNodeIdx ) );

      BlendTreeMockNode* node1 = new BlendTreeMockNode( nodeActivated[0], wasNodeSampled[0] );
      BlendTreeMockNode* node2 = new BlendTreeMockNode( nodeActivated[1], wasNodeSampled[1] );
      BlendTreeMockNode* node3 = new BlendTreeMockNode( nodeActivated[2], wasNodeSampled[2] );
      selector->add( node1 );
      selector->add( node2 );
      selector->add( node3 );

      // selector needs 2.5s to switch between the nodes      
      selector->setEffect( new BlendTreeEffectMock( 2.5f, &isEffectActive  ) );
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
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( !nodeActivated[2] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[2] ); wasNodeSampled[2] = false;

      // activeNodeIdx machine runs node 0
      activeNodeIdx = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( !nodeActivated[2] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( !isEffectActive );

      // switch to node 1 - both node 0 & 1 become active and engaged in the transition, node 2 stays inactive
      activeNodeIdx = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( !nodeActivated[2] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( isEffectActive );

      // try switching to node 2 - it will not happen until the transition is accomplished
      activeNodeIdx = 2;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( !nodeActivated[2] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( isEffectActive );

      // in this frame transition 0-->1 comes to an end, but since only one transition can be made in one frame, the next one is not yet activated
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( !nodeActivated[2] );
      CPPUNIT_ASSERT( wasNodeSampled[0] ); wasNodeSampled[0] = false; // the transition was still coming to an end this frame, so the previously active node ( 0 ) has also been sampled
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( !isEffectActive );

      // ...and here's when the transition 1-->2 becomes active
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( nodeActivated[2] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( isEffectActive );

      // still going on...
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( nodeActivated[1] );
      CPPUNIT_ASSERT( nodeActivated[2] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( isEffectActive );

      // and the transition's over
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( nodeActivated[2] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( wasNodeSampled[1] ); wasNodeSampled[1] = false; // same here - the transition was still active this frame, so node 1, which was previously active, has also been sampled
      CPPUNIT_ASSERT( wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( !isEffectActive );

      // just one more frame to make sure it's running ok
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActivated[0] );
      CPPUNIT_ASSERT( !nodeActivated[1] );
      CPPUNIT_ASSERT( nodeActivated[2] );
      CPPUNIT_ASSERT( !wasNodeSampled[0] ); wasNodeSampled[0] = false;
      CPPUNIT_ASSERT( !wasNodeSampled[1] ); wasNodeSampled[1] = false;
      CPPUNIT_ASSERT( wasNodeSampled[2] ); wasNodeSampled[2] = false;
      CPPUNIT_ASSERT( !isEffectActive );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeSelector, activatedNodesSynchronization )
{
   // setup reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   int activeNodeIdx = 0;
   bool nodeActivated[] = { false, false, false };
   bool wasNodeSampled[] = { false, false, false };
   BlendTreeMockNode* node1 = NULL;
   BlendTreeMockNode* node2 = NULL;
   BlendTreeMockNode* node3 = NULL;
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();

      BlendTreeSelector* selector = new BlendTreeSelector(); 
      rootStateMachine.add( selector );
      selector->setSwitch( new BlendTreeSelectorSwitchMock( activeNodeIdx ) );

      node1 = new BlendTreeMockNode( nodeActivated[0], wasNodeSampled[0] ); node1->m_synchronizeToMe = true;
      node2 = new BlendTreeMockNode( nodeActivated[1], wasNodeSampled[1] ); node2->m_synchronizeToMe = true;
      node3 = new BlendTreeMockNode( nodeActivated[2], wasNodeSampled[2] ); node3->m_synchronizeToMe = true;
      selector->add( node1 );
      selector->add( node2 );
      selector->add( node3 );

      // selector needs 2.5s to switch between the nodes      
      selector->setEffect( new BlendTreeEffectMock( 1.0f ) );
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
      activeNodeIdx = 0;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 1.0f, node1->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 0.0f, node2->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 0.0f, node3->m_currTime );

      // we're starting the transition - both nodes become active
      activeNodeIdx = 1;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 2.0f, node1->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 2.0f, node2->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 0.0f, node3->m_currTime );

      activeNodeIdx = 2;
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT_EQUAL( 2.0f, node1->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 3.0f, node2->m_currTime );
      CPPUNIT_ASSERT_EQUAL( 3.0f, node3->m_currTime );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////
