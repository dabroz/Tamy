#include "core\Sphere.h"
#include "core\Assert.h"
#include "core\Plane.h"
#include "core\Matrix.h"
#include "core\AxisAlignedBox.h"
#include "core\RaycastResult.h"
#include "core\Ray.h"
#include "core\IntersectionResult.h"
#include "core\InStream.h"
#include "core\OutStream.h"


///////////////////////////////////////////////////////////////////////////////

Sphere::Sphere()
   : origin( Quad_0001 )
   , radius( Float_Minus1 )
{
}

///////////////////////////////////////////////////////////////////////////////

Sphere::Sphere( const Vector& _origin, float _radius )
   : origin( _origin )
{
   radius.setFromFloat( _radius );
   origin[3] = 1;
}

///////////////////////////////////////////////////////////////////////////////

Sphere::Sphere( const Sphere& rhs )
   : origin( rhs.origin )
   , radius( rhs.radius )
{
}

///////////////////////////////////////////////////////////////////////////////

void Sphere::set( const Vector& _origin, float _radius )
{
   origin = _origin;
   origin[3] = 1;
   
   radius.setFromFloat( _radius );
}

///////////////////////////////////////////////////////////////////////////////

void Sphere::set( const Vector& _origin, const FastFloat& _radius )
{
   origin = _origin;
   origin[3] = 1;

   radius = _radius;
}

///////////////////////////////////////////////////////////////////////////////

void Sphere::include( const Vector& pt )
{
   Vector displ;
   displ.setSub( pt, origin );
   FastFloat distToOrigin = displ.length();
   if ( distToOrigin <= radius )
   {
      // nothing to do here - the point is already inside the sphere
      return;
   }

   if ( radius < Float_0 )
   {
      // there was nothing inside the sphere before, so set the new origin here
      origin = pt;
      radius = Float_0;
   }
   else
   {
      // if we think about it, the new circle that embraces the previous one and the new point
      // has a diameter equal to the distance between the two points ( the origin and the new point ) + the old radius
      // and its origin lies exactly in the middle of the diameter ( 'cause it's a circle ;))
      radius = ( distToOrigin + radius ) * Float_Inv2;
      displ.div( distToOrigin );
      origin.setMulAdd( displ, radius, pt );
   }

}

///////////////////////////////////////////////////////////////////////////////

void Sphere::transform( const Matrix& mtx, Sphere& transformedSphere ) const
{
   mtx.transform( origin, transformedSphere.origin );
   transformedSphere.radius = radius;
}

///////////////////////////////////////////////////////////////////////////////

void Sphere::fromAxisAlignedBox( const AxisAlignedBox& box )
{
   Vector extents;
   box.getExtents( extents );
   radius = extents.length() * Float_Inv2;
   box.getCenter( origin );
}

///////////////////////////////////////////////////////////////////////////////

void Sphere::calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const 
{
   Vector radVec;
   radVec.setBroadcast( radius );

   outBoundingBox.min.setSub( origin, radVec );
   outBoundingBox.max.setAdd( origin, radVec );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Sphere::distanceToPlane( const Plane& plane ) const
{
   const FastFloat distance = plane.dotCoord( origin );
   FastFloat absDist;
   absDist.setAbs( distance );

   if ( absDist <= radius ) 
   { 
      return Float_0; 
   }
   else
   { 
      return distance; 
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Sphere::rayCast( const Ray& ray, RaycastResult& outResult ) const
{
   FastFloat a, b, c, l2;

   Vector L;
   L.setSub( ray.origin, origin );
   l2 = L.lengthSq();

   a = ray.direction.lengthSq();
   b.setMul( Float_2, ray.direction.dot( L ) );

   c.setMul( radius, radius );
   c.neg();
   c.add( l2 );

   if ( c < Float_0 ) 
   {
      // the ray starts inside the sphere
      outResult.m_dist = Float_0;
      outResult.m_contactPt = ray.origin;
      outResult.m_surfaceNormal.setSub( ray.origin, origin ); 

      // <fastfloat.todo> selector
      if ( outResult.m_surfaceNormal.lengthSq() <= Float_0 )
      {
         // origins of ray and sphere overlap
         outResult.m_surfaceNormal = Vector_OX;
      }
      else
      {
         outResult.m_surfaceNormal.normalize();
      } 

      return true;
   }

   FastFloat t0, t1;
   if ( !solveQuadratic( a, b, c, t0, t1 ) || t1 < Float_0 ) 
   {
      // the sphere doesn't intersect the ray
      return false;
   }

   if ( t0 < Float_0 ) 
   {
      outResult.m_dist = t1;
   } 
   else 
   {
      outResult.m_dist = t0;
   }

   outResult.m_dist = t0;
   outResult.m_contactPt.setMulAdd( ray.direction, t0, ray.origin );
   outResult.m_surfaceNormal.setSub( outResult.m_contactPt, origin ); 
   outResult.m_surfaceNormal.normalize();
   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Sphere::isInside( const Vector& point ) const
{
   Vector to;
   to.setSub( origin, point );

   FastFloat radiusSq;
   radiusSq.setMul( radius, radius );

   return ( to.lengthSq() < radiusSq );
}

///////////////////////////////////////////////////////////////////////////////

bool Sphere::testIntersection( const AxisAlignedBox& aabb, IntersectionResult& outResult ) const
{
   Vector relCenter, absRelCenter, boxHalfExtents;
   aabb.getCenter( relCenter );
   relCenter.setSub( origin, relCenter );
   absRelCenter.setAbs( relCenter ); 

   aabb.getExtents( boxHalfExtents );
   boxHalfExtents.mul( Float_Inv2 );

   // Early-out check to see if we can exclude the contact.
   if ( absRelCenter.getComponent(0) - radius > boxHalfExtents.getComponent(0) ||
        absRelCenter.getComponent(1) - radius > boxHalfExtents.getComponent(1) ||
        absRelCenter.getComponent(2) - radius > boxHalfExtents.getComponent(2) )
   {
      return false;
   }

   Vector negBoxHalfExtents;
   FastFloat dist;
   negBoxHalfExtents.setMul( boxHalfExtents, Float_Minus1 );

   // Clamp each coordinate to the box.
   VectorComparison compResult;
   relCenter.greater( boxHalfExtents, compResult );
   outResult.m_contactPoint.setSelect( compResult, boxHalfExtents, relCenter );
   relCenter.less( negBoxHalfExtents, compResult );
   outResult.m_contactPoint.setSelect( compResult, negBoxHalfExtents, outResult.m_contactPoint );

   // Check we’re in contact.
   Vector displ;
   displ.setSub( outResult.m_contactPoint, relCenter );
   dist = displ.lengthSq();
   if ( dist > radius * radius ) 
   {
      return false;
   }

   outResult.m_contactNormal = Vector_OX;
   if ( dist > Float_0 )
   {
      outResult.m_contactNormal.setSub( origin, outResult.m_contactPoint );
      outResult.m_contactNormal.normalize();
   }

   dist.sqrt();
   outResult.m_penetrationDepth = radius - dist;
   return outResult.m_penetrationDepth > Float_1e_4;
}

///////////////////////////////////////////////////////////////////////////////

bool Sphere::testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const
{
   Vector to;
   to.setSub( origin, sphere.origin );
   const FastFloat inflatedRadius = radius + sphere.radius;

   const FastFloat distSq = to.lengthSq();
   if ( distSq > ( inflatedRadius * inflatedRadius ) )
   {
      return false;
   }

   outResult.m_penetrationDepth.setSqrt( distSq );
   outResult.m_contactNormal.setDiv( to, outResult.m_penetrationDepth ); // normalize the displacement vector to get the collision normal
   outResult.m_penetrationDepth = inflatedRadius - outResult.m_penetrationDepth; // penetrationDist = (sphere1.rad + sphere2.rad) - distanceBetweenSpheres
   outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, sphere.radius, sphere.origin );

   return outResult.m_penetrationDepth >= Float_1e_4;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const Sphere& sphere )
{
   stream << sphere.origin;
   stream << sphere.radius;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, Sphere& sphere )
{
   stream >> sphere.origin;
   stream >> sphere.radius;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
