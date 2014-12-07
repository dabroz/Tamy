#include "core-TestFramework\TestFramework.h"
#include "core\EulerAngles.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\FastFloat.h"
#include "core\MathDefs.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, fromAxisAngle )
{
   Vector arbitraryAxis( 0.3f, 0.5f, -0.2f );
   arbitraryAxis.normalize();

   Vector axis[] = { Vector( Quad_1000 ), Vector( Quad_0100 ), Vector( Quad_0010 ), Vector( Quad_Neg_1000 ), Vector( Quad_Neg_0100 ), Vector( Quad_Neg_0010 ), arbitraryAxis };
   int axisCount = 7;

   D3DXQUATERNION dxQuat;
   Quaternion tamyQuat;

   for ( int axisIdx = 0; axisIdx < axisCount; ++axisIdx )
   {
      for ( float angle = -179.0f; angle <= 179.0f; angle += 1.0f )
      {
         tamyQuat.setAxisAngle( axis[axisIdx], FastFloat::fromFloat( DEG2RAD( angle ) ) );
         D3DXQuaternionRotationAxis( &dxQuat, ( const D3DXVECTOR3* )&axis[axisIdx], DEG2RAD( angle ) );
         COMPARE_QUAT( dxQuat, tamyQuat );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, retrievingAxisAngleValues )
{
   Vector arbitraryAxis( 0.3f, 0.5f, -0.2f );
   arbitraryAxis.normalize();

   Vector axis[] = { Vector( Quad_1000 ), Vector( Quad_0100 ), Vector( Quad_0010 ), Vector( Quad_Neg_1000 ), Vector( Quad_Neg_0100 ), Vector( Quad_Neg_0010 ), arbitraryAxis };
   int axisCount = 7;

   Quaternion quat;
   Vector retAxis;
   FastFloat retAngle, signChange;
   for ( int axisIdx = 0; axisIdx < axisCount; ++axisIdx )
   {
      for ( float angle = -179.0f; angle <= 179.0f; angle += 1.0f )
      {
         if ( angle == 0.0f )
         {
            // the method doesn't work for angles == 0
            continue;
         }
         quat.setAxisAngle( axis[axisIdx], FastFloat::fromFloat( DEG2RAD( angle ) ) );
         quat.getAngle( retAngle );
         retAngle.mul( Float_Rad2Deg );
         
         CPPUNIT_ASSERT_DOUBLES_EQUAL( abs( angle ), retAngle.getFloat(), 1e-3 );

         quat.getAxis( retAxis );

         signChange.setMul( retAngle, FastFloat::fromFloat( angle ) );
         signChange.sign();
         retAxis.mul( signChange );
         COMPARE_VEC( axis[axisIdx], retAxis );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, angleDecomposition )
{
   Quaternion quat;
   Quaternion remainingQuat;
   FastFloat angle;

   {
      quat.setAxisAngle( Vector( Quad_1000 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );

      // a decomposition along the creation axis should not leave any quaternion behind
      {
         quat.decompose( Vector( Quad_1000 ), remainingQuat, angle );
         angle.mul( Float_Rad2Deg );

         COMPARE_QUAT( remainingQuat, Quaternion::IDENTITY );
         CPPUNIT_ASSERT_DOUBLES_EQUAL( 90.0f, angle.getFloat(), 1e-3 );
      }

      // a decomposition along an orthogonal axis should leave the entire quaternion untouched
      // and return angle == 0
      {
         quat.decompose( Vector( Quad_0100 ), remainingQuat, angle );
         angle.mul( Float_Rad2Deg );

         COMPARE_QUAT( remainingQuat, quat );
         CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0395f, angle.getFloat(), 1e-3 );
      }
   }

   quat.setAxisAngle( Vector( Quad_Neg_1000 ), FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
   {
      // complete removal
      {
         quat.decompose( Vector( Quad_Neg_1000 ), remainingQuat, angle );
         angle.mul( Float_Rad2Deg );

         COMPARE_QUAT( remainingQuat, Quaternion::IDENTITY );
         CPPUNIT_ASSERT_DOUBLES_EQUAL( 45.0f, angle.getFloat(), 1e-3 );
      }

      // null removal
      {
         quat.decompose( Vector( Quad_0100 ), remainingQuat, angle );
         angle.mul( Float_Rad2Deg );

         COMPARE_QUAT( remainingQuat, quat );
         CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, angle.getFloat(), 1e-1 );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, setFromShortestRotation )
{
   Quaternion quat;

   quat.setFromShortestRotation( Vector( Quad_0010 ), Vector( Quad_1000 ) );
   COMPARE_QUAT_AXIS_ANGLE( quat, Vector( Quad_0100 ), 90.0f );   

   quat.setFromShortestRotation( Vector( Quad_0010 ), Vector( Quad_Neg_1000 ) );
   COMPARE_QUAT_AXIS_ANGLE( quat, Vector( Quad_Neg_0100 ), 90.0f );

   quat.setFromShortestRotation( Vector( Quad_0100 ), Vector( Quad_0010 ) );
   COMPARE_QUAT_AXIS_ANGLE( quat, Vector( Quad_1000 ), 90.0f );

   quat.setFromShortestRotation( Vector( Quad_1000 ), Vector( Quad_0100 ) );
   COMPARE_QUAT_AXIS_ANGLE( quat, Vector( Quad_0010 ), 90.0f );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, setFromEulerAngles )
{
   Quaternion tamyQuat;
   D3DXQUATERNION dxQuat;
   EulerAngles ea;

   for ( float yaw = -179.0f; yaw <= 179.0f; yaw += 15.0f )
   {
      for ( float pitch = -179.0f; pitch <= 179.0f; pitch += 15.0f )
      {
         for ( float roll = -179.0f; roll <= 179.0f; roll += 15.0f )
         {
            ea.set( FastFloat::fromFloat( yaw ), FastFloat::fromFloat( pitch ), FastFloat::fromFloat( roll ) );

            tamyQuat.setFromEulerAngles( ea );
            D3DXQuaternionRotationYawPitchRoll( &dxQuat, DEG2RAD( yaw ), DEG2RAD( pitch ), DEG2RAD( roll ) ); 

            COMPARE_QUAT( dxQuat, tamyQuat );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, slerp )
{
   Quaternion q1, q2, resultQ;
   q1.setAxisAngle( Vector( Quad_0100 ), Float_0 );
   q2.setAxisAngle( Vector( Quad_0100 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );

   resultQ.setSlerp( q1, q2, Float_0 );
   COMPARE_QUAT( q1, resultQ );

   resultQ.setSlerp( q1, q2, Float_1 );
   COMPARE_QUAT( q2, resultQ );

   resultQ.setSlerp( q1, q2, Float_Inv2 );
   COMPARE_QUAT_AXIS_ANGLE( resultQ, Vector( Quad_0100 ), 45.0f );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, transform )
{
   Quaternion q;
   Vector result;

   q.setAxisAngle( Vector( Quad_0100 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
   q.transform( Vector( Quad_0010 ), result );
   COMPARE_VEC( Vector( Quad_1000 ), result );

   q.setAxisAngle( Vector( Quad_1000 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
   q.transform( Vector( Quad_0100 ), result );
   COMPARE_VEC( Vector( Quad_0010 ), result );

   q.setAxisAngle( Vector( Quad_0010 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
   q.transform( Vector( Quad_1000 ), result );
   COMPARE_VEC( Vector( Quad_0100 ), result );

   q.setAxisAngle( Vector( Quad_Neg_0010 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
   q.transform( Vector( Quad_1000 ), result );
   COMPARE_VEC( Vector( Quad_Neg_0100 ), result );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, worldToLocalTransform )
{
   Quaternion parentWorld, childLocal, childWorld;

   // define parent and child transforms
   parentWorld.setAxisAngle( Vector( Quad_0100 ), FastFloat::fromFloat(  DEG2RAD( 90.0f ) ) );
   childLocal.setAxisAngle( Vector( Quad_0010 ), FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );

   // calculate and test their concatenation
   childWorld.setMul( childLocal, parentWorld ); // (1) - this equation MUST be the same as equation (2) in order for this test to give true results

   {
      Vector oxConcatenatedParentChildSpace;
      childWorld.transform( Vector( Quad_1000 ), oxConcatenatedParentChildSpace );
      COMPARE_VEC( Vector( Quad_0100 ), oxConcatenatedParentChildSpace );
   }

   // introduce additional world rotation that should be applied only to the child
   Quaternion childWorldRotation;
   childWorldRotation.setAxisAngle( Vector( Quad_1000 ), FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );

   Quaternion newChildWorld;
   newChildWorld.setMul( childWorld, childWorldRotation );

   // test the new world transform
   {
      Vector oxConcatenatedParentChildSpace;
      newChildWorld.transform( Vector( Quad_1000 ), oxConcatenatedParentChildSpace );
      COMPARE_VEC( Vector( 0.0f, 0.70710659f, 0.70710659f ), oxConcatenatedParentChildSpace );
   }


   Quaternion invParentWorld;
   invParentWorld.setConjugated( parentWorld );
   childLocal.setMul( newChildWorld, invParentWorld );

   // recalculate and test the new local transform
   childWorld.setMul( childLocal, parentWorld );  // (2) - this equation MUST be the same as equation (1) in order for this test to give true results
   {
      Vector oxConcatenatedParentChildSpace;
      childWorld.transform( Vector( Quad_1000 ), oxConcatenatedParentChildSpace );
      COMPARE_VEC( Vector( 0.0f, 0.70710659f, 0.70710659f ), oxConcatenatedParentChildSpace );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Quaternion, addVectorScaled )
{
   Quaternion quat;
   quat.setIdentity();

   Vector angularVelocity( 1.0f, 2.0f, 3.0f );
   FastFloat time = FastFloat::fromFloat( 5.0f );

   Quaternion a, b;
   a.setAddScaledVector( quat, angularVelocity, time );
   b = quat;
   b.addScaledVector( angularVelocity, time );

   COMPARE_QUAT( a, b );

   Quaternion expectedQuat;
   expectedQuat.set( 0.26574f, 0.53149f, 0.79724f, 0.10629f );
   COMPARE_QUAT( a, expectedQuat );
}

///////////////////////////////////////////////////////////////////////////////
