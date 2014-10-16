/// @file   core\Ray.h
/// @file   Mathematical representation of a ray.
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Sphere;
struct RaycastResult;
struct AxisAlignedBox;
struct Triangle;
struct Cylinder;
struct Box;

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical representation of a ray.
 */
ALIGN_16 struct Ray
{
   DECLARE_ALLOCATOR( Ray, AM_ALIGNED_16 );

   Vector origin;

   // direction should always be a normalized vector. If it's 
   // a problem (someone's changing it from outside),
   // restrict access with getters and setters
   Vector direction;

   /**
    * Default constructor.
    */
   Ray();

   /**
    * Constructor.
    *
    * @param origin
    * @param direction
    */
   Ray( const Vector& origin, const Vector& direction );

   /**
    * Initializes a ray that starts at the origin and goes through the end point.
    *
    * @param origin
    * @param endPoint
    */
   void setFromTwoPoints( const Vector& origin, const Vector& endPoint );

   /**
    * Initializes a ray that starts at the origin and goes int the specified direction.
    *
    * @param origin
    * @param direction
    */
   void setFromPointDirection( const Vector& origin, const Vector& direction );

   /**
    * Transforms the ray.
    *
    * @param mtx
    * @param transformedRay
    */
   void transform( const Matrix& mtx, Ray& transformedRay ) const;

   // -------------------------------------------------------------------------
   // Ray casts
   // -------------------------------------------------------------------------

   /**
    * A ray-cast against a plane.
    *
    * @param plane
    * @param outResult
    */
   bool rayCast( const Plane& plane, RaycastResult& outResult ) const;

   /**
    * A ray-cast against an Axis Aligned Box.
    *
    * @param aabox
    * @param outResult
    */
   bool rayCast( const AxisAlignedBox& aabox, RaycastResult& outResult ) const;

   /**
    * A ray-cast against a sphere.
    *
    * @param sphere
    * @param outResult
    */
   bool rayCast( const Sphere& sphere, RaycastResult& outResult ) const;

   /**
    * A ray-cast against a triangle.
    *
    * @param triangle
    * @param outResult
    */
   bool rayCast( const Triangle& triangle, RaycastResult& outResult ) const;

   /**
    * A ray-cast against a cylinder.
    *
    * @param cylinder
    * @param outResult
    */
   bool rayCast( const Cylinder& cylinder, RaycastResult& outResult ) const;

   /**
    * A ray-cast against a box.
    *
    * @param box
    * @param outResult
    */
   bool rayCast( const Box& box, RaycastResult& outResult ) const;

};

///////////////////////////////////////////////////////////////////////////////
