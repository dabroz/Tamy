#include "core\Frustum.h"
#include "core\Assert.h"
#include "core\Matrix.h"
#include "core\AxisAlignedBox.h"
#include "core\Sphere.h"
#include "core\PlaneUtils.h"


///////////////////////////////////////////////////////////////////////////////

void Frustum::transform( const Matrix& mtx, Frustum& transformedFrustum ) const
{
   for (char i = 0; i < 6; ++i)
   {
      mtx.transform( planes[i], transformedFrustum.planes[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Frustum::isInside(const Vector& point ) const
{
   for ( int i = 0; i < 6; ++i )
   {
      const FastFloat n = planes[i].dotCoord( point );
      if ( n < Float_0 ) 
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Frustum::isInside( const AxisAlignedBox& aabb ) const
{
   Vector pv, nv;
   for ( int i = 0; i < 6; ++i )
   {
      const Plane& plane = planes[i];

      pv.set( plane[0] > 0 ? aabb.max[0] : aabb.min[0], plane[1] > 0 ? aabb.max[1] : aabb.min[1], plane[2] > 0 ? aabb.max[2] : aabb.min[2], 1.0f );

      const FastFloat n = plane.dotCoord( pv );
      if ( n < Float_0 )
      {
         // bounding box is outside the frustum
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Frustum::isInside( const Sphere& sphere ) const
{
   FastFloat negSphereRad;
   negSphereRad.setNeg( sphere.radius );

   for ( int i = 0; i < 6; ++i )
   {
      const FastFloat n = planes[i].dotCoord( sphere.origin );
      if ( n < negSphereRad ) 
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void Frustum::calculateBoundingBox( AxisAlignedBox& outBB ) const
{
   Vector points[8];
   PlaneUtils::calculatePlanesIntersection( planes[FP_LEFT], planes[FP_TOP], planes[FP_NEAR], points[0] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_RIGHT], planes[FP_TOP], planes[FP_NEAR], points[1] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_LEFT], planes[FP_BOTTOM], planes[FP_NEAR], points[2] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_RIGHT], planes[FP_BOTTOM], planes[FP_NEAR], points[3] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_LEFT], planes[FP_TOP], planes[FP_FAR], points[4] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_RIGHT], planes[FP_TOP], planes[FP_FAR], points[5] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_LEFT], planes[FP_BOTTOM], planes[FP_FAR], points[6] );
   PlaneUtils::calculatePlanesIntersection( planes[FP_RIGHT], planes[FP_BOTTOM], planes[FP_FAR], points[7] );

   outBB.min.set( FLT_MAX, FLT_MAX, FLT_MAX );
   outBB.max.set( -FLT_MAX, -FLT_MAX, -FLT_MAX );
   for ( char i = 0; i < 8; ++ i )
   {
      outBB.include( points[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

