#include "core-TestFramework\TestFramework.h"
#include "core\Box.h"
#include "core\IntersectionResult.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Box, noCollision )
{
   Box b1, b2;
   Matrix transform;
   IntersectionResult contactPointsArr[8];

   // axis aligned boxes, apart from each other
   {
      b1.set( Vector( 1.0f, 1.0f, 1.0f ) );

      // boxes far apart
      transform.setTranslation( Vector( 4.0f, 0.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 4.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 0.0f, 4.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( -4.0f, 0.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, -4.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 0.0f, -4.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      // boxes very close, but still not touching
      transform.setTranslation( Vector( 1.1f, 0.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 1.1f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 0.0f, 1.1f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( -1.1f, 0.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, -1.1f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 0.0f, 0.0f, -1.1f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );
   }

   // unaligned boxes, apart from each other
   {
      Quaternion rot;
      rot.setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );

      transform.setRotation( rot ); transform.setPosition<3>( Vector( 1.0f, 2.0f, 3.0f ) ); b1.set( Vector( 1.0f, 1.0f, 1.0f ), transform );

      transform.setTranslation( Vector( 3.0f, 2.0f, 3.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 1.0f, 4.0f, 3.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 1.0f, 2.0f, 5.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( -1.0f, 2.0f, 3.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 1.0f, 0.0f, 3.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );

      transform.setTranslation( Vector( 1.0f, 2.0f, 1.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, b1.testIntersection( b2, contactPointsArr, 8 ) );
   }
}

TEST( Box, vertexCollision )
{
   Box b1, b2;
   Matrix transform;
   IntersectionResult contactPointsArr[8];

   // axis aligned boxes, colliding
   {
      b1.set( Vector( 1.0f, 1.0f, 1.0f ) );

      // face-face collision

      // NOTE: when two bodies collide, two sets of contact points should be generated, that would affect each body individually.
      // But the fact is that we can do only with one set of contact points just for one body, 'cause one for the other one
      // is just a reflection of the first set.
      // So the convention is that if we call bodyA.testIntersection( bodyB ), the returned contact points
      // will refer to bodyA. If it's bodyB we want to move in response to the contact point, we just need
      // to calculate a mirror reflection of it.

      // box slides along the x axis
      transform.setTranslation( Vector( -0.5f, 0.75f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( -0.5f, 0.5f,  0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector(  0.0f, 0.5f,  0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector(  0.0f, 0.5f, -0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f, 0.5f, -0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.0f, 0.75f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( -0.5f, 0.5f, -0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f, 0.5f,  0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f, 0.5f,  0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f, 0.5f, -0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.5f, 0.75f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( 0.0f, 0.5f, -0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, 0.5f, -0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector( 0.0f, 0.5f,  0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, 0.5f,  0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OY, contactPointsArr[3].m_contactNormal );

      // now it slides along the y axis
      transform.setTranslation( Vector( 0.0f, -0.5f, 0.75f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector(  0.5f, -0.5f, 0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f,  0.0f, 0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f,  0.0f, 0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f, -0.5f, 0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.0f, 0.0f, 0.75f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( -0.5f, -0.5f, 0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f,  0.5f, 0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f, -0.5f, 0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f,  0.5f, 0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.0f, 0.5f, 0.75f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( -0.5f, 0.0f, 0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( -0.5f, 0.5f, 0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f, 0.0f, 0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector(  0.5f, 0.5f, 0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OZ, contactPointsArr[3].m_contactNormal );

      // and finally - along the z axis
      transform.setTranslation( Vector( 0.75f, 0.0f, -0.5f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( 0.5f,  0.5f,  0.0f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f,  0.5f, -0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, -0.5f, -0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, -0.5f,  0.0f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.75f, 0.0f, 0.0f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( 0.5f, -0.5f, -0.5f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f,  0.5f,  0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, -0.5f,  0.5f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f,  0.5f, -0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[3].m_contactNormal );

      transform.setTranslation( Vector( 0.75f, 0.0f, 0.5f ) ); b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );
      CPPUNIT_ASSERT_EQUAL( (uint)4, b1.testIntersection( b2, contactPointsArr, 8 ) );
      COMPARE_VEC( Vector( 0.5f, -0.5f, 0.0f ), contactPointsArr[0].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f, -0.5f, 0.5f ), contactPointsArr[1].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[1].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f,  0.5f, 0.0f ), contactPointsArr[2].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[2].m_contactNormal );
      COMPARE_VEC( Vector( 0.5f,  0.5f, 0.5f ), contactPointsArr[3].m_contactPoint );
      COMPARE_VEC( Vector_NEG_OX, contactPointsArr[3].m_contactNormal );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Box, edgeCollision )
{
   Box b1, b2;
   Matrix transform;
   IntersectionResult contactPointsArr[8];

   Quaternion rot;
   rot.setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
   transform.setRotation( rot );
   transform.setPosition<3>( Vector( 0.75f, 0.0f, 0.0f ) );

   b1.set( Vector( 1.0f, 1.0f, 0.2f ) );
   b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );

   CPPUNIT_ASSERT_EQUAL( ( uint )2, b1.testIntersection( b2, contactPointsArr, 8 ) );
   COMPARE_VEC( Vector( 0.5f, 0.0f, -0.1f ), contactPointsArr[0].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
   COMPARE_VEC( Vector( 0.5f, 0.0f,  0.1f ), contactPointsArr[1].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[1].m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Box, faceCollision )
{
   Box b1, b2;
   Matrix transform;
   IntersectionResult contactPointsArr[8];

   transform.setTranslation( Vector( 0.75f, 0.0f, 0.0f ) );

   b1.set( Vector( 1.0f, 5.0f, 5.0f ) );
   b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 4, b1.testIntersection( b2, contactPointsArr, 8 ) );
   COMPARE_VEC( Vector( 0.5f, -0.5f, -0.5f ), contactPointsArr[0].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[0].m_contactNormal );
   COMPARE_VEC( Vector( 0.5f,  0.5f,  0.5f ), contactPointsArr[1].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[1].m_contactNormal );
   COMPARE_VEC( Vector( 0.5f,  0.5f, -0.5f ), contactPointsArr[2].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[2].m_contactNormal );
   COMPARE_VEC( Vector( 0.5f, -0.5f,  0.5f ), contactPointsArr[3].m_contactPoint );
   COMPARE_VEC( Vector_NEG_OX, contactPointsArr[3].m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Box, offsetCollision )
{
   Box b1, b2;
   Matrix transform;
   IntersectionResult contactPointsArr[8];

   transform.setAxisAnglePos( Vector_OX, FastFloat::fromFloat( DEG2RAD( 45.0f ) ), Vector( 0.0f, 0.0f, 4.0f ) );
   b1.set( Vector( 1.0f, 1.0f, 1.0f ), transform );

   transform.setTranslation( Vector( 0.0f, 0.0f, 3.0f ) );
   b2.set( Vector( 1.0f, 1.0f, 1.0f ), transform );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, b1.testIntersection( b2, contactPointsArr, 8 ) );
   COMPARE_VEC( Vector( -0.5f, 0.0f, 3.2929f ), contactPointsArr[0].m_contactPoint );
   COMPARE_VEC( Vector_OZ, contactPointsArr[0].m_contactNormal );
   COMPARE_VEC( Vector( 0.5f, 0.0f, 3.2929f ), contactPointsArr[1].m_contactPoint );
   COMPARE_VEC( Vector_OZ, contactPointsArr[1].m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////
