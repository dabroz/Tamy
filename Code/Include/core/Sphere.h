/// @file   core/Sphere.h
/// @brief  Sphere shape
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Matrix;
struct AxisAlignedBox;
struct Plane;
struct RaycastResult;
struct Ray;
struct IntersectionResult;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct Sphere
{
   DECLARE_ALLOCATOR( Sphere, AM_ALIGNED_16 );

   Vector      origin;
   FastFloat   radius;

   /**
    * Default constructor.
    */
   Sphere();

   /**
    * Constructor.
    *
    * @param origin
    * @param radius
    */
   Sphere( const Vector& origin, float radius );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Sphere( const Sphere& rhs );

   /**
    * Sets new sphere values.
    * 
    * @param origin
    * @param radius
    */
   void set( const Vector& origin, float radius );

   /**
    * Sets new sphere values.
    *
    * @param origin
    * @param radius
    */
   void set( const Vector& origin, const FastFloat& radius );


   /**
    * This method will expand the bounding sphere so that it includes the specified point
    * in space.
    *
    * @param pt   point we want to bound inside the sphere
    */
   void include( const Vector& pt );

   /**
    * Transforms the sphere.
    *
    * @param mtx
    * @param transformedSphere
    */
   void transform( const Matrix& mtx, Sphere& transformedSphere ) const;

   /**
   * Builds a sphere that surrounds the specified axis aligned box.
   *
   * @param box
   */
   void fromAxisAlignedBox( const AxisAlignedBox& box );

   /**
    * Calculates an axis aligned bounding box around the sphere.
    *
    * @param outBoundingBox
    */
   void calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const;

   /**
    * Calculates a distance from the sphere to the specified plane.
    *
    * @param plane
    */
   const FastFloat distanceToPlane( const Plane& plane ) const;

   /**
    * A ray-cast against a sphere.
    *
    * @param ray
    * @param outResult
    */
   bool rayCast( const Ray& ray, RaycastResult& outResult ) const;

   /**
    * Checks if the specified point is located inside the sphere.
    *
    * @param point
    */
   bool isInside( const Vector& point ) const;

   /**
    * Checks an intersection between this sphere and the specified axis-aligned bounding box.
    *
    * @param aabb
    * @param outResult
    */
   bool testIntersection( const AxisAlignedBox& aabb, IntersectionResult& outResult ) const;

   /**
    * Checks an intersection between two spheres
    *
    * @param sphere
    * @param outResult
    */
   bool testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const Sphere& sphere );
   friend InStream& operator>>( InStream& stream, Sphere& sphere );
};

///////////////////////////////////////////////////////////////////////////////
