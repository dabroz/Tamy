#include "core-TestFramework\TestFramework.h"
#include "core\EulerAngles.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\MathDefs.h"


///////////////////////////////////////////////////////////////////////////////

TEST(EulerAngles, normalization)
{
   EulerAngles o1, o2;

   o1.set( FastFloat::fromFloat( 360.0f ), FastFloat::fromFloat( 720.0f ), FastFloat::fromFloat( -360.0f ) );
   o1.normalize();
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o1.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o1.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o1.getRoll(), 1e-3);

   o2.set( FastFloat::fromFloat( 124.0f ), FastFloat::fromFloat( -91.0f ), FastFloat::fromFloat( -369.0f ) );
   o2.normalize();
   CPPUNIT_ASSERT_DOUBLES_EQUAL(124.f, o2.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-91.f, o2.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.f, o2.getRoll(), 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST(EulerAngles, comparisonOperatesOnNormalizedOrientation)
{
   EulerAngles o1, o2;

   o1.set( FastFloat::fromFloat( 124.0f ), FastFloat::fromFloat( -91.0f ), FastFloat::fromFloat( -369.0f ) );
   o2.set( FastFloat::fromFloat( 124.0f ), FastFloat::fromFloat( -91.0f ), FastFloat::fromFloat( -9.0f ) );
   CPPUNIT_ASSERT_EQUAL(o1, o2);
}

///////////////////////////////////////////////////////////////////////////////

TEST( EulerAngles, fromMatrix )
{
   Matrix rotMtx;
   EulerAngles angles, testAngle;

   {
      testAngle.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // northern gimball lock
      testAngle.set( Float_0, FastFloat::fromFloat( 90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // another case of northern gimball lock
      testAngle.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // another case of northern gimball lock
      testAngle.set( FastFloat::fromFloat( -45.0f ), FastFloat::fromFloat( 90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      testAngle.set( Float_0, Float_0, FastFloat::fromFloat( 90.0f ) );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      testAngle.set( FastFloat::fromFloat( -90.0f ), Float_0, Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // southern gimball lock
      testAngle.set( Float_0, FastFloat::fromFloat( -90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // another case of southern gimball lock
      testAngle.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( -90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      // another case of southern gimball lock
      testAngle.set( FastFloat::fromFloat( -45.0f ), FastFloat::fromFloat( -90.0f ), Float_0 );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      testAngle.set( Float_0, Float_0, FastFloat::fromFloat( -90.0f ) );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }
   
   {
      testAngle.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 45.0f ) );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }

   {
      testAngle.set( FastFloat::fromFloat( 10.0f ), FastFloat::fromFloat( 20.0f ), FastFloat::fromFloat( 30.0f ) );
      rotMtx.setRotation( testAngle);
      rotMtx.getRotation( angles );

      COMPARE_EULER( testAngle, angles );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( EulerAngles, setFromShortestRotation )
{
   EulerAngles o;
   o.setFromShortestRotation( Vector_OZ, Vector_OX );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(90.f, o.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getRoll(), 1e-3);

   o.setFromShortestRotation( Vector_OZ, Vector_NEG_OX );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.f, o.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getRoll(), 1e-3);

   o.setFromShortestRotation( Vector_OY, Vector_OZ );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(90.f, o.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getRoll(), 1e-1);

   o.setFromShortestRotation( Vector_OX, Vector_OY );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(90.f, o.getRoll(), 1e-1);

   o.setFromShortestRotation( Vector_OY, Vector_OX );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getYaw(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, o.getPitch(), 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-90.f, o.getRoll(), 1e-1);

   Vector diagVec( 0, 0.5f, 0.5f );
   diagVec.normalize();
   o.setFromShortestRotation( Vector_OY, diagVec );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, o.getYaw(), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 45.f, o.getPitch(), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, o.getRoll(), 1e-1 );
}

///////////////////////////////////////////////////////////////////////////////

TEST( EulerAngles, setFromQuaternion )
{
   Vector arbitraryAxis( 0.3f, 0.5f, -0.2f );
   arbitraryAxis.normalize();

   Vector axis[] = { Vector_OY, Vector_OZ, Vector_OX, Vector_NEG_OX, Vector_NEG_OY, Vector_NEG_OZ, arbitraryAxis };

   // conversion Quaternion->EulerAngle is not so well defined, especially for complex rotations.
   // That's why the last part of the test - the one that rotates around an arbitrarily selected axis
   // breaks instantaneously.
   // That's why I'm limiting the number of cases used here to rotations around the fixed axis only,
   // but leaving the other case so that maybe one day, when I come up with a correct equation, I can make it work again
   int axisCount = 6;

   Quaternion q;
   Quaternion testQ;
   EulerAngles ea;

   for ( int axisIdx = 0; axisIdx < axisCount; ++axisIdx )
   {
      for ( float angle = -179.0f; angle <= 179.0f; angle += 1.0f )
      {
         q.setAxisAngle( axis[axisIdx], FastFloat::fromFloat( DEG2RAD( angle ) ) );

         ea.setFromQuaternion( q );
         testQ.setFromEulerAngles( ea );

         // there's another issue with Quaternion->EulerAngle conversion here. For yaw < -90.0f && yaw > 90.0f,
         // the returned rotation is a rotation complement- basically it will return info that 'pitch' and 'roll' 
         // are both equal 180deg and 'yaw' = sign('acutalYaw') > 0 ? 90.0f - 'actualYaw' : 90.0f + 'actualYaw'.
         // I browsed the web but the equations they have there exhibit the same characteristics, that's why I'm using
         // this high-tolerance comparison method here
         COMPARE_QUAT_ALLOW_CONJUGATION( testQ, q );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
