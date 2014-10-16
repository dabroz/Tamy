#include "core\PolygonMesh2D.h"
#include "core\Triangle.h"
#include "core\MeshAdjacencyMap.h"
#include "core\MathDefs.h"
#include "core\ListUtils.h"
#include "core\VectorUtil.h"


///////////////////////////////////////////////////////////////////////////////

PolygonMergeRules::PolygonMergeRules( )
   : m_angularDifference( FastFloat::fromFloat( DEG2RAD( 180.0f ) ) )
   , m_vertexWeldDistance( Float_1e_3 )
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PolygonMesh2D::PolygonMesh2D( const Vector& normal )
   : m_normal( normal )
{
}

///////////////////////////////////////////////////////////////////////////////

void PolygonMesh2D::addTriangleSoup( const Vector* arrVertices, const word* arrIndices, const uint trianglesCount, const PolygonMergeRules& mergeRules )
{
   Triangle triangle;
   uint startIdx = 0;
   for ( uint i = 0; i < trianglesCount; ++i )
   {
      startIdx = i * 3;
      triangle.setFromVertices( arrVertices[arrIndices[startIdx]], arrVertices[arrIndices[startIdx + 1]], arrVertices[arrIndices[startIdx + 2]] );

      addTriangle( triangle, mergeRules );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool PolygonMesh2D::addTriangle( const Triangle& triangle, const PolygonMergeRules& mergeRules )
{
   // verify that the triangle plane's normal is oriented at a tolerable angle to the polygon's normal
   {
      Vector triangleNormal;
      triangle.getTriangleNormal( triangleNormal );

      FastFloat angleAcos = triangleNormal.dot( m_normal );
      angleAcos.acos();

      if ( angleAcos > mergeRules.m_angularDifference )
      {
         return false;      
      }
   }

   // project the vertices of the triangle to the plane of the polygon
   Vector projectedVertex[3];
   for ( byte i = 0; i < 3; ++i )
   {
      const Vector& vtx = triangle.vertex( i );
      const FastFloat dist = m_normal.dot( vtx );
      projectedVertex[i].setMulAdd( m_normal, -dist, vtx );
   }

   // verify that the triangle hasn't become degenerate ( that it's still got a non-zero field value )
   {
      Vector edge[3];
      edge[0].setSub( projectedVertex[1], projectedVertex[0] );
      edge[1].setSub( projectedVertex[2], projectedVertex[1] );
      edge[2].setSub( projectedVertex[0], projectedVertex[2] );
      const FastFloat edgeLength [] = { edge[0].length(), edge[1].length(), edge[2].length() };
      const FastFloat p = ( edgeLength[0] + edgeLength[1] + edgeLength[2] ) * Float_Inv2;
      FastFloat s;
      s.setSqrt( p * ( p - edgeLength[0] ) * ( p - edgeLength[1] ) * ( p - edgeLength[2] ) );

      if ( s <= Float_0 )
      {
         // the triangle became degenerate
         return false;
      }
   }

   // some of the new vertices might lie in the middle of existing edges - in that case
   // split up the triangle to insert the new vertex
   uint trianglesCount = m_indices.size() / 3;
   for ( uint triangleIdx = 0; triangleIdx < trianglesCount; ++triangleIdx )
   {
      for ( byte vtxIdx = 0; vtxIdx < 3; ++vtxIdx )
      {
         const Vector& newVtx = projectedVertex[vtxIdx];
         bool wasSplit = splitTriangleWithVertex( triangleIdx, newVtx, mergeRules.m_vertexWeldDistance );
         if ( wasSplit )
         {
            // a new triangle was appended to the end of the collection
            ++trianglesCount;
         }
      }
   }

   // add the new triangle
   const uint indicesCount = m_indices.size();
   m_indices.resize( indicesCount + 3, 0 );
   for ( byte i = 0; i < 3; ++i )
   {
      m_indices[indicesCount + i] = allocateVertex( projectedVertex[i], mergeRules.m_vertexWeldDistance );
   }

   // and finally - the new triangle might also be in need of splitting - one of the existing vertices may
   // be intersecting its edge, and we didn't test that yet - so do it now

   // start from the new triangle Idx
   uint oldTrianglesCount = trianglesCount;
   trianglesCount = m_indices.size() / 3;
   const uint verticesCount = m_vertices.size();
   for ( uint triangleIdx = oldTrianglesCount; triangleIdx < trianglesCount; ++triangleIdx )
   {
      for ( byte vtxIdx = 0; vtxIdx < verticesCount; ++vtxIdx )
      {
         const Vector& splitterVtx = m_vertices[vtxIdx];
         bool wasSplit = splitTriangleWithVertex( triangleIdx, splitterVtx, mergeRules.m_vertexWeldDistance );
         if ( wasSplit )
         {
            // a new triangle was appended to the end of the collection
            ++trianglesCount;
         }
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

uint PolygonMesh2D::getTriangleCount() const
{
   return m_indices.size() / 3;
}

///////////////////////////////////////////////////////////////////////////////

void PolygonMesh2D::getTriangle( uint triangleIdx, Triangle& outTriangle ) const
{
   uint startIdx = triangleIdx * 3;
   outTriangle.setFromVertices( m_vertices[m_indices[startIdx]], m_vertices[m_indices[startIdx + 1]], m_vertices[m_indices[startIdx + 2]] );
}

///////////////////////////////////////////////////////////////////////////////

struct AdjacencyMapInputFilter
{
   List< uint >      m_indices;
   bool              m_initialAddition;


   void reset()
   {
      m_indices.clear();
      m_initialAddition = true;
   }

   void add( const List< uint >& arrIndices )
   {
      if ( m_initialAddition )
      {
         m_indices = arrIndices;
         m_initialAddition = false;
      }
      else
      {
         // Cross check elements between the lists. Throw out those that are in one list, but not the other

         for ( List< uint >::const_iterator itA = arrIndices.begin(); !itA.isEnd(); ++itA )
         {
            List< uint >::iterator itB = ListUtils::find( m_indices, *itA );
            if ( itB.isEnd() )
            {
               itB.markForRemoval();
            }
         }

         for ( List< uint >::iterator itA = m_indices.begin(); !itA.isEnd(); ++itA )
         {
            List< uint >::const_iterator itB = ListUtils::find( arrIndices, *itA );
            if ( itB.isEnd( ) )
            {
               itA.markForRemoval();
            }
         }
      }
   }

   uint getFilteredIndices( uint* outArrIndices ) const
   {
      const uint count = m_indices.size();
      ASSERT( count <= 2 );

      uint idx = 0;
      for ( List< uint >::const_iterator itA = m_indices.begin( ); !itA.isEnd( ); ++itA, ++idx )
      {
         outArrIndices[idx] = *itA;
      }

      return count;
   }
};

void PolygonMesh2D::generateAdjacencyMap( MeshAdjacencyMap& outMap ) const
{
   const uint indicesCount = m_indices.size();
   const uint verticesCount = m_vertices.size();
   const uint trianglesCount = indicesCount / 3;
   outMap.initialize( verticesCount, indicesCount );

   // create a mapping between vertices and the triangles they belong to
   uint startIdx = 0;
   for ( uint triangleIdx = 0; triangleIdx < trianglesCount; ++triangleIdx )
   {
      startIdx = triangleIdx * 3;
      outMap.assignTriangleToVertex( m_indices[startIdx], triangleIdx );
      outMap.assignTriangleToVertex( m_indices[startIdx + 1], triangleIdx );
      outMap.assignTriangleToVertex( m_indices[startIdx + 2], triangleIdx );
   }

   // use the vertex-to-triangle mapping to build a mapping of edge-to-triangle
   uint triIndices[2];
   AdjacencyMapInputFilter filter;
   for ( uint triangleIdx = 0; triangleIdx < trianglesCount; ++triangleIdx )
   {
      startIdx = triangleIdx * 3;

      for ( byte i = 0; i < 3; ++i )
      {
         const uint startVtxIdx = m_indices[startIdx + i];
         const uint endVtxIdx = m_indices[startIdx + ( i + 1 ) % 3];

         filter.reset( );
         const List< uint >& startVtxTris = outMap.getTrianglesOfVertex( startVtxIdx );
         filter.add( startVtxTris );
         const List< uint >& endVtxTris = outMap.getTrianglesOfVertex( endVtxIdx );
         filter.add( endVtxTris );

         const uint filteredTrisCount = filter.getFilteredIndices( triIndices );
         const uint edgeIdx = startIdx + i;
         for ( byte j = 0; j < filteredTrisCount; ++j )
         {
            outMap.assignTriangleToEdge( edgeIdx, triIndices[j] );
         }
      }
   }

   outMap.buildGraph();
}

///////////////////////////////////////////////////////////////////////////////

void PolygonMesh2D::getBoundaryEdge( const MeshAdjacencyMap& adjacencyMap, List< Vector >& outBoundaryEdgeSegments ) const
{
   const uint indicesCount = m_indices.size( );
   const uint trianglesCount = indicesCount / 3;
   uint indices[2];
   uint trisCount, startIdx;
   for ( uint triangleIdx = 0; triangleIdx < trianglesCount; ++triangleIdx )
   {
      startIdx = triangleIdx * 3;
      for ( byte i = 0; i < 3; ++i )
      {
         trisCount = adjacencyMap.getTrianglesForEdge( startIdx + i, indices );
         if ( trisCount == 1 )
         {

            const uint startVtxIdx = m_indices[startIdx + i];
            const uint endVtxIdx = m_indices[startIdx + ( i + 1 ) % 3];

            outBoundaryEdgeSegments.pushBack( m_vertices[startVtxIdx] );
            outBoundaryEdgeSegments.pushBack( m_vertices[endVtxIdx] );
         }
      }
   }

   ASSERT( outBoundaryEdgeSegments.size( ) % 2 == 0 );
}

///////////////////////////////////////////////////////////////////////////////

bool PolygonMesh2D::splitTriangleWithVertex( uint triangleIdx, const Vector& splitterVertex, const FastFloat& distTolerance )
{
   const uint firstVtxIdx = triangleIdx * 3;
   for ( byte i = 0; i < 3; ++i )
   {
      const uint i1 = firstVtxIdx + i;
      const uint i2 = firstVtxIdx + ( ( i + 1 ) % 3 );
      const uint edgeStartIdx = m_indices[i1];
      const uint edgeEndIdx = m_indices[i2];

      const Vector& segStartVtx = m_vertices[edgeStartIdx];
      const Vector& segEndVtx = m_vertices[edgeEndIdx];

      Vector displToNewVtx, segNormal, segDirection;
      segDirection.setSub( segEndVtx, segStartVtx );
      const FastFloat segmentLength = segDirection.length();
      segDirection.div( segmentLength );
      displToNewVtx.setSub( splitterVertex, segStartVtx );
      VectorUtil::calculatePerpendicularVector2D( segDirection, segNormal );


      // is it on the front side of the segment
      const FastFloat distFromSegment = displToNewVtx.dot( segNormal );
      if ( distFromSegment < -distTolerance || distFromSegment > distTolerance )
      {
         // the circle doesn't intersect with this edge, but it might intersect the other 2 edges
         continue;
      }

      FastFloat x = segDirection.dot( displToNewVtx );
      if ( x < distTolerance || x > ( segmentLength - distTolerance ) )
      {
         continue;
      }
         
      // the vertex splits the triangle in half

      // first - the new vertex. Caution - there may be a similar vertex added already, so don't
      // add it just like that, but instead search the existing vertices for a match
      uint newVertexIdx = allocateVertex( splitterVertex, distTolerance );

      // add a new triangle that maps the remaining triangle vertices to the end of the collection
      const uint i3 = firstVtxIdx + ( ( i + 2 ) % 3 );
      const uint lastIdxOffset = m_indices.size();
      m_indices.resize( lastIdxOffset + 3 );
      m_indices[lastIdxOffset] = newVertexIdx;
      m_indices[lastIdxOffset + 1] = m_indices[i3];
      m_indices[lastIdxOffset + 2] = m_indices[i1];

      // remap the old triangle - put the new vertex in place of the one that used to end this edge
      m_indices[i1] = newVertexIdx;

      // our work is done - the point cannot intersect more than one edge of a triangle at a time
      return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

uint PolygonMesh2D::allocateVertex( const Vector& vertex, const FastFloat& distanceTolerance )
{
   const uint verticesCount = m_vertices.size();
   const FastFloat distToleranceSq = distanceTolerance * distanceTolerance;

   Vector t;
   for ( uint i = 0; i < verticesCount; ++i )
   {
      t.setSub( vertex, m_vertices[i] );
      if ( t.lengthSq() <= distToleranceSq )
      {
         // found a match
         return i;
      }
   }

   // this is a new vertex
   uint newIdx = m_vertices.size();
   m_vertices.push_back( vertex );

   return newIdx;
}

///////////////////////////////////////////////////////////////////////////////
