#include "core\AxisAlignedBox.h"
#include "core\Assert.h"
#include "core\Matrix.h"
#include "core\Plane.h"
#include "core\RaycastResult.h"
#include "core\Ray.h"
#include "core\IntersectionResult.h"
#include "core\InStream.h"
#include "core\OutStream.h"


///////////////////////////////////////////////////////////////////////////////

AxisAlignedBox::AxisAlignedBox()
{
   min.set( FLT_MAX, FLT_MAX, FLT_MAX );
   max.set( -FLT_MAX, -FLT_MAX, -FLT_MAX );
}

///////////////////////////////////////////////////////////////////////////////

AxisAlignedBox::AxisAlignedBox( const Vector& _min, const Vector& _max )
{
   min = _min;
   max = _max;
}

///////////////////////////////////////////////////////////////////////////////

AxisAlignedBox::AxisAlignedBox( const AxisAlignedBox& rhs )
   : min( rhs.min )
   , max( rhs.max )
{
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::set( const Vector& _min, const Vector& _max )
{
   min = _min;
   max = _max;
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::setExpanded( const AxisAlignedBox& bounds, const FastFloat& multiplier )
{
   Vector center;
   getCenter( center );
   
   Vector a, b;
   a.setSub( min, center );
   b.setSub( max, center );

   min.setMulAdd( a, multiplier, center );
   max.setMulAdd( b, multiplier, center );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::add( const AxisAlignedBox& otherBox, AxisAlignedBox& unionBox ) const
{
   unionBox.min.setMin( otherBox.min, min );
   unionBox.max.setMax( otherBox.max, max );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::intersection( const AxisAlignedBox& otherBox, AxisAlignedBox& intersectionBox ) const
{
   // do the boxes even intersect?
   if ( testIntersection( otherBox ) )
   {
      // they do
      intersectionBox.min.setMax( otherBox.min, min );
      intersectionBox.max.setMin( otherBox.max, max );
   }
   else
   {
      // no intersection
      intersectionBox.reset();
   }
}

///////////////////////////////////////////////////////////////////////////////

float AxisAlignedBox::calcArea() const
{
   // check that the box is not degenerate
   VectorComparison comp;
   min.less( max, comp );
   if ( !comp.areAllSet< VectorComparison::MASK_XY >() )
   {
      return 0.f;
   }

   Vector extents;
   extents.setSub( max, min );

   Vector diff;
   diff.setSub( max, min );

   return diff[0] * diff[1];
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::getExtents( Vector& outExtents ) const
{
   outExtents.setSub( max, min );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::getCenter( Vector& outCenter ) const
{
   outCenter.setAdd( min, max );
   outCenter.mul( Float_Inv2 );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::getVertices( Vector* outArrVertices ) const
{
   outArrVertices[0] = min;
   outArrVertices[1].setSelect< VectorComparison::MASK_X >( min, max );
   outArrVertices[2].setSelect< VectorComparison::MASK_Y >( min, max );
   outArrVertices[3].setSelect< VectorComparison::MASK_Z >( min, max );
   outArrVertices[4].setSelect< VectorComparison::MASK_YZ >( min, max );
   outArrVertices[5].setSelect< VectorComparison::MASK_XZ >( min, max );
   outArrVertices[6].setSelect< VectorComparison::MASK_XY >( min, max );
   outArrVertices[7] = max;
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::getEdges( uint* outArrEdgeIndices )
{
   outArrEdgeIndices[0] = 0; outArrEdgeIndices[1] = 4;
   outArrEdgeIndices[2] = 4; outArrEdgeIndices[3] = 2;
   outArrEdgeIndices[4] = 2; outArrEdgeIndices[5] = 6;
   outArrEdgeIndices[6] = 6; outArrEdgeIndices[7] = 0;
   outArrEdgeIndices[8] = 5; outArrEdgeIndices[9] = 3;
   outArrEdgeIndices[10] = 3; outArrEdgeIndices[11] = 7;
   outArrEdgeIndices[12] = 7; outArrEdgeIndices[13] = 1;
   outArrEdgeIndices[14] = 1; outArrEdgeIndices[15] = 5;
   outArrEdgeIndices[16] = 5; outArrEdgeIndices[17] = 0;
   outArrEdgeIndices[18] = 3; outArrEdgeIndices[19] = 4;
   outArrEdgeIndices[20] = 7; outArrEdgeIndices[21] = 2;
   outArrEdgeIndices[22] = 1; outArrEdgeIndices[23] = 6;
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::reset()
{
   min.setBroadcast( Float_INF );
   max.setBroadcast( Float_NegINF );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::transform( const Matrix& mtx, AxisAlignedBox& transformedBox ) const
{
   // find extreme points by considering product of 
   // min and max with each component of mtx.
   float a, b;
   ALIGN_16 float tMin[4];
   ALIGN_16 float tMax[4];

   // begin at transform position
   const Vector& newCenter = mtx.position();
   newCenter.store( tMin );
   newCenter.store( tMax );
   for( uint j = 0; j < 3; ++j )
   {
      for( uint i = 0; i < 3; ++i )
      {
         a = mtx( i, j ) * min[i];
         b = mtx( i, j ) * max[i];

         if( a < b )
         {
            tMin[j] += a;
            tMax[j] += b;
         }
         else
         {
            tMin[j] += b;
            tMax[j] += a;
         }
      }
   }

   transformedBox.min.set( tMin );
   transformedBox.max.set( tMax );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat AxisAlignedBox::distanceToPlane( const Plane& plane ) const
{
   Vector planeNormal;
   plane.getNormal( planeNormal );

   Vector nearPoint, farPoint;

   VectorComparison planeNormalGZ;
   planeNormal.greater( Quad_0, planeNormalGZ );

   farPoint.setSelect( planeNormalGZ, max, min );
   nearPoint.setSelect( planeNormalGZ, min, max );

   if ( plane.dotCoord( nearPoint ) > Float_0 )
   {
      // the box is in front
      return Float_1;
   }
   if ( plane.dotCoord( farPoint ) >= Float_0 )
   {
      return Float_0;
   }
   else
   {
      // the box is behind
      return Float_Minus1;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::rayCast( const Ray& ray, RaycastResult& outResult ) const
{
   // 'slabs' algorithm
   FastFloat tMin; tMin.setNeg( Float_INF );
   FastFloat tMax = Float_INF;
   FastFloat t1, t2, fTemp;

   Vector extentsMin;
   extentsMin.setSub( min, ray.origin );
   Vector extentsMax;
   extentsMax.setSub( max, ray.origin );

   Vector axes[] = { Vector_NEG_OX, Vector_NEG_OY, Vector_NEG_OZ };

   // <fastfloat.todo> this should also be feasible to do using selectors
   FastFloat dir, dirAbs, dirComp;
   Vector candidateNormal;
   for (int i = 0; i < 3; ++i)
   {
      dirComp = ray.direction.getComponent( i );
      dirAbs.setAbs( dirComp );
      if ( dirAbs > Float_1e_3 )
      {
         fTemp.setReciprocal( dirComp );
         t1.setMul( extentsMax.getComponent( i ), fTemp );
         t2.setMul( extentsMin.getComponent( i ), fTemp );

         candidateNormal = axes[i];

         if (t1 > t2) 
         {
            fTemp = t1; 
            t1 = t2; 
            t2 = fTemp; 

            candidateNormal.neg();
         }

         if ( t1 > tMin ) 
         {
            tMin = t1;
            outResult.m_surfaceNormal = candidateNormal;
         }
         if ( t2 < tMax )
         {
            tMax = t2;
         }

         if ( tMin > tMax )
         {
            return false;
         }
         if ( tMax < Float_0 )
         {
            return false;
         }
      }
      else
      {
         if ( ( ray.origin[i] < min[i] ) || ( ray.origin[i] > max[i] ) )
         {
            return false;
         }
      }
   }

   if ( tMin < Float_0 )
   {
      tMin = Float_0;
   }

   outResult.m_dist = tMin;
   outResult.m_contactPt.setMulAdd( ray.direction, tMin, ray.origin );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::isInside( const Vector& point, IntersectionResult& outResult ) const
{
   Vector absPoint;
   absPoint.setAbs( point );

   Vector halfSize;
   halfSize.setSub( max, min );
   halfSize.mul( Float_Inv2 );

   Vector penetrationDistances;
   penetrationDistances.setSub( halfSize, absPoint );

   outResult.m_penetrationDepth = penetrationDistances.getMin<3>();
   if ( outResult.m_penetrationDepth < Float_0 )
   {
      return false;
   }

   // select the lowest penetration depth
   VectorComparison minDistIdx;
   penetrationDistances.selectMin<3>( minDistIdx );

   // calculate contact normal
   outResult.m_contactNormal.setSelect( minDistIdx, Vector_ONE, Vector_ZERO );

   // is the point in the rear of the box ( behind the origin ) or in its front - 'cause if it's in the back,
   // then we need to flip the direction of normal
   Vector boxCenter;
   boxCenter.setAdd( min, max );
   boxCenter.mul( Float_Inv2 );
   
   // flip the direction of the normal if the point lies 'behind' the origin in that particular direction
   VectorComparison isPointBehindBoxCenter;
   point.less( boxCenter, isPointBehindBoxCenter );

   isPointBehindBoxCenter.setAnd( minDistIdx, isPointBehindBoxCenter );
   outResult.m_contactNormal.mul( isPointBehindBoxCenter.isAnySet< VectorComparison::MASK_XYZ >() ? Float_Minus1 : Float_1 );

   // calculate contact point
   outResult.m_contactPoint.setMul( outResult.m_contactNormal, halfSize );
   outResult.m_contactPoint.add( boxCenter );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::calcContactPoint( const Vector& point, const Vector& contactAxis, IntersectionResult& outResult ) const
{
   Vector absPoint;
   absPoint.setAbs( point );

   Vector halfSize;
   halfSize.setSub( max, min );
   halfSize.mul( Float_Inv2 );

   Vector penetrationDistances;
   penetrationDistances.setSub( halfSize, absPoint );

   outResult.m_penetrationDepth = penetrationDistances.getMin<3>();
   if ( outResult.m_penetrationDepth < Float_0 )
   {
      return false;
   }

   Vector displToMin, displToMax;
   displToMin.setSub( point, min );
   displToMax.setSub( max, point );

   FastFloat distToMin, distToMax;
   distToMin.setAbs( contactAxis.dot( displToMin ) );
   distToMax.setAbs( contactAxis.dot( displToMax ) );

   if ( distToMin < distToMax )
   {
      outResult.m_contactNormal.setMul( contactAxis, Float_Minus1 );
      outResult.m_penetrationDepth = distToMin;
   }
   else
   {
      outResult.m_contactNormal = contactAxis;
      outResult.m_penetrationDepth = distToMax;
   }

   outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, outResult.m_penetrationDepth, point );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::testIntersection( const AxisAlignedBox& aabb ) const
{
   VectorComparison c1, c2;
   max.less( aabb.min, c1 );
   aabb.max.less( min, c2 );
   c1.setOr( c1, c2 );

   bool result = c1.areAllClear< VectorComparison::MASK_XYZ >();
   return result;
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::findIntersectionRemovalVector( const AxisAlignedBox& aabb, Vector& outRemovalVec ) const
{
   Vector v1, v2;
   v1.setSub( min, aabb.max );
   v2.setSub( max, aabb.min );

   VectorComparison c1, c2, c3;
   min.lessEqual( aabb.max, c1 );
   max.greaterEqual( aabb.max, c2 );
   c1.setAnd( c1, c2 );

   min.lessEqual( aabb.min, c2 );
   max.greaterEqual( aabb.min, c3 );
   c2.setAnd( c2, c3 );

   Vector tmpV;
   tmpV.setSelect( c2, v2, Quad_0 );
   outRemovalVec.setSelect( c1, v1, tmpV );
}

///////////////////////////////////////////////////////////////////////////////

void AxisAlignedBox::include( const Vector& pt )
{
   min.setMin( pt, min );
   max.setMax( pt, max );
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::operator==( const AxisAlignedBox& rhs ) const
{
   return ( min == rhs.min ) && ( max == rhs.max );
}

///////////////////////////////////////////////////////////////////////////////

bool AxisAlignedBox::operator!=( const AxisAlignedBox& rhs ) const
{
   return ( min != rhs.min ) || ( max != rhs.max );
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const AxisAlignedBox& box )
{
   stream << box.min;
   stream << box.max;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, AxisAlignedBox& box )
{
   stream >> box.min;
   stream >> box.max;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
