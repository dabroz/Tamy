/// @file   core/Frustum.h
/// @brief  Frustum representation
#pragma once

#include "core\MemoryRouter.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

struct AxisAlignedBox;
struct Sphere;
struct Matrix;
struct Frustum;

///////////////////////////////////////////////////////////////////////////////

enum PlanesEnum
{
   FP_BOTTOM,
   FP_TOP,
   FP_LEFT,
   FP_RIGHT,
   FP_FAR,
   FP_NEAR,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Frustum representation.
 */
ALIGN_16 struct Frustum
{
   DECLARE_ALLOCATOR( Frustum, AM_ALIGNED_16 );

   Plane planes[6];

   /**
    * Transforms the frustum.
    *
    * @param mtx
    * @param transformedFrustum
    */
   void transform( const Matrix& mtx, Frustum& transformedFrustum ) const;

   /**
    * Calculates an axis-aligned bounding box around the frustum.
    *
    * @param outBoundingBox
    */
   void calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const;

   /**
    * Tests if a point is located inside the frustum
    *
    * @param point
    */
   bool isInside( const Vector& point ) const;

   /**
    * Tests if an axis-aligned bounding box is inside the frustum.
    *
    * @param aabb
    */
   bool isInside( const AxisAlignedBox& aabb ) const;

   /**
    * Tests if a sphere is inside the frustum.
    *
    * @param aabb
    */
   bool isInside( const Sphere& sphere ) const;
};

///////////////////////////////////////////////////////////////////////////////
