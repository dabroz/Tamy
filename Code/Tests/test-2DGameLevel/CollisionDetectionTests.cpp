#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"
#include "ext-2DGameLevel\GL2DWorld.h"
#include "ext-2DGameLevel\GL2DListener.h"
#include "ext-2DGameLevel\GL2DCollisionPointsCollector.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct MockCollisionsListener : public GL2DListener
   {
      DECLARE_ALLOCATOR( MockCollisionsListener, AM_DEFAULT );

      int                         m_collisionsCount;
      GL2DContactPointManifold    m_collisionPoints[16];

      MockCollisionsListener( )
         : m_collisionsCount( 0 )
      {}

      void onCollision( const GL2DContactPointManifold& collisionPoints )
      {
         m_collisionPoints[m_collisionsCount] = collisionPoints;
         ++m_collisionsCount;
      }

   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( CollisionDetection, twoCirclesColliding )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   GL2DBody* bodyA = new GL2DBody( );
   GL2DBody* bodyB = new GL2DBody( );
   bodyA->setShape( new GL2DCircleShape( ) );
   bodyB->setShape( new GL2DCircleShape( ) );

   Entity* objA = new Entity( );
   Entity* objB = new Entity( );

   objA->addChild( bodyA );
   objB->addChild( bodyB );

   objA->accessLocalMtx( ).setTranslation( Vector( 0, 0, 0 ) );
   objB->accessLocalMtx( ).setTranslation( Vector( 10, 0, 0 ) );

   Model scene;
   scene.addChild( objA );
   scene.addChild( objB );

   GL2DWorld world;
   scene.attachListener( &world );
   world.enableSimulation( true );

   MockCollisionsListener collisionsListener;
   world.attachListener( &collisionsListener );

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );
   CPPUNIT_ASSERT_EQUAL( 0, collisionsListener.m_collisionsCount );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 10.0f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 2, collisionsListener.m_collisionsCount );

   const GL2DContactPointManifold& manifold = collisionsListener.m_collisionPoints[0];
   COMPARE_VEC( Vector( 9.0f, 0.0f, 0.0f ), manifold.m_penetrationContact.m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, manifold.m_penetrationContact.m_contactNormal );

}

///////////////////////////////////////////////////////////////////////////////

TEST( CollisionDetection, circleAndRectangleColliding )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   GL2DBody* bodyA = new GL2DBody( );
   GL2DBody* bodyB = new GL2DBody( );
   bodyA->setShape( new GL2DCircleShape( ) );

   GL2DLineSegmentsShape* staticGeometry = new GL2DLineSegmentsShape();
   bodyB->setShape( staticGeometry );

   Transform rectTransform;
   rectTransform.m_rotation.setIdentity();
   rectTransform.m_translation.set( 10, 0, 0 );
   staticGeometry->addRect( rectTransform, Vector( 0.1f, 1.0f, 1.0f ) );
   staticGeometry->build();

   Entity* objA = new Entity( );
   Entity* objB = new Entity( );

   objA->addChild( bodyA );
   objB->addChild( bodyB );

   objA->accessLocalMtx( ).setTranslation( Vector( 0, 0, 0 ) );
   objB->accessLocalMtx( ).setTranslation( Vector( 0, 0, 0 ) );

   Model scene;
   scene.addChild( objA );
   scene.addChild( objB );

   GL2DWorld world;
   scene.attachListener( &world );
   world.enableSimulation( true );

   MockCollisionsListener collisionsListener;
   world.attachListener( &collisionsListener );

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );
   CPPUNIT_ASSERT_EQUAL( 0, collisionsListener.m_collisionsCount );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 10.0f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 1, collisionsListener.m_collisionsCount );

   const GL2DContactPointManifold& manifold = collisionsListener.m_collisionPoints[0];
   COMPARE_VEC( Vector( 9.9f, 0.0f, 0.0f ), manifold.m_penetrationContact.m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, manifold.m_penetrationContact.m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////