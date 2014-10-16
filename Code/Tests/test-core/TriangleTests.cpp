#include "core-TestFramework\TestFramework.h"
#include "core\Triangle.h"
#include "core\Vector.h"
#include "core\Sphere.h"
#include "core\Box.h"
#include "core\IntersectionResult.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Triangle, collisionWithSphere )
{
   Triangle triangle( Vector( -1, -1, 0 ), Vector( 1, 1, 0 ), Vector( 1, -1, 0 ) );
   IntersectionResult result;


   // test the triangle normal while we're at it
   Vector triangleNormal;
   triangle.getTriangleNormal( triangleNormal );
   COMPARE_VEC( Vector_NEG_OZ, triangleNormal );

   Sphere sphere;
   sphere.radius = Float_1;

   {
      // sphere in front of the triangle, barely touching its face
      sphere.origin.set( 0.0f, -0.5f, -1.01f );
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );

      // sphere in front of the triangle, embedded in its face - now there's a collision
      sphere.origin.set( 0.0f, -0.5f, -0.9f );
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 0.0f, -0.5f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1f, result.m_penetrationDepth.getFloat(), 1e-3f );

      // it doesn't work from the other direction - triangle normal doesn't allow it
      sphere.origin.set( 0.0f, -0.5f, 0.9f );
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );
   }
   

   // move the sphere to each side of the triangle, testing that it DOESN'T intersect it
   {
      sphere.origin.set( -1.0f, 1.0f, 0.0f ); // above the triangle
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );

      sphere.origin.set( 0.0f, -2.01f, 0.0f ); // below the triangle
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );

      sphere.origin.set( 2.01f, 0.0f, 0.0f ); // right of the triangle
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );
   }


   // test a collision where the sphere touches the edge
   {
      sphere.origin.set( 0.0f, 0.0f, -0.5f ); // above the triangle
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 0.0f, 0.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat( ), 1e-3f );

      sphere.origin.set( 0.0f, -1.3f, -0.5f ); // below the triangle
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector( 0.0f, -0.5145f, -0.8575f ), result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.4169f, result.m_penetrationDepth.getFloat( ), 1e-3f );

      sphere.origin.set( 1.3f, 0.0f, -0.5f ); // right of the triangle
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector( 0.5145f, 0.0f, -0.8575f ), result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.4169f, result.m_penetrationDepth.getFloat( ), 1e-3f );
   }

   // move the sphere to each vertex of the triangle, testing that it DOESN'T intersect it
   {
      sphere.origin.set( -2.0f, -2.0f, 0.0f ); // vtx 0
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );

      sphere.origin.set( 2.0f, 2.0f, 0.0f ); // vtx 1
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );

      sphere.origin.set( 2.0f, -2.0f, 0.0f ); // vtx 2
      CPPUNIT_ASSERT_EQUAL( false, triangle.testIntersection( sphere, result ) );
   }
   
   // test a collision where the sphere touches the vertex
   {
      sphere.origin.set( -1.3f, -1.3f, -0.5f ); // vtx 0
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( -1.0f, -1.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector( -0.4575f, -0.4575f, -0.7625f ), result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3443f, result.m_penetrationDepth.getFloat( ), 1e-3f );

      sphere.origin.set( 1.3f, 1.3f, -0.5f ); // vtx 1
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 1.0f, 1.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector( 0.4575f, 0.4575f, -0.7625f ), result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3443f, result.m_penetrationDepth.getFloat( ), 1e-3f );

      sphere.origin.set( 1.3f, -1.3f, -0.5f ); // vtx 2
      CPPUNIT_ASSERT_EQUAL( true, triangle.testIntersection( sphere, result ) );
      COMPARE_VEC( Vector( 1.0f, -1.0f, 0.0f ), result.m_contactPoint );
      COMPARE_VEC( Vector( 0.4575f, -0.4575f, -0.7624f ), result.m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3443f, result.m_penetrationDepth.getFloat( ), 1e-3f );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Triangle, collisionWithBox )
{
   // <physics.todo> uncomment me when you start working on the physics again
   /*
   Triangle triangle( Vector( -1, -1, 0 ), Vector( 1, 1, 0 ), Vector( 1, -1, 0 ) );

   Box templateBox, testBox;
   templateBox.set( Vector( 2, 2, 2 ), Matrix::IDENTITY );

   Matrix testMtx;
   IntersectionResult contactPointsArr[8];

   // simple ones first
   {
      // no collision at all
      testMtx.setTranslation( Vector( 0, 0, -2 ) );
      templateBox.transform( testMtx, testBox );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, testBox.testIntersection( triangle, contactPointsArr, 8 ) );

      // face slightly embedded 
      testMtx.setTranslation( Vector( 0, 0, -0.9f ) );
      templateBox.transform( testMtx, testBox );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, testBox.testIntersection( triangle, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( 0.0f, 0.0f, 0.0f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1f, contactPointsArr[0].m_penetrationDepth.getFloat( ), 1e-3f );
   }
   */
}

///////////////////////////////////////////////////////////////////////////////
