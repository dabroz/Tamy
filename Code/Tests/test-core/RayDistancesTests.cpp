#include "core-TestFramework\TestFramework.h"
#include "core\RaycastResult.h"
#include "core\AxisAlignedBox.h"
#include "core\Ray.h"
#include "core\Sphere.h"
#include "core\Cylinder.h"
#include "core\Box.h"
#include "core\Matrix.h"
#include "core-TestFramework\MatrixWriter.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Ray, Sphere )
{
   Ray ray(Vector(0, 0, 0), Vector(1, 0, 0));

   Sphere bs(Vector(0, 0, 0), 1);
   RaycastResult result;
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( bs, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

   bs.origin = Vector(10, 0, 0);
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( bs, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

   bs.origin = Vector(-10, 0, 0);
   CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( bs, result ) );

   bs.origin = Vector(10, 0.5f, 0);
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( bs, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(9.134f, result.m_dist.getFloat(), 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Ray, AxisAlignedBox )
{
   AxisAlignedBox aabb(Vector(-1, -1, -1), Vector(1, 1, 1));

   Ray ray1(Vector(0, 0, 0), Vector(1, 0, 0));
   RaycastResult result;
   CPPUNIT_ASSERT_EQUAL( true, ray1.rayCast( aabb, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

   Ray ray2(Vector(0, 0, 0), Vector(-1, 0, 0));
   CPPUNIT_ASSERT_EQUAL( true, ray2.rayCast( aabb, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

   Ray ray3(Vector(-10, 0, 0), Vector(1, 0, 0));
   CPPUNIT_ASSERT_EQUAL( true, ray3.rayCast( aabb, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

   Ray ray4(Vector(-10, 0, 0), Vector(-1, 0, 0));
   CPPUNIT_ASSERT_EQUAL( false, ray4.rayCast( aabb, result ) );

   Ray ray5(Vector(0, 10, 0), Vector(0, -1, 0));
   CPPUNIT_ASSERT_EQUAL( true, ray5.rayCast( aabb, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

   Ray ray6(Vector(10, 10, 10), Vector(-1, -1, -1));
   CPPUNIT_ASSERT_EQUAL( true, ray6.rayCast( aabb, result ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(15.588f, result.m_dist.getFloat(), 1e-3);

   Ray ray7(Vector(-10, 5, 0), Vector(1, 0, 0));
   CPPUNIT_ASSERT_EQUAL( false, ray7.rayCast( aabb, result ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Ray, Cylinder )
{
   Cylinder cylinder( Vector( 10.0f, 0.0, -0.5f ), Vector( 10.0f, 0.0, 0.5f ), 1.0f );

   // test to see if the body can be queried
   Ray ray;
   RaycastResult hitDesc;

   ray.direction = Vector_OY;
   CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( cylinder, hitDesc ) );

   ray.direction = Vector_OX;
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( cylinder, hitDesc ) );
   CPPUNIT_ASSERT_EQUAL( 9.0f, hitDesc.m_dist.getFloat() );
   COMPARE_VEC( Vector_NEG_OX, hitDesc.m_surfaceNormal );

   // move the ray down a bit and see if it bounces in a different direction
   ray.origin.set( 0.0f, 0.5f, 0.0f );
   ray.direction = Vector_OX;
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( cylinder, hitDesc ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 9.134f, hitDesc.m_dist.getFloat(), 1e-3f );
   COMPARE_VEC( Vector( -0.866025f, 0.5f, 0.0f ), hitDesc.m_surfaceNormal );

   // move the ray so that it bounces off the bottom base
   ray.origin.set( 0.5f, 0.0f, -10.5f );
   ray.direction.set( 0.707f, 0.0f, 0.707f ); ray.direction.normalize();
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( cylinder, hitDesc ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 14.142f, hitDesc.m_dist.getFloat(), 1e-3f );
   COMPARE_VEC( Vector_NEG_OZ, hitDesc.m_surfaceNormal );

   // move the ray so that it bounces off the top base
   ray.origin.set( 0.5f, 0.0f, 10.5f );
   ray.direction.set( 0.707f, 0.0f, -0.707f ); ray.direction.normalize();
   CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( cylinder, hitDesc ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 14.142f, hitDesc.m_dist.getFloat(), 1e-3f );
   COMPARE_VEC( Vector_OZ, hitDesc.m_surfaceNormal );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Ray, Box )
{
   Box box( Vector( -1.0f, -1.0f, -1.0f ), Vector( 1.0f, 1.0f, 1.0f ) );
   Ray ray;
   RaycastResult result;

   // when the box is not rotated, it's just another axis aligned box - so let's repeat 
   // AxisAlignerdBox's test suite for it
   {
      ray.setFromPointDirection(Vector(0, 0, 0), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( box, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(0, 0, 0), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( box, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(-10, 0, 0), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( box, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(-10, 0, 0), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( box, result ) );

      ray.setFromPointDirection(Vector(0, 10, 0), Vector(0, -1, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( box, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(10, 10, 10), Vector(-1, -1, -1));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( box, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(15.588f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(-10, 5, 0), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( box, result ) );
   }

   // now let's translate the box - previous test cases fail, but if we translate the ray as well,
   // they start working again
   {
      Matrix testTranslation;
      testTranslation.setTranslation( Vector( 10.0f, 20.0f, 30.0f ) );

      Box translatedBox;
      box.transform( testTranslation, translatedBox );

      ray.setFromPointDirection(Vector(0, 0, 0), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      ray.setFromPointDirection(Vector(0, 0, 0), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      ray.setFromPointDirection(Vector(-10, 0, 0), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      ray.setFromPointDirection(Vector(-10, 0, 0), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      ray.setFromPointDirection(Vector(0, 10, 0), Vector(0, -1, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      ray.setFromPointDirection(Vector(10, 10, 10), Vector(-1, -1, -1));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( translatedBox, result ) );

      // translated rays
      ray.setFromPointDirection(Vector(10.0f, 20.0f, 30.0f), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( translatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(10.0f, 20.0f, 30.0f), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( translatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(0.0f, 20.0f, 30.0f), Vector(1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( translatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(0.0f, 20.0f, 30.0f), Vector(-1, 0, 0));
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( box, result ) );

      ray.setFromPointDirection(Vector(10.0f, 30.0f, 30.0f), Vector(0, -1, 0));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( translatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector(20.0f, 30.0f, 40.0f), Vector(-1, -1, -1));
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( translatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(15.588f, result.m_dist.getFloat(), 1e-3);
   }

   // now let's try with a rotated box
   {
      Matrix testRotation;
      Quaternion rotQ;
      rotQ.setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      testRotation.setRotation( rotQ );

      Box rotatedBox;
      box.transform( testRotation, rotatedBox );

      // let's try hitting the previous corner positions first - that should fail
      ray.setFromPointDirection(Vector( -1.0f, -1.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( rotatedBox, result ) );
      ray.setFromPointDirection(Vector(  1.0f, -1.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( rotatedBox, result ) );
      ray.setFromPointDirection(Vector(  1.0f,  1.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( rotatedBox, result ) );
      ray.setFromPointDirection(Vector( -1.0f,  1.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( false, ray.rayCast( rotatedBox, result ) );

      // the center is still hittable though
      ray.setFromPointDirection(Vector( 0.0f, 0.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( rotatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      // and since the box is now rotated, we can hit points above the lines
      // that divided box's Z base in half
      ray.setFromPointDirection(Vector( 1.4f, 0.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( rotatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3); 

      ray.setFromPointDirection(Vector( -1.4f, 0.0f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( rotatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3);

      ray.setFromPointDirection(Vector( 0.0f, 1.4f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( rotatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3); 

      ray.setFromPointDirection(Vector( 0.0f, -1.4f, -10.0f ), Vector_OZ );
      CPPUNIT_ASSERT_EQUAL( true, ray.rayCast( rotatedBox, result ) );
      CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, result.m_dist.getFloat(), 1e-3); 
   }
}

///////////////////////////////////////////////////////////////////////////////
