/// @file   Tamy/DebugGeometryBuilder.h
/// @brief  debug geometrical shapes builder
#pragma once

#include "core\Math.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class TriangleMesh;
struct LitVertex;
struct Face;
struct Color;

///////////////////////////////////////////////////////////////////////////////

class DebugGeometryBuilder
{
public:
   /**
    * Creates a cube mesh.
    *
    * @param min     bounding box's minimum extent
    * @param max     bounding box's maximum extent
    */
   static TriangleMesh* createBox( const Vector& min, const Vector& max );

   /**
    * Creates a line-shaped mesh with an arrow-shaped tip.
    *
    * @param size    arrow line 'width'
    * @param start   arrow start point
    * @param end     arrow tip end point
    * @return        new triangle mesh instance
    */
   static TriangleMesh* createArrow( float size, const Vector& start, const Vector& end );

   /**
   * Creates a diamond shape
   *
   * @param size    arrow line 'width'
   * @param start   arrow start point
   * @param end     arrow tip end point
   * @return        new triangle mesh instance
   */
   static TriangleMesh* createDiamond( float size, const Vector& start, const Vector& end );

   /**
    * Creates a line-shaped mesh with a box-shaped tip.
    *
    * @param size    arrow line 'width'
    * @param start   arrow start point
    * @param end     arrow tip end point
    * @return        new triangle mesh instance
    */
   static TriangleMesh* createBoxHeadedArrow( float size, const Vector& start, const Vector& end );


   /**
    * Creates a torus.
    * A torus is a ring composed of connected SEGMENTS ( number of which is controlled by the 'segmentsCount' param ).
    * Each segment has a number of vertices, the number of which you can specify using the 'segmentVerticesCount' parameter
    *
    * @param innerRadius
    * @param outerRadius
    * @param transform
    * @param segmentsCount
    * @param segmentVerticesCount
    * @return        new triangle mesh instance
    */
   static TriangleMesh* createTorus( float innerRadius, float outerRadius, const Matrix& transform, int segmentsCount, int segmentVerticesCount );

   /**
   * Creates a plane with the specified dimensions.
   *
   * @param width
   * @param height
   * @param horizTextureSize
   * @param vertTextureSize
   */
   static TriangleMesh* createPlane( float width, float height, float horizTextureSize, float vertTextureSize );

   /**
    * Creates a 'bone' debug mesh.
    */
   static TriangleMesh* createBone();

private:
   static void addCone( const FastFloat& baseSize, const Vector& start, const Vector& end, Array< LitVertex >& outVertices, Array< Face >& outFaces );
   static void addCuboid( const FastFloat& size, const Vector& start, const Vector& end, Array< LitVertex >& outVertices, Array< Face >& outFaces );
};

///////////////////////////////////////////////////////////////////////////////
