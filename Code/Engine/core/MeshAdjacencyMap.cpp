#include "core\MeshAdjacencyMap.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

void MeshAdjacencyMap::initialize( uint verticesCount, uint edgesCount )
{
   m_trianglesSharedByVertex.resize( verticesCount );

   // Each edge can be shared by at most 2 triangles.
   // Let's use that to our advantage and create a simple array that will store 2 entries per each edge
   m_trianglesSharedByEdges.resize( edgesCount * 2, (uint)-1 );

   m_trianglesGraph.clear();
   m_highestTriangleIdx = 0;
}

///////////////////////////////////////////////////////////////////////////////

void MeshAdjacencyMap::assignTriangleToVertex( uint vertexIdx, uint triangleIdx )
{
   List< TriangleIdx >& l = m_trianglesSharedByVertex[vertexIdx];
   List< TriangleIdx >::iterator it = ListUtils::find( l, triangleIdx );
   if ( it.isEnd() )
   {
      l.pushBack( triangleIdx );
   }

   // determine the largest triangle index - we need that info later on to build the graph
   m_highestTriangleIdx = max2( m_highestTriangleIdx, triangleIdx );
}

///////////////////////////////////////////////////////////////////////////////

const List< uint >& MeshAdjacencyMap::getTrianglesOfVertex( uint vertexIdx ) const
{
   ASSERT_MSG( vertexIdx < m_trianglesSharedByVertex.size(), "Invalid vertex index" );

   const List< TriangleIdx >& l = m_trianglesSharedByVertex[vertexIdx];
   return l;
}

///////////////////////////////////////////////////////////////////////////////

void MeshAdjacencyMap::assignTriangleToEdge( uint edgeIdx, uint triangleIdx )
{
   const uint addr = edgeIdx * 2;

   // assign it to the first available slot
   if ( m_trianglesSharedByEdges[addr] == (uint)-1 )
   {
      m_trianglesSharedByEdges[addr] = triangleIdx;
   }
   else if ( m_trianglesSharedByEdges[addr + 1] == (uint)-1 )
   {
      m_trianglesSharedByEdges[addr + 1] = triangleIdx;
   }
   else
   {
      ASSERT_MSG( false, "Edge seems to be shared by more than one triangle, which shouldn't be possible with a 2D polygon mesh" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MeshAdjacencyMap::buildGraph( )
{
   // add the nodes
   for ( uint i = 0; i <= m_highestTriangleIdx; ++i )
   {
      m_trianglesGraph.addNode( i );
   }

   // connect the triangles
   const uint edgesCount = m_trianglesSharedByEdges.size() / 2;
   TriangleIdx indices[2];
   uint trisCount;
   for ( uint edgeIdx = 0; edgeIdx < edgesCount; ++edgeIdx )
   {
      trisCount = getTrianglesForEdge( edgeIdx, indices );
      if ( trisCount == 2 && !m_trianglesGraph.areConnected( indices[0], indices[1] ) )
      {
         // define a 2 way connection
         m_trianglesGraph.connect( indices[0], indices[1] );
         m_trianglesGraph.connect( indices[1], indices[0] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

uint MeshAdjacencyMap::getTrianglesForEdge( uint edgeIdx, uint* outArrTriangleIndices ) const
{
   const uint addr = edgeIdx * 2;
   outArrTriangleIndices[0] = m_trianglesSharedByEdges[addr];
   outArrTriangleIndices[1] = m_trianglesSharedByEdges[addr + 1];
   
   return outArrTriangleIndices[1] < (uint)-1 ? 2 : 1;
}

///////////////////////////////////////////////////////////////////////////////

uint MeshAdjacencyMap::getTrianglesCount( ) const
{
   return m_trianglesGraph.getNodesCount();
}

///////////////////////////////////////////////////////////////////////////////

uint MeshAdjacencyMap::getAdjacentTriangles( uint triangleIdx, uint* outArrTriangleIndices ) const
{
   const TriangleGraph::EdgeIndices& edges = m_trianglesGraph.getEdges( triangleIdx );
   
   const uint count = edges.size();
   for ( uint i = 0; i < count; ++i )
   {
      outArrTriangleIndices[i] = m_trianglesGraph.getEdge( edges[i] );
   }

   return count;
}

///////////////////////////////////////////////////////////////////////////////
