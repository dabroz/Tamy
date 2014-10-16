/// @file   core/PolygonMesh2D.h
/// @brief  a mesh made of polygon faces
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Triangle;
class MeshAdjacencyMap;

///////////////////////////////////////////////////////////////////////////////

struct PolygonMergeRules
{
   DECLARE_ALLOCATOR( PolygonMergeRules, AM_ALIGNED_16 );

   FastFloat      m_angularDifference;
   FastFloat      m_vertexWeldDistance;

   PolygonMergeRules();
};

///////////////////////////////////////////////////////////////////////////////

/**
* A mesh made of polygon faces.
*/
ALIGN_16 class PolygonMesh2D
{
   DECLARE_ALLOCATOR( PolygonMesh2D, AM_ALIGNED_16 );

private:
   Vector               m_normal;
   Array< Vector >      m_vertices;
   Array< word >        m_indices;
   
public:
   /**
    * Constructor.
    *
    * @param plane
    */
   PolygonMesh2D( const Vector& normal );

   /**
   * Turns a triangle mesh into a polygon mesh, simplifying whatever triangles can be simplified
   *
   * @param arrVertices
   * @param arrIndices          3 subsequent indices make a single triangle face
   * @param trianglesCount
   * @param mergeRules
   */
   void addTriangleSoup( const Vector* arrVertices, const word* arrIndices, const uint trianglesCount, const PolygonMergeRules& mergeRules );

   /**
    * Adds a triangle to the mesh.
    *
    * @param triangle
    * @param mergeRules
    * @return 'true' if the triangle was successfully added, 'false' if it didn't meet the constraints
    */
   bool addTriangle( const Triangle& triangle, const PolygonMergeRules& mergeRules );

   /**
    * Returns the number of stored triangles.
    */
   uint getTriangleCount() const;

   /**
    * Returns a selected triangle.
    *
    * @param triangleIdx
    * @param outTriangle
    */
   void getTriangle( uint triangleIdx, Triangle& outTriangle ) const;

   /**
    * Generates an adjacency map of the mesh.
    *
    * @param outMap
    */
   void generateAdjacencyMap( MeshAdjacencyMap& outMap ) const;

   /**
   * Returns a list of segments that create a boundary edge around the described polygon.
   * Every segment is composed of 2 adjacent list elements.
   *
   * @param adjacencyMap
   * @param outBoundaryEdgeSegments
   */
   void getBoundaryEdge( const MeshAdjacencyMap& adjacencyMap, List< Vector >& outBoundaryEdgeSegments ) const;

private:
   /**
    * Checks if the vertex splits any of the triangle's edges, and if it does, it splits
    * the triangle in two, leaving the first half where the previous triangle was, and adding the new
    * one to the end of the triangles list.
    *
    * @param triangleIdx
    * @param splitterVertex
    * @param distTolerance
    * @return  'true' if the triangle was split, 'false' otherwise
    */
   bool splitTriangleWithVertex( uint triangleIdx, const Vector& splitterVertex, const FastFloat& distTolerance );

   /**
    * Tries to match the specified vertex with an existing vertex, and if that succeeds, 
    * the method returns the index of that vertex.
    * Otherwise it will add the vertex to the collection of vertices and return an index
    * assigned to it.
    */
   uint allocateVertex( const Vector& vertex, const FastFloat& distanceTolerance );
};

///////////////////////////////////////////////////////////////////////////////
