#include "core-TestFramework\TestFramework.h"
#include "core\Cylinder.h"
#include "core\Sphere.h"
#include "core\AxisAlignedBox.h"
#include "core\IntersectionResult.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Cylinder, boundingBox )
{
   Cylinder cylinder( Vector( -1.0f, 0.0f, 0.0f ), Vector( 2.0f, 0.0f, 0.0f ), 1.0f );

   AxisAlignedBox boundingBox; 
   cylinder.calculateBoundingBox( boundingBox );

   COMPARE_VEC( Vector( -1.0f, -1.0f, -1.0f ), boundingBox.min );
   COMPARE_VEC( Vector(  2.0f,  1.0f,  1.0f ), boundingBox.max );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Cylinder, isPointInside )
{
   Cylinder cylinder( Vector( -1.0f, 0.0f, 0.0f ), Vector( 2.0f, 0.0f, 0.0f ), 1.0f );

   // points inside the hull of the cylinder
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( -0.5f,  1.0f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 0.0f, -1.0f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 0.5f,  0.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 1.0f, -0.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 1.5f, 0.0f,  1.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 2.0f, 0.0f, -1.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( -1.0f, 0.0f,  0.5f ) ) );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.isInside( Vector( 0.0f, 0.0f, -0.5f ) ) );

   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( -0.5f,  1.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( 0.0f, -1.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( 1.5f, 0.0f,  1.5f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( 2.0f, 0.0f, -1.5f ) ) );

   // points  around the cylinder's core, that lie beyond the bases
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( -1.5f,  0.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( -1.01f, -0.5f, 0.0f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( 2.01f, 0.0f,  0.5f ) ) );
   CPPUNIT_ASSERT_EQUAL( false, cylinder.isInside( Vector( 2.5f, 0.0f, -0.5f ) ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Cylinder, cylinderSphereIntersection)
{
   Cylinder cylinder( Vector( -1.0f, 0.0f, 0.0f ), Vector( 2.0f, 0.0f, 0.0f ), 1.0f );
   Sphere sphere( Vector( 0.0f, 0.0f, 0.0f ), 1.0f );
   IntersectionResult result;

   // intersection in OY direction
   {
      sphere.origin.set( 0.0f, 0.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 1.0f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 1.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // intersection in OZ direction
   {
      sphere.origin.set( 0.0f, 0.0f, 0.5f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, 1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 0.0f, 1.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, 1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 0.0f, 1.5f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, 1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // intersection in -OY direction
   {
      sphere.origin.set( 0.0f, -0.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, -1.0f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, -1.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OY, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // intersection in -OZ direction
   {
      sphere.origin.set( 0.0f, 0.0f, -0.5f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, -1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 0.0f, -1.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, -1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, result.m_penetrationDepth.getFloat(), 1e-3 );

      sphere.origin.set( 0.0f, 0.0f, -1.5f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.0f, -1.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // collision with the bottom base
   {
      // origin located directly on the core, but underneath the bottom base
      sphere.origin.set( -1.5f, 0.0f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OX, result.m_contactNormal );
      COMPARE_VEC( Vector( -1.0f, 0.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      // origin located right of the the core
      sphere.origin.set( -1.5f, 0.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_NEG_OX, result.m_contactNormal );
      COMPARE_VEC( Vector( -1.0f, 0.5f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // collision with the top base
   {
      // origin located directly on the core, but above the top base
      sphere.origin.set( 2.5f, 0.0f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OX, result.m_contactNormal );
      COMPARE_VEC( Vector( 2.0f, 0.0f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

      // origin located right of the the core
      sphere.origin.set( 2.5f, 0.5f, 0.0f );
      CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector_OX, result.m_contactNormal );
      COMPARE_VEC( Vector( 2.0f, 0.5f, 0.0f ), result.m_contactPoint );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );
   }

   // and a special case where the sphere's origin is located on the cylinder's core
   sphere.origin.set( 0.0f, 0.0f, 0.0f );
   CPPUNIT_ASSERT_EQUAL( true, cylinder.testIntersection( sphere, result ) );
   COMPARE_VEC( Vector_OX, result.m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////

