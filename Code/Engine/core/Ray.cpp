#include "core.h"
#include "core\Ray.h"
#include "core\Assert.h"
#include "core\AxisAlignedBox.h"
#include "core\Sphere.h"
#include "core\Algorithms.h"
#include "core\Matrix.h"
#include "core\Plane.h"
#include "core\Box.h"
#include "core\RaycastResult.h"


///////////////////////////////////////////////////////////////////////////////

Ray::Ray() 
   : origin( Quad_0 )
   , direction( Quad_0010 ) 
{
}

///////////////////////////////////////////////////////////////////////////////

Ray::Ray( const Vector& _origin, const Vector& _direction )
   : origin( _origin )
{
   direction.setNormalized( _direction );
}

///////////////////////////////////////////////////////////////////////////////

void Ray::setFromTwoPoints( const Vector& _origin, const Vector& _endPoint )
{
   origin = _origin;

   direction.setSub( _endPoint, origin );
   direction.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Ray::setFromPointDirection( const Vector& _origin, const Vector& _direction )
{
   origin = _origin;
   direction.setNormalized( _direction );
}

///////////////////////////////////////////////////////////////////////////////

void Ray::transform( const Matrix& mtx, Ray& transformedRay ) const
{
   mtx.transform( origin, transformedRay.origin );
   mtx.transformNorm( direction, transformedRay.direction );
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const AxisAlignedBox& aabb, RaycastResult& outResult ) const
{
   return aabb.rayCast( *this, outResult );
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const Sphere& sphere, RaycastResult& outResult ) const
{
   return sphere.rayCast( *this, outResult );
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const Triangle& triangle, RaycastResult& outResult ) const
{
   return triangle.rayCast( *this, outResult );
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const Plane& plane, RaycastResult& outResult ) const
{
   const FastFloat planeDistance = plane.dotCoord( origin );
   const FastFloat projRayLength = plane.dotNormal( direction );

   if ( projRayLength == Float_0 )
   {
      if ( planeDistance == Float_0 )
      {
         outResult.m_dist = Float_0;
      }
      else
      {
         outResult.m_dist = Float_Minus1;
      }
   }
   else
   {
      outResult.m_dist.setDiv(  planeDistance, projRayLength );
      outResult.m_dist.neg();
   }

   if ( outResult.m_dist < Float_0 ) 
   {
      return false;
   }
   else
   {
      outResult.m_contactPt.setMulAdd( direction, outResult.m_dist, origin );
      plane.getNormal( outResult.m_surfaceNormal );
      return true;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const Cylinder& cylinder, RaycastResult& outResult ) const
{
  return cylinder.rayCast( *this, outResult );
}

///////////////////////////////////////////////////////////////////////////////

bool Ray::rayCast( const Box& box, RaycastResult& outResult ) const
{
   return box.rayCast( *this, outResult );
}

///////////////////////////////////////////////////////////////////////////////
