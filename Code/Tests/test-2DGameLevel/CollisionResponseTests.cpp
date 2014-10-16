#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"
#include "ext-2DGameLevel\GL2DWorld.h"
#include "ext-2DGameLevel\GL2DListener.h"


///////////////////////////////////////////////////////////////////////////////

TEST( PenetrationResolution, movingAndStaticBody )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   // we have 2 spheres colliding - first we want to make sure that their penetration will be resolved.

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

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 15.0f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );

   // ball A bounced into ball B, but since the latter is static, it doesn't move
   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 8, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 10, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );

   // that position doesn't change with the next frame - the collision's still there, but the penetration resolution
   // system keeps the bodies at bay
   world.tickSimulation( 1.0f );

   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 8, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 10, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( PenetrationResolution, movingAndStaticBody_sideCollision )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   // we have 2 spheres colliding - first we want to make sure that their penetration will be resolved.

   GL2DBody* bodyA = new GL2DBody( );
   GL2DBody* bodyB = new GL2DBody( );
   bodyB->setMovable( false ); // bodyB is static
   bodyA->setShape( new GL2DCircleShape( ) );
   bodyB->setShape( new GL2DCircleShape( ) );

   Entity* objA = new Entity( );
   Entity* objB = new Entity( );

   objA->addChild( bodyA );
   objB->addChild( bodyB );

   objA->accessLocalMtx( ).setTranslation( Vector( 0, 1, 0 ) );
   objB->accessLocalMtx( ).setTranslation( Vector( 5, 0, 0 ) );

   Model scene;
   scene.addChild( objA );
   scene.addChild( objB );

   GL2DWorld world;
   scene.attachListener( &world );
   world.enableSimulation( true );

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 10.0f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );

   // ball A bounced into ball B, but since the latter is static, it doesn't move
   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 3.361f, 1.1462f, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 5, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );

   // that position doesn't change with the next frame - the collision's still there, but the penetration resolution
   // system keeps the bodies at bay
   world.tickSimulation( 1.0f );

   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 3.361f, 1.1462f, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 5, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( PenetrationResolution, twoMovableBodies_oneBodyMoving )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   // we have 2 spheres colliding - first we want to make sure that their penetration will be resolved.

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

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 10.0f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );

   // ball A bounced into ball B, the latter was also thrown away
   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 8.0f, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 10.0f, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );

   // that position doesn't change with the next frame - the collision's still there, but the penetration resolution
   // system keeps the bodies at bay
   world.tickSimulation( 1.0f );

   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 8.0f, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 10.0f, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( PenetrationResolution, twoMovableBodies_bothBodiesMoving )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   // we have 2 spheres colliding - first we want to make sure that their penetration will be resolved.

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

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );

   // Send body A moving fast towards body B. After 1s of simulation, they should collide with one another.
   bodyA->setLinearVelocity( Vector( 4.5f, 0.0f, 0.0f ) );
   bodyB->setLinearVelocity( Vector( -4.5f, 0.0f, 0.0f ) );
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );

   // ball A bounced into ball B, the latter was also thrown away
   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 4.0f, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 6.0f, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );

   // that position doesn't change with the next frame - the collision's still there, but the penetration resolution
   // system keeps the bodies at bay
   world.tickSimulation( 1.0f );

   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 4.0f, 0, 0 ), objA->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 6.0f, 0, 0 ), objB->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyA->getLinearVelocity( ) );
   COMPARE_VEC( Vector( 0, 0, 0 ), bodyB->getLinearVelocity( ) );

}

///////////////////////////////////////////////////////////////////////////////

TEST( PenetrationResolution, twoBodiesSqueezingOneBody )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;


   GL2DBody* topBody = new GL2DBody( );
   GL2DBody* squeezedBody = new GL2DBody( );
   GL2DBody* bottomBody = new GL2DBody( );
   bottomBody->setMovable( false ); // the bottom-most body is static
   topBody->setShape( new GL2DCircleShape( ) );
   squeezedBody->setShape( new GL2DCircleShape( ) );
   bottomBody->setShape( new GL2DCircleShape( ) );

   Entity* topObj = new Entity( );
   Entity* squeezedObj = new Entity( );
   Entity* bottomObj = new Entity( );

   topObj->addChild( topBody );
   squeezedObj->addChild( squeezedBody );
   bottomObj->addChild( bottomBody );

   bottomObj->accessLocalMtx( ).setTranslation( Vector( 0, 0, 0 ) );
   squeezedObj->accessLocalMtx( ).setTranslation( Vector( 1.5f, 0, 0 ) );
   topObj->accessLocalMtx( ).setTranslation( Vector( 3.0f, 0, 0 ) );

   Model scene;
   scene.addChild( bottomObj );
   scene.addChild( squeezedObj );
   scene.addChild( topObj );

   GL2DWorld world;
   scene.attachListener( &world );
   world.enableSimulation( true );

   // first update, bodies are far apart - there's no collision
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.5f );

   // the squeezed and top bodies should be pushed up ( bottom body can't move, since it's static, 
   // so it plays the role of the ground )
   scene.getRoot( )->updateTransforms( );
   COMPARE_VEC( Vector( 0, 0, 0 ), bottomObj->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 2, 0, 0 ), squeezedObj->getGlobalMtx( ).position( ) );
   COMPARE_VEC( Vector( 4, 0, 0 ), topObj->getGlobalMtx( ).position( ) );
}

///////////////////////////////////////////////////////////////////////////////
