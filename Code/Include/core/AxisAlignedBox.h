/// @file   core/AxisAlignedBox.h
/// @brief  
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Plane;
struct Matrix;
struct RaycastResult;
struct IntersectionResult;
struct Ray;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct AxisAlignedBox
{
   DECLARE_ALLOCATOR( AxisAlignedBox, AM_ALIGNED_16 );

   Vector min;
   Vector max;

   /**
    * Constructor.
    */
   AxisAlignedBox();

   /**
    * Constructor.
    *
    * @param min
    * @param max
    */
   AxisAlignedBox( const Vector& min, const Vector& max );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   AxisAlignedBox( const AxisAlignedBox& rhs );

   /**
    * Sets the corner values of the bounding box.
    *
    * @param min
    * @param max
    */
   void set( const Vector& min, const Vector& max );

   /**
    * Sets new bounds and expands them at the same time, so that the bounding box becomes larger.
    *
    * @param bounds
    * @param multiplier
    */
   void setExpanded( const AxisAlignedBox& bounds, const FastFloat& multiplier );

   /**
    * Union of two boxes.
    *
    * @param otherBox
    * @param unionBox
    */
   void add( const AxisAlignedBox& otherBox, AxisAlignedBox& unionBox ) const; 

   /**
    * Intersection of two boxes.
    *
    * @param otherBox
    * @param intersectionBox
    */
   void intersection( const AxisAlignedBox& otherBox, AxisAlignedBox& intersectionBox ) const;

   /**
    * This method will modify the bounding box such that it includes
    * the specified point.
    *
    * @param pt   point we want to bound inside the box
    */
   void include( const Vector& pt );

   /**
    * Calculates a vector with the extents of the bounding box in all dimensions.
    *
    * @param outExtents
    */
   void getExtents( Vector& outExtents ) const;

   /**
    * Calculates the central point of the bounding box.
    *
    * @param outCenter
    */
   void getCenter( Vector& outCenter ) const;

   /**
    * Returns vertices of the box.
    *
    * @param outArrVertices      this array needs to be large enough to accommodate 8 vertices
    */
   void getVertices( Vector* outArrVertices ) const;

   /**
    * Returns edge indices of the box, indices that can be used to index into an array of
    * vertices returned by 'getVertices' method.
    * Every axis aligned box is going to return the same configuration of edge indices, and
    * that's why this method is static.
    *
    * Every edge features 2 indices - ( startVtxIdx, endVtxIdx ). But for simplicity reasons,
    * we're putting them into a linear array, where an N-th edge's start idx can be found
    * at N*2 idx, and the N-th edge's end idx - at N*2+1 idx.
    *
    * @param outArrEdgeIndices
    */
   static void getEdges( uint* outArrEdgeIndices );

   /**
    * Resets the bounds.
    */
   void reset();

   /**
    * Calculates the area occupied by the box on the XY plane.
    */
   float calcArea() const;

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   bool operator==( const AxisAlignedBox& rhs ) const;
   bool operator!=( const AxisAlignedBox& rhs ) const;
   

   /**
    * Transforms the box.
    *
    * @param mtx
    * @param transformedAAB
    */
   void transform( const Matrix& mtx, AxisAlignedBox& transformedAAB ) const;

   /**
    * Calculates distance to the specified plane.
    *
    * @param plane
    */
   const FastFloat distanceToPlane( const Plane& plane ) const;

   /**
    * A ray-cast against an Axis Aligned Box.
    *
    * @param ray
    * @param outResult
    */
   bool rayCast( const Ray& ray, RaycastResult& outResult ) const;

   /**
    * Checks if the specified point is located inside the AAB.
    *
    * @param point
    * @param outResult
    */
   bool isInside( const Vector& point, IntersectionResult& outResult ) const;

   /**
    * Checks if the boxes intersect and calculates a contact point along the specified axis
    * ( the test is performed in both directions of the axis, to ensure that the smallest
    * penetration distance will be found ).
    *
    * @param point
    * @param contactAxis
    * @param outResult
    */
   bool calcContactPoint( const Vector& point, const Vector& contactAxis, IntersectionResult& outResult ) const;

   /**
    * Tests intersection of two axis aligned boxes.
    *
    * @param aabb
    */
   bool testIntersection( const AxisAlignedBox& aabb ) const;

   /**
    * If we have two intersecting axis aligned boxes, this method will return a displacement vector
    * that will move the first box so that the two don't collide any more.
    *
    * @param aabb
    * @param outRemovalVec
    */
   void findIntersectionRemovalVector( const AxisAlignedBox& aabb, Vector& outRemovalVec ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const AxisAlignedBox& box );
   friend InStream& operator>>( InStream& stream, AxisAlignedBox& box );
};

///////////////////////////////////////////////////////////////////////////////
