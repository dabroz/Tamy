#include "core-TestFramework\TestFramework.h"
#include "core\PolygonMesh2D.h"
#include "core\MeshAdjacencyMap.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TEST( PolygonMesh2D, addingTrianglesToPolygonMesh )
{
   Triangle triangle;
   PolygonMergeRules mergeRules;

   PolygonMesh2D polygon( Vector_OZ );

   // merge the first triangle - since its plane matches that of the triangle ( to the normal direction that is )
   // the triangle will be added as is
   triangle.setFromVertices( Vector( 0, 0, 0 ), Vector( 1, 1, 0 ), Vector( 1, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, polygon.getTriangleCount( ) );
   polygon.getTriangle( 0, triangle );
   COMPARE_VEC( Vector( 0, 0, 0 ), triangle.vertex( 0 ) );
   COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
   COMPARE_VEC( Vector( 1, 0, 0 ), triangle.vertex( 2 ) );


   // add another triangle - this one's normal still matches that of the plane, but it's located
   // above the polygon plane, so its vertices will be projected down to that plane
   triangle.setFromVertices( Vector( 0, 0, 5 ), Vector( 0, 1, 5 ), Vector( 1, 1, 5 ) );
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, polygon.getTriangleCount( ) );
   polygon.getTriangle( 1, triangle );
   COMPARE_VEC( Vector( 0, 0, 0 ), triangle.vertex( 0 ) );
   COMPARE_VEC( Vector( 0, 1, 0 ), triangle.vertex( 1 ) );
   COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 2 ) );

   // add another triangle - the plane of this one exceeds the acceptable difference of angles, so it will be rejected
   mergeRules.m_angularDifference.setFromFloat( DEG2RAD( 10.0f ) );
   triangle.setFromVertices( Vector( 0, 1, 0 ), Vector( 0, 1.5f, 1 ), Vector( 1, 1.5f, 1 ) );
   CPPUNIT_ASSERT_EQUAL( false, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, polygon.getTriangleCount( ) );

   // let's relax the rules a bit and add a triangle that becomes degenerate after the projection - such a triangle won't be added
   mergeRules.m_angularDifference.setFromFloat( DEG2RAD( 180.0f ) );
   triangle.setFromVertices( Vector( 0, 1, 0 ), Vector( 0, 1, 1 ), Vector( 1, 1, 1 ) );
   CPPUNIT_ASSERT_EQUAL( false, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, polygon.getTriangleCount( ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( PolygonMesh2D, newTriangleSplitsExistingGeometry )
{
   Triangle triangle;
   PolygonMergeRules mergeRules;

   PolygonMesh2D polygon( Vector_OZ );

   // merge the first triangle - since its plane matches that of the triangle ( to the normal direction that is )
   // the triangle will be added as is
   triangle.setFromVertices( Vector( 0, 0, 0 ), Vector( 1, 1, 0 ), Vector( 1, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, polygon.getTriangleCount() );
   polygon.getTriangle( 0, triangle );
   COMPARE_VEC( Vector( 0, 0, 0 ), triangle.vertex( 0 ) );
   COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
   COMPARE_VEC( Vector( 1, 0, 0 ), triangle.vertex( 2 ) );


   // add another triangle - one that will split the previous triangle
   triangle.setFromVertices( Vector( 0.2f, 0, 0 ), Vector( 0.2f, -1, 0 ), Vector( 0.5f, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 4, polygon.getTriangleCount() );

   // see how the existing triangles changed
   {
      // tri 0
      polygon.getTriangle( 0, triangle );
      COMPARE_VEC( Vector( 0, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 2 ) );

      // tri 1
      polygon.getTriangle( 1, triangle );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 2 ) );

      // tri 2
      polygon.getTriangle( 2, triangle );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 1, 0, 0 ), triangle.vertex( 2 ) );

      // tri 3 - the newly added one
      polygon.getTriangle( 3, triangle );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 0.2f, -1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 2 ) );
   }

}

///////////////////////////////////////////////////////////////////////////////

TEST( PolygonMesh2D, existingGeometrySplitsNewTriangle )
{
   Triangle triangle;
   PolygonMergeRules mergeRules;

   PolygonMesh2D polygon( Vector_OZ );

   // merge the first triangle - since its plane matches that of the triangle ( to the normal direction that is )
   // the triangle will be added as is
   triangle.setFromVertices( Vector( 0.2f, 0, 0 ), Vector( 0.2f, -1, 0 ), Vector( 0.5f, 0, 0 ) ); 
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, polygon.getTriangleCount() );
   polygon.getTriangle( 0, triangle );
   COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 0 ) );
   COMPARE_VEC( Vector( 0.2f, -1, 0 ), triangle.vertex( 1 ) );
   COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 2 ) );


   // add another triangle - one that will split the previous triangle
   triangle.setFromVertices( Vector( 0, 0, 0 ), Vector( 1, 1, 0 ), Vector( 1, 0, 0 ) );
   CPPUNIT_ASSERT_EQUAL( true, polygon.addTriangle( triangle, mergeRules ) );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 4, polygon.getTriangleCount() );

   // see how the existing triangles changed
   {
      // tri 0
      polygon.getTriangle( 0, triangle );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 0.2f, -1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 2 ) );

      // tri 1
      polygon.getTriangle( 1, triangle );
      COMPARE_VEC( Vector( 0, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 2 ) );

      // tri 2
      polygon.getTriangle( 2, triangle );
      COMPARE_VEC( Vector( 0.2f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 2 ) );

      // tri 3 - the newly added one
      polygon.getTriangle( 3, triangle );
      COMPARE_VEC( Vector( 0.5f, 0, 0 ), triangle.vertex( 0 ) );
      COMPARE_VEC( Vector( 1, 1, 0 ), triangle.vertex( 1 ) );
      COMPARE_VEC( Vector( 1, 0, 0 ), triangle.vertex( 2 ) );
   }

}

///////////////////////////////////////////////////////////////////////////////

TEST( PolygonMesh2D, generatingAdjacencyMap )
{
   // build a test polygon mesh with 2 islands of triangles
   PolygonMesh2D polygon( Vector_OZ );
   {
      Triangle triangle;
      PolygonMergeRules mergeRules;

      // the first island - first triangle has 2 adjacent triangles, but those don't lie
      // next to each other
      triangle.setFromVertices( Vector( 0, 0, 0 ), Vector( 1, 1, 0 ), Vector( 1, 0, 0 ) ); // 2 neighbors
      polygon.addTriangle( triangle, mergeRules );

      triangle.setFromVertices( Vector( 0, 0, 0 ), Vector( 0, 1, 0 ), Vector( 1, 1, 0 ) ); // 1 neighbor
      polygon.addTriangle( triangle, mergeRules );

      triangle.setFromVertices( Vector( 0, -1, 0 ), Vector( 0, 0, 0 ), Vector( 1, 0, 0 ) ); // 1 neighbor
      polygon.addTriangle( triangle, mergeRules );

      // the second island - 4 triangles, each of which has 2 neighbors
      triangle.setFromVertices( Vector( 5, 5, 0 ), Vector( 5, 6, 0 ), Vector( 6, 5, 0 ) ); // neighbors: ( 4, 6 )
      polygon.addTriangle( triangle, mergeRules );

      triangle.setFromVertices( Vector( 4, 5, 0 ), Vector( 5, 6, 0 ), Vector( 5, 5, 0 ) ); // neighbors: ( 3, 5 )
      polygon.addTriangle( triangle, mergeRules );

      triangle.setFromVertices( Vector( 4, 5, 0 ), Vector( 5, 5, 0 ), Vector( 5, 4, 0 ) ); // neighbors: ( 4, 6 )
      polygon.addTriangle( triangle, mergeRules );

      triangle.setFromVertices( Vector( 5, 4, 0 ), Vector( 5, 5, 0 ), Vector( 6, 5, 0 ) ); // neighbors: ( 3, 5 )
      polygon.addTriangle( triangle, mergeRules );
   }

   // basic verification that all triangles were successfully added
   CPPUNIT_ASSERT_EQUAL( ( uint ) 7, polygon.getTriangleCount( ) );

   MeshAdjacencyMap adjacencyMap;
   polygon.generateAdjacencyMap( adjacencyMap );
   uint indicesArr[3];

   // let's check which triangles share which edges
   {
      // triangle 0
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 0, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 1, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 2, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, indicesArr[1] );

      // triangle 1
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 3, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 4, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 5, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, indicesArr[1] );

      // triangle 2
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 6, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 7, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 8, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, indicesArr[0] );

      // triangle 4
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 9, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 3, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 4, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getTrianglesForEdge( 10, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 3, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getTrianglesForEdge( 11, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 3, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 6, indicesArr[1] );
   }


   // let's check which triangles connect to each
   {
      CPPUNIT_ASSERT_EQUAL( ( uint ) 7, adjacencyMap.getTrianglesCount() );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getAdjacentTriangles( 0, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getAdjacentTriangles( 1, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 1, adjacencyMap.getAdjacentTriangles( 2, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 0, indicesArr[0] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getAdjacentTriangles( 3, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 4, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 6, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getAdjacentTriangles( 4, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 3, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 5, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getAdjacentTriangles( 5, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 4, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 6, indicesArr[1] );

      CPPUNIT_ASSERT_EQUAL( ( uint ) 2, adjacencyMap.getAdjacentTriangles( 6, indicesArr ) );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 3, indicesArr[0] );
      CPPUNIT_ASSERT_EQUAL( ( uint ) 5, indicesArr[1] );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( MeshAdjacencyMap, collectingBorderEdges )
{
   // we want to test edge merging as well - we want to have two adjacent quads, so that when we extract the edge,
   // it will get merged
   PolygonMesh2D polygon( Vector_OZ );
   {
      Triangle triangle;
      PolygonMergeRules mergeRules;

      Vector vertices[] = { 
         Vector( 0, 1, 0 ), Vector( 1, 1, 0 ), Vector( 2, 1, 0 ),  
         Vector( 0, 0, 0 ), Vector( 1, 0, 0 ), Vector( 2, 0, 0 ) 
      };
      word indices[] = {
      0, 1, 3, 
      3, 1, 4,
      1, 2, 4,
      4, 2, 5 };
      polygon.addTriangleSoup( vertices, indices, 4, mergeRules );
   }

   // basic verification that all triangles were successfully added
   CPPUNIT_ASSERT_EQUAL( ( uint ) 4, polygon.getTriangleCount( ) );

   MeshAdjacencyMap adjacencyMap;
   polygon.generateAdjacencyMap( adjacencyMap );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 4, adjacencyMap.getTrianglesCount( ) );

   List< Vector > boundaryEdgeSegments;
   polygon.getBoundaryEdge( adjacencyMap, boundaryEdgeSegments );

   Vector expectedVertices [] = {
      Vector( 0, 1, 0 ), Vector( 1, 1, 0 ),
      Vector( 0, 0, 0 ), Vector( 0, 1, 0 ),
      Vector( 1, 0, 0 ), Vector( 0, 0, 0 ),
      Vector( 1, 1, 0 ), Vector( 2, 1, 0 ),
      Vector( 2, 1, 0 ), Vector( 2, 0, 0 ),
      Vector( 2, 0, 0 ), Vector( 1, 0, 0 ),
   };

   uint idx = 0;
   for ( List< Vector >::iterator it = boundaryEdgeSegments.begin(); !it.isEnd(); ++it, ++idx )
   {
      const Vector& vtx = *it;
      COMPARE_VEC( expectedVertices[idx], vtx );
   }
}

///////////////////////////////////////////////////////////////////////////////
