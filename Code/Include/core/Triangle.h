/// @file   core\Triangle.h
/// @brief  a triangle representation
#pragma once

#include "core\Vector.h"
#include "core\Plane.h"
#include "core\Assert.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

struct Plane;
struct Matrix;
struct AxisAlignedBox;
struct RaycastResult;
struct IntersectionResult;
struct Ray;
class InStream;
struct Sphere;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

enum PlaneClassification
{
   PPC_BACK,
   PPC_FRONT,
   PPC_COPLANAR,
   PPC_SPANNING
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure represents a triangle with some basic operations
 * we can perform on it.
 */
ALIGN_16 struct Triangle
{
   DECLARE_ALLOCATOR( Triangle, AM_ALIGNED_16 );

private:
   // static data
   Vector v[3];
   Vector e[3];
   Vector en[3];

   // runtime data
   Plane  m_trianglePlane;

public:
   /**
    * Default constructor.
    */
   Triangle();

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Triangle( const Triangle& rhs );

   /**
    * Constructor.
    *
    * Make sure to specify the edges in the CLOCKWISE order,
    * otherwise the collision tests will fail.
    *
    * @param pt1
    * @param pt2
    * @param pt3
    */
   Triangle( const Vector& pt1, const Vector& pt2, const Vector& pt3 );

   /**
    * Sets the vertices of the triangle.
    *
    * Make sure to specify the edges in the CLOCKWISE order,
    * otherwise the collision tests will fail.
    *
    * @param pt1
    * @param pt2
    * @param pt3
    */
   void setFromVertices( const Vector& pt1, const Vector& pt2, const Vector& pt3 );

   const Vector& vertex( unsigned int idx ) const
   {
      ASSERT_MSG(idx <= 2, "Vertex index should be <= 2");
      return v[idx];
   }

   const Vector& vertexPos( unsigned int idx ) const
   {
      ASSERT_MSG(idx <= 2, "Vertex index should be <= 2");
      return v[idx];
   }

   const Vector& edge( unsigned int idx ) const
   {
      ASSERT_MSG(idx <= 2, "Edge index should be <= 2");

      return e[idx];
   }

   const Vector& edgeNormal( unsigned int idx ) const
   {
      ASSERT_MSG(idx <= 2, "Edge normal index should be <= 2");

      return en[idx];
   }

   inline void getTriangleNormal( Vector& outNormal ) const
   {
      return m_trianglePlane.getNormal( outNormal );
   }

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   bool operator==( const Triangle& rhs ) const;
   bool operator!=( const Triangle& rhs ) const; 

   // -------------------------------------------------------------------------
   // Splittable triangle implementation
   // -------------------------------------------------------------------------
   void splitEdge( const FastFloat& percentage, unsigned int startVtxIdx, unsigned int endVtxIdx, Vector& outEdge ) const;

   void split( const Plane& splitPlane, Array< Triangle* >& frontSplit, Array< Triangle* >& backSplit );

   /**
    * Transforms the triangle.
    *
    * @param mtx
    * @param transformedTriangle
    */
   void transform( const Matrix& mtx, Triangle& transformedTriangle ) const;

   /**
    * Calculates a bounding box around the triangle
    */
   void calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const;

   /**
    * Calculates triangle's distance to the specified plane.
    *
    * @param plane
    */
   const FastFloat distanceToPlane( const Plane& plane ) const;

   /**
    * Tells which side of the plane the triangle is located at.
    *
    * @param plane
    */
   PlaneClassification classifyAgainsPlane( const Plane& plane ) const;

   /**
    * A ray-cast against a triangle.
    *
    * @param ray
    * @param outResult
    */
   bool rayCast( const Ray& ray, RaycastResult& outResult ) const;

   /**
    * Tests the intersection between the triangle and a sphere.
    *
    * @param sphere
    * @param outResult
    * @return  'true' if there's a collision, 'false' otherwise
    */
   bool testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const;

   /**
    * Used for debug purposes - sends a textual description of the triangle to an output stream.
    *
    * @param os
    * @param t
    */
   friend std::ostream& operator<<( std::ostream &os, const Triangle& t );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const Triangle& triangle );
   friend InStream& operator>>( InStream& stream, Triangle& triangle );

protected:
   void triangulatePoly( const Array< Vector >& poly, Array< Triangle* >& output);
};

///////////////////////////////////////////////////////////////////////////////
