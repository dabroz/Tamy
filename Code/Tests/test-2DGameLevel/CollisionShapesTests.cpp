#include "core-TestFramework\TestFramework.h"
#include "core\Math.h"
#include "ext-2DGameLevel\GL2DCircleShape.h"
#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"


///////////////////////////////////////////////////////////////////////////////

TEST( GL2DShape, circleVsLevelGeometry )
{
   GL2DCircleShape dynamicShape( 0.5f );
   GL2DLineSegmentsShape levelGeometry;

   // add a box 1 unit wide and 1 unit broad, at point ( 10, 0 )
   Transform transform;
   transform.m_translation.set( 10, 0, 0 );
   transform.m_rotation.setIdentity();
   levelGeometry.addRect( transform, Vector( 0.5f, 0.5f, 0 ) );
   levelGeometry.build();

   IntersectionResult result[MAX_INTERSECTION_RESULTS];

   // no collision
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, dynamicShape.detectNarrowphaseCollision( &levelGeometry, Matrix::IDENTITY, Matrix::IDENTITY, result ) );

   Matrix circlePos;
   // frontal collision
   circlePos.setTranslation( Vector( 9.3f, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, dynamicShape.detectNarrowphaseCollision( &levelGeometry, circlePos, Matrix::IDENTITY, result ) );
   COMPARE_VEC( Vector_NEG_OX, result[0].m_contactNormal );
   COMPARE_VEC( Vector( 9.5f, 0, 0 ), result[0].m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3f, result[0].m_penetrationDepth.getFloat( ), 1e-3 );

   // collision with a corner
   circlePos.setTranslation( Vector( 9.3f, -0.5f, 0 ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, dynamicShape.detectNarrowphaseCollision( &levelGeometry, circlePos, Matrix::IDENTITY, result ) );
   COMPARE_VEC( Vector_NEG_OX, result[0].m_contactNormal );
   COMPARE_VEC( Vector( 9.5f, -0.5f, 0 ), result[0].m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3f, result[0].m_penetrationDepth.getFloat( ), 1e-3 );

   circlePos.setTranslation( Vector( 9.5f, -0.7f, 0 ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, dynamicShape.detectNarrowphaseCollision( &levelGeometry, circlePos, Matrix::IDENTITY, result ) );
   COMPARE_VEC( Vector_NEG_OY, result[0].m_contactNormal );
   COMPARE_VEC( Vector( 9.5f, -0.5f, 0 ), result[0].m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.3f, result[0].m_penetrationDepth.getFloat( ), 1e-3 );

   circlePos.setTranslation( Vector( 9.25f, -0.75f, 0 ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, dynamicShape.detectNarrowphaseCollision( &levelGeometry, circlePos, Matrix::IDENTITY, result ) );
   COMPARE_VEC( Vector( -0.7071f, -0.7071f, 0 ), result[0].m_contactNormal );
   COMPARE_VEC( Vector( 9.5f, -0.5f, 0 ), result[0].m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1464f, result[0].m_penetrationDepth.getFloat( ), 1e-3 );
}

///////////////////////////////////////////////////////////////////////////////

TEST( GL2DShape, circleVsCircle )
{
   GL2DCircleShape agentA( 1 );
   GL2DCircleShape agentB( 1 );

   IntersectionResult result[MAX_INTERSECTION_RESULTS];

   Matrix agentAPos, agentBPos;

   // no collision
   agentAPos.setTranslation( Vector( 0, 0, 0 ) );
   agentBPos.setTranslation( Vector( 5, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( (uint)0, agentA.detectNarrowphaseCollision( &agentB, agentAPos, agentBPos, result ) );

   // collision
   agentAPos.setTranslation( Vector( 3.5f, 0, 0 ) );
   agentBPos.setTranslation( Vector( 5, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( ( uint )1, agentA.detectNarrowphaseCollision( &agentB, agentAPos, agentBPos, result ) );
   COMPARE_VEC( Vector_NEG_OX, result[0].m_contactNormal );
   COMPARE_VEC( Vector( 4, 0, 0 ), result[0].m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result[0].m_penetrationDepth.getFloat( ), 1e-3 );
}

///////////////////////////////////////////////////////////////////////////////
