#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"



///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class BTNActivationTestingMock : public BlendTreeNode
   {
      DECLARE_ALLOCATOR( BTNActivationTestingMock, AM_ALIGNED_16 );

   private:
      bool&                   m_isActive;
      bool&                   m_wasPoseSampled;

   public:
      BTNActivationTestingMock( bool& isActivate, bool& wasPoseSampled )
         :  m_isActive( isActivate )
         , m_wasPoseSampled( wasPoseSampled )
      {
      }

      void onActivateNode( BlendTreePlayer* player ) const
      {
         m_isActive = true;
      }

      void onDeactivateNode( BlendTreePlayer* player ) const 
      {
         m_isActive = false;
      }

      void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, uint bonesCount ) const
      {
         m_wasPoseSampled = true;
      }
   };

   // -------------------------------------------------------------------------

   class BTNSynchronizationTestingMock : public BlendTreeNode
   {
      DECLARE_ALLOCATOR( BTNSynchronizationTestingMock, AM_ALIGNED_16 );

   public:
      const float             m_duration;
      mutable float           m_currTime;

   public:
      BTNSynchronizationTestingMock( float duration )
         : m_duration( duration )
         , m_currTime( 0.0f )
      {
      }

      void onGenerateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const
      {
         outSyncData.submit( (BlendTreeEvent*)1, m_currTime );
      }

      void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
      {
         RuntimeDataBuffer& data = player->data();
         if ( data[m_state] == ToSynchronize )
         {
            if ( syncData.m_eventsCount > 0 )
            {
               m_currTime = syncData.m_progress[0] * m_duration;
            }
            else
            {
               m_currTime = 0.0f;
            }
         }

         outNodeSyncProfile.setSyncPoint( 0, m_duration - m_currTime );
      }

      void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, uint bonesCount ) const
      {
         m_currTime += timeDelta;
      }
   };


}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeBlender1D, activatingChildren )
{
   // register reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   bool nodeActive[] = { false, false, false, false, false };
   bool wasPoseSampled[] = { false, false, false, false, false };
   BTVarFloat* blendControlParam = NULL;
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();

      blendControlParam = new BTVarFloat();
      tree.addVariable( blendControlParam );

      BlendTreeBlender1D*  blender = new BlendTreeBlender1D();
      rootStateMachine.add( blender );

      BTNActivationTestingMock* node1 = new BTNActivationTestingMock( nodeActive[0], wasPoseSampled[0] );
      BTNActivationTestingMock* node2 = new BTNActivationTestingMock( nodeActive[1], wasPoseSampled[1] );
      BTNActivationTestingMock* node3 = new BTNActivationTestingMock( nodeActive[2], wasPoseSampled[2] );

      blender->add( node1 );
      blender->add( node2 );
      blender->add( node3 );

      // assign blend weights
      blender->assignParameterValue( 0,  0.0f );
      blender->assignParameterValue( 1,  1.0f );
      blender->assignParameterValue( 2, -3.0f );
      blender->setControlParameter( blendControlParam );
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

   // test if the poses are being assembled ( just to remind, param values are: 0, 1, -3 )
   {
      // values are exactly the same as our control points
      blendControlParam->setRuntime( player, 0.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( !nodeActive[2] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[2] ); wasPoseSampled[2] = false;


      blendControlParam->setRuntime( player, 1.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( !nodeActive[2] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[2] ); wasPoseSampled[2] = false;

      blendControlParam->setRuntime( player, -3.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( nodeActive[2] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( wasPoseSampled[2] ); wasPoseSampled[2] = false;

      // control value goes out of range
      blendControlParam->setRuntime( player, -4.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( nodeActive[2] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( wasPoseSampled[2] ); wasPoseSampled[2] = false;

      blendControlParam->setRuntime( player, 2.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( !nodeActive[0] );
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( !nodeActive[2] );
      CPPUNIT_ASSERT( !wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[2] ); wasPoseSampled[2] = false;

      // control value falls in between two nodes
      blendControlParam->setRuntime( player, -1.0f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( !nodeActive[1] );
      CPPUNIT_ASSERT( nodeActive[2] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( wasPoseSampled[2] ); wasPoseSampled[2] = false;

      blendControlParam->setRuntime( player, 0.5f );
      animWorld.tickAnimations( 1.0f );
      CPPUNIT_ASSERT( nodeActive[0] );
      CPPUNIT_ASSERT( nodeActive[1] );
      CPPUNIT_ASSERT( !nodeActive[2] );
      CPPUNIT_ASSERT( wasPoseSampled[0] ); wasPoseSampled[0] = false;
      CPPUNIT_ASSERT( wasPoseSampled[1] ); wasPoseSampled[1] = false;
      CPPUNIT_ASSERT( !wasPoseSampled[2] ); wasPoseSampled[2] = false;
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeBlender1D, activatedNodesSynchronization )
{
   // register reflection types
   BLENDTREETESTS_INIT_TYPES_REGISTRY();

   // define a skeleton
   Skeleton skeleton;

   // define a blend tree
   BlendTree tree;
   BTNSynchronizationTestingMock* node1 = NULL;
   BTNSynchronizationTestingMock* node2 = NULL;
   {
      tree.setSkeleton( &skeleton );

      BlendTreeStateMachine& rootStateMachine = tree.getRoot();

      BTVarFloat* blendControlParam = new BTVarFloat();
      blendControlParam->set( 0.5f );
      tree.addVariable( blendControlParam );

      BlendTreeBlender1D* blender = new BlendTreeBlender1D();
      rootStateMachine.add( blender );

      node1 = new BTNSynchronizationTestingMock( 1.0f );
      node2 = new BTNSynchronizationTestingMock( 2.0f );

      blender->add( node1 );
      blender->add( node2 );

      // assign blend weights
      blender->assignParameterValue( 0,  0.0f );
      blender->assignParameterValue( 1,  1.0f );
      blender->setControlParameter( blendControlParam );
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
      animWorld.tickAnimations( 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.06f, node1->m_currTime, 0.01f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.13f, node2->m_currTime, 0.01f );
      
      animWorld.tickAnimations( 0.1f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.13f, node1->m_currTime, 0.01f );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.26f, node2->m_currTime, 0.01f );
   }

   // cleanup
   scene.detachListener( &animWorld );
}

///////////////////////////////////////////////////////////////////////////////
