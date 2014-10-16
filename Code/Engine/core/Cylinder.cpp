#include "core\Cylinder.h"
#include "core\Plane.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\AxisAlignedBox.h"
#include "core\RaycastResult.h"
#include "core\Ray.h"
#include "core\IntersectionResult.h"
#include "core\Sphere.h"


///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder()
   : m_radius( Float_0 )
   , m_height( Float_0 )
{
   m_bottomBaseOrigin;
   Vector               m_cylinderDir;
   FastFloat            m_radius;
   FastFloat            m_height;
}

///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder( const Vector& bottomBase, const Vector& topBase, float radius )
   : m_bottomBaseOrigin( bottomBase )
{
   m_radius.setFromFloat( radius );

   m_cylinderDir.setSub( topBase, bottomBase );
   m_height = m_cylinderDir.length();
   m_cylinderDir.div( m_height );
}

///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder( const Cylinder& rhs )
   : m_bottomBaseOrigin( rhs.m_bottomBaseOrigin )
   , m_cylinderDir( rhs.m_cylinderDir )
   , m_height( rhs.m_height )
   , m_radius( rhs.m_radius )
{
}

///////////////////////////////////////////////////////////////////////////////

void Cylinder::set( const Vector& bottomBase, const Vector& topBase, float radius )
{
   m_bottomBaseOrigin = bottomBase;
   m_radius.setFromFloat( radius );

   m_cylinderDir.setSub( topBase, bottomBase );
   m_height = m_cylinderDir.length();
   m_cylinderDir.div( m_height );
}

///////////////////////////////////////////////////////////////////////////////

void Cylinder::set( const Vector& bottomBase, const Vector& direction, float radius, float height )
{
   m_bottomBaseOrigin = bottomBase;
   m_cylinderDir.setNormalized( direction );
   m_radius.setFromFloat( radius );
   m_height.setFromFloat( height );
}

///////////////////////////////////////////////////////////////////////////////

void Cylinder::transform( const Matrix& mtx, Cylinder& transformedCylinder ) const
{
   mtx.transform( m_bottomBaseOrigin, transformedCylinder.m_bottomBaseOrigin );
   mtx.transformNorm( m_cylinderDir, transformedCylinder.m_cylinderDir );
   transformedCylinder.m_radius = m_radius;
   transformedCylinder.m_height = m_height;
}

///////////////////////////////////////////////////////////////////////////////

void Cylinder::calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const
{
   Vector w; w.set( m_radius, Float_0, Float_0 );
   Vector h; h.set( Float_0, m_radius, Float_0 );

   // define the bounding box as if the cylinder was lying on its side, it's hub aligned with the Z axis
   outBoundingBox.min.set( -m_radius, -m_radius, Float_0 );
   outBoundingBox.max.set(  m_radius,  m_radius, m_height );

   // rotate the box so that it matches cylinder's orientation
   Quaternion cylOrient;
   cylOrient.setFromShortestRotation( Vector_OZ, m_cylinderDir );

   Matrix cylOrientMtx;
   cylOrientMtx.setRotation( cylOrient );
   cylOrientMtx.setPosition<3>( m_bottomBaseOrigin );

   outBoundingBox.transform( cylOrientMtx, outBoundingBox );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Cylinder::distanceToPlane( const Plane& plane ) const
{
   // TODO:
   return Float_0;
}

///////////////////////////////////////////////////////////////////////////////

bool Cylinder::isInside( const Vector& point ) const
{
   Vector disp;
   disp.setSub( point, m_bottomBaseOrigin );
   const FastFloat dist = m_cylinderDir.dot( disp );
   if ( dist < Float_0 || dist >m_height )
   {
      return false;
   }

   Vector PO;
   PO.setSub( point, m_bottomBaseOrigin );

   Vector POxD;
   POxD.setCross( PO, m_cylinderDir );

   const FastFloat distFromCoreToPointSq = POxD.dot( POxD );
   const FastFloat rSq = m_radius * m_radius;
   return distFromCoreToPointSq <= rSq;
}

///////////////////////////////////////////////////////////////////////////////

bool Cylinder::rayCast( const Ray& ray, RaycastResult& outResult ) const
{
   Vector O;
   O.setSub( m_bottomBaseOrigin, ray.origin );

   Vector VxD, OxD;
   VxD.setCross( ray.direction, m_cylinderDir );
   OxD.setCross( O, m_cylinderDir );

   const FastFloat a = VxD.dot( VxD );
   const FastFloat b = VxD.dot( OxD ) * Float_Minus2;
   const FastFloat c = OxD.dot( OxD ) - ( m_radius * m_radius );

   FastFloat t0, t1;
   bool solutionExists = solveQuadratic( a, b, c, t0, t1 );
   if ( !solutionExists || t1 < Float_0 )
   {
      // that's it - the ray doesn't intersect the infinite cylinder, so it doesn't have a change to intersect a segment of it
      return false;
   }

   FastFloat distToFirstIntersection;
   if ( t0 < Float_0 ) 
   {
      distToFirstIntersection = t1;
   } 
   else 
   {
      distToFirstIntersection = t0;
   }

   Vector contactPt0;
   contactPt0.setMulAdd( ray.direction, distToFirstIntersection, ray.origin );

   Vector B;
   B.setSub( contactPt0, m_bottomBaseOrigin );
   const FastFloat d0 = m_cylinderDir.dot( B );

   if ( d0 > Float_0 && d0 < m_height )
   {
      // the ray intersects cylinder's body

      // set the contact point and the distance along the ray
      outResult.m_contactPt = contactPt0;
      outResult.m_dist = distToFirstIntersection;

      // calculate a surface normal
      Vector corePointProj;
      corePointProj.setMulAdd( m_cylinderDir, d0, m_bottomBaseOrigin );
      outResult.m_surfaceNormal.setSub( outResult.m_contactPt, corePointProj );
      outResult.m_surfaceNormal.normalize();

      return true;
   }


   // the cylinder may intersect one of the bases
   Vector contactPt1;
   contactPt1.setMulAdd( ray.direction, t1, ray.origin );
   B.setSub( contactPt1, m_bottomBaseOrigin );
   const FastFloat d1 = m_cylinderDir.dot( B );

   const FastFloat bottomBaseSignDiff = d0 * d1;
   const FastFloat topBaseSignDiff = ( d0 - m_height ) * ( d1 - m_height );

   if ( bottomBaseSignDiff <= Float_0 )
   {
      // ray intersects the bottom base
      Vector bottomBaseNormal;
      bottomBaseNormal.setMul( m_cylinderDir, Float_Minus1 );

      Plane bottomBase; 
      bottomBase.setFromPointNormal( m_bottomBaseOrigin, bottomBaseNormal );
      ray.rayCast( bottomBase, outResult );

      return true;
   }
   else if ( topBaseSignDiff <= Float_0 ) 
   {
      Vector topBaseOrigin;
      topBaseOrigin.setMulAdd( m_cylinderDir, m_height, m_bottomBaseOrigin );

      Plane topBase; 
      topBase.setFromPointNormal( topBaseOrigin, m_cylinderDir );
      ray.rayCast( topBase, outResult );

      return true;
   }

   ASSERT_MSG( false, "Ray-cylinder intersection point is undefined" );
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool Cylinder::testIntersection( const Sphere& sphere, IntersectionResult& outResult )  const
{
   Vector PO;
   PO.setSub( sphere.origin, m_bottomBaseOrigin );

   const FastFloat distAlongCoreToP = m_cylinderDir.dot( PO );

   // check if the point lies beyond cylinder bases
   if ( ( ( distAlongCoreToP + sphere.radius ) < Float_0 ) || ( ( distAlongCoreToP - sphere.radius ) > m_height ) )
   {
      // the sphere is located beyond cylinder bases
      return false;
   }

   if ( distAlongCoreToP <= Float_0 )
   {
      // the sphere collides with the bottom base
      outResult.m_contactNormal.setMul( m_cylinderDir, Float_Minus1 );
      outResult.m_penetrationDepth = sphere.radius + distAlongCoreToP;
      outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, distAlongCoreToP, sphere.origin );
      return true;
   }

   if ( distAlongCoreToP >= m_height )
   {
      // the sphere collides with the top base
      outResult.m_contactNormal = m_cylinderDir;
      outResult.m_penetrationDepth = sphere.radius - ( distAlongCoreToP - m_height );
      outResult.m_contactPoint.setMulAdd( m_cylinderDir, -outResult.m_penetrationDepth, sphere.origin );
      return true;
   }

   // the sphere collides with the cylinder's side
   Vector projectedSphereOrigin;
   projectedSphereOrigin.setMulAdd( m_cylinderDir, distAlongCoreToP, m_bottomBaseOrigin );

   Vector D;
   D.setSub( sphere.origin, projectedSphereOrigin );

   const FastFloat joinedRadius = sphere.radius + m_radius;
   const FastFloat maxIntersectionRangeSq = joinedRadius * joinedRadius;
   const FastFloat DLenSq = D.lengthSq();
   if ( DLenSq > maxIntersectionRangeSq )
   {
      // the sphere lies far away from the cylinder
      return false; 
   }

   if ( DLenSq <= Float_0 )
   {
      // sphere's origin overlaps with the cylinder's core
      outResult.m_penetrationDepth = Float_0;
      outResult.m_contactNormal = Vector_OX;
   }
   else
   {
      outResult.m_penetrationDepth.setSqrt( DLenSq );
      outResult.m_penetrationDepth = joinedRadius - outResult.m_penetrationDepth;
      outResult.m_contactNormal.setNormalized( D );
   }
   outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, m_radius, projectedSphereOrigin );

   return true;
}

///////////////////////////////////////////////////////////////////////////////
