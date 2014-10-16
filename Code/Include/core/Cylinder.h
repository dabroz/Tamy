/// @file   core/Cylinder.h
/// @brief  
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Plane;
struct Matrix;
struct AxisAlignedBox;
struct Sphere;
struct Ray;
struct RaycastResult;
struct IntersectionResult;

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct Cylinder
{
   DECLARE_ALLOCATOR( Cylinder, AM_DEFAULT );

   Vector               m_bottomBaseOrigin;
   Vector               m_cylinderDir;
   FastFloat            m_radius;
   FastFloat            m_height;


   /**
    * Default constructor.
    */
   Cylinder();

   /**
    * Constructor.
    *
    * @param bottomBase
    * @param topBase
    * @param radius
    */
   Cylinder( const Vector& bottomBase, const Vector& topBase, float radius );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Cylinder( const Cylinder& rhs );

   /**
    * Sets new cylinder values.
    *
    * @param bottomBase
    * @param topBase
    * @param radius
    */
   void set( const Vector& bottomBase, const Vector& topBase, float radius ); 

   /**
    * Sets new cylinder values.
    *
    * @param bottomBase
    * @param direction
    * @param radius
    * @param height
    */
   void set( const Vector& bottomBase, const Vector& direction, float radius, float height );

   /**
    * Transforms the sphere.
    *
    * @param mtx
    * @param transformedSphere
    */
   void transform( const Matrix& mtx, Cylinder& cylinder ) const;

   /**
    * Calculates an axis aligned bounding box around the cylinder.
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
    * A ray-cast against a cylinder.
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
    * Checks if a sphere intersects this cylinder.
    * 
    * @param sphere
    * @param outResult
    */
   bool testIntersection( const Sphere& sphere, IntersectionResult& outResult )  const;

};

///////////////////////////////////////////////////////////////////////////////
