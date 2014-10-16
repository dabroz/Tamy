/// @file   core/MeshAdjacencyMap.h
/// @brief  this map describes which triangles are connected together in a mesh
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\List.h"
#include "core\Array.h"
#include "core\Graph.h"

///////////////////////////////////////////////////////////////////////////////

struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * This map describes which triangles are connected together in a mesh.
 */
class MeshAdjacencyMap
{
   DECLARE_ALLOCATOR( MeshAdjacencyMap, AM_DEFAULT );

private:
   typedef uint                     TriangleIdx;

   Array< List< TriangleIdx > >     m_trianglesSharedByVertex;
   Array< TriangleIdx >             m_trianglesSharedByEdges;
   uint                             m_highestTriangleIdx;

   typedef Graph< TriangleIdx >     TriangleGraph;
   TriangleGraph                    m_trianglesGraph;
   
public:
   /**
    * Initializes the map, destroying its previous contents.
    *
    * @param verticesCount
    * @param indicesCount
    */
   void initialize( uint verticesCount, uint indicesCount );

   /**
    * Builds an adjacency graph.
    * Should be called after all indices have been assigned.
    */
   void buildGraph();

   /**
    * Maps a triangle to a vertex.
    *
    * @param vertexIdx
    * @param triangleIdx
    */
   void assignTriangleToVertex( uint vertexIdx, uint triangleIdx );

   /**
    * Returns indices of triangles that share the specified vertex.
    *
    * @param vertexIdx
    * @return a list of indices of triangles that share that vertex
    */
   const List< uint >& getTrianglesOfVertex( uint vertexIdx ) const;

   /**
    * Maps a triangles that shares an edge to that edge.
    */
   void assignTriangleToEdge( uint edgeIdx, uint triangleIdx );

   /**
    * Returns indices of triangles that share the specified edge.
    *
    * @param edgeIdx
    * @param outArrTriangleIndices  ( this array should be able to store at least 2 elements ).
    * @return number of entries returned
    */
   uint getTrianglesForEdge( uint edgeIdx, uint* outArrTriangleIndices ) const;

   /**
    * Returns the number of triangles the map describes.
    */
   uint getTrianglesCount() const;

   /**
    * Returns indices of triangles adjacent to the specified triangle.
    *
    * @param triangleIdx
    * @param outArrTriangleIndices  ( this array should be able to store at least 3 elements ).
    * @return number of entries returned
    */
   uint getAdjacentTriangles( uint triangleIdx, uint* outArrTriangleIndices ) const;
};

///////////////////////////////////////////////////////////////////////////////
