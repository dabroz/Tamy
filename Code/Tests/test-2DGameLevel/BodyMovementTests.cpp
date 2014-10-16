#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"
#include "ext-2DGameLevel\GL2DWorld.h"


///////////////////////////////////////////////////////////////////////////////

TEST( BodyMovement, velocities )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   GL2DBody* body = new GL2DBody( );
   body->setShape( new GL2DCircleShape( 1.0f ) );

   Entity* obj = new Entity( );
   obj->addChild( body );
   obj->accessLocalMtx( ).setTranslation( Vector_ZERO );

   Model scene;
   scene.addChild( obj );
   GL2DWorld world;
   scene.attachListener( &world );
   world.enableSimulation( true );

   // the body doesn't have any velocity, and there's no gravity, so it just stays put
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );
   COMPARE_VEC( Vector_ZERO, obj->getLocalMtx( ).position( ) );
   COMPARE_VEC( Vector_ZERO, body->getLinearVelocity( ) );

   Quaternion actualOrientation, expectedOrientation;
   expectedOrientation.setIdentity( );

   // set some linear velocity
   body->setLinearVelocity( Vector( 1.0f, 0.0f, 0.0f ) );

   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.1f );
   COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), body->getLinearVelocity( ) );

   // and the body will have traveled a distance of 0.1m
   COMPARE_VEC( Vector( 0.1f, 0.0f, 0.0f ), obj->getLocalMtx( ).position( ) );
   obj->getLocalMtx( ).getRotation( actualOrientation );
   COMPARE_QUAT( expectedOrientation, actualOrientation );


   // the body continues moving with the velocity set by the user
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.1f );
   COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), body->getLinearVelocity( ) );

   // and the body will now have traveled a total distance of 0.2m
   COMPARE_VEC( Vector( 0.2f, 0.0f, 0.0f ), obj->getLocalMtx( ).position( ) );
   obj->getLocalMtx( ).getRotation( actualOrientation );
   COMPARE_QUAT( expectedOrientation, actualOrientation );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BodyMovement, bodiesConnectedInAHierarchy )
{
   GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY;

   EulerAngles parentObjRot; parentObjRot.set( 0.0f, 0.0f, 90.0f );

   Model scene;
   GL2DWorld world;
   GL2DBody* parentBody;
   GL2DBody* childBody;
   Entity* parentObj;
   Entity* childObj;
   {
      parentBody = new GL2DBody( );
      childBody = new GL2DBody( );
      parentBody->setShape( new GL2DCircleShape( 1.0f ) );
      childBody->setShape( new GL2DCircleShape( 1.0f ) );

      childObj = new Entity( );
      childObj->addChild( childBody );
      childObj->accessLocalMtx( ).setTranslation( Vector( 10.0f, 0.0f, 0.0f ) );

      parentObj = new Entity( );
      parentObj->addChild( parentBody );
      parentObj->addChild( childObj );

      // parent is rotated initially
      parentObj->accessLocalMtx( ).setRotation( parentObjRot );

      scene.addChild( parentObj );
      scene.attachListener( &world );
      world.enableSimulation( true );
   }

   // the body doesn't have any velocity, and there's no gravity, so it just stays put
   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 1.0f );
   {
      Matrix expectedParentGlobalMtx, expectedChildGlobalMtx;
      expectedParentGlobalMtx.setRotation( parentObjRot );
      expectedParentGlobalMtx.setPosition< 3 >( Vector( 0.0f, 0.0f, 0.0f ) );

      // parent's system is rotated, so the local space (10,0,0) becomes world space (0, 0, 0)
      expectedChildGlobalMtx.setRotation( parentObjRot );
      expectedChildGlobalMtx.setPosition< 3 >( Vector( 0.0f, 10.0f, 0.0f ) );

      COMPARE_MTX( expectedParentGlobalMtx, parentObj->getGlobalMtx( ) );
      COMPARE_VEC( Vector_ZERO, parentBody->getLinearVelocity( ) );
      COMPARE_MTX( expectedChildGlobalMtx, childObj->getGlobalMtx( ) );
      COMPARE_VEC( Vector_ZERO, childBody->getLinearVelocity( ) );
   }

   // set velocities
   parentBody->setLinearVelocity( Vector( 0.0f, 1.0f, 0.0f ) );
   childBody->setLinearVelocity( Vector( 1.0f, 0.0f, 0.0f ) );

   scene.getRoot( )->updateTransforms( );
   world.tickSimulation( 0.1f );
   {
      scene.getRoot( )->updateTransforms( );

      Matrix expectedParentGlobalMtx, expectedChildGlobalMtx;
      expectedParentGlobalMtx.setRotation( parentObjRot );
      expectedParentGlobalMtx.setPosition< 3 >( Vector( 0.0f, 0.1f, 0.0f ) );

      // even though the child belongs to a rotated system, physical velocity works in world space
      // and that's how it will affect the body
      // The additional Y offset on the Y axis comes from the propulsion provided by the parent body
      expectedChildGlobalMtx.setRotation( parentObjRot );
      expectedChildGlobalMtx.setPosition< 3 >( Vector( 0.1f, 10.1f, 0.0f ) );

      COMPARE_MTX( expectedParentGlobalMtx, parentObj->getGlobalMtx( ) );
      COMPARE_VEC( Vector( 0.0f, 1.0f, 0.0f ), parentBody->getLinearVelocity( ) );
      COMPARE_MTX( expectedChildGlobalMtx, childObj->getGlobalMtx( ) );
      COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), childBody->getLinearVelocity( ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
