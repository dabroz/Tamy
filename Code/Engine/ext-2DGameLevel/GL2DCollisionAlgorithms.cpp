#include "ext-2DGameLevel\GL2DCollisionAlgorithms.h"
#include "ext-2DGameLevel\GL2DCircleShape.h"
#include "ext-2DGameLevel\GL2DBoxShape.h"
#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionAlgorithms::circleVsCircle( const GL2DCircleShape& shapeA, const GL2DCircleShape& shapeB, const Matrix& shapeATransform, const Matrix& shapeBTransform, IntersectionResult& outResult )
{
   Vector to;
   to.setSub( shapeBTransform.position( ), shapeATransform.position( ) );
   const FastFloat radiusA = shapeB.getRadius( );

   const FastFloat inflatedRadius = radiusA + shapeB.getRadius( );
      
   const FastFloat distSq = to.lengthSq( );
   if ( distSq > ( inflatedRadius * inflatedRadius ) )
   {
      return false;
   }

   outResult.m_penetrationDepth.setSqrt( distSq );
   outResult.m_contactNormal.setDiv( to, outResult.m_penetrationDepth ); // normalize the displacement vector to get the collision normal
   outResult.m_penetrationDepth = inflatedRadius - outResult.m_penetrationDepth; // penetrationDist = (sphere1.rad + sphere2.rad) - distanceBetweenSpheres
   outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, radiusA, shapeATransform.position( ) );

   return outResult.m_penetrationDepth > Float_0;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionAlgorithms::circleVsBox( const GL2DCircleShape& shapeA, const GL2DBoxShape& shapeB, const Matrix& shapeATransform, const Matrix& shapeBTransform, IntersectionResult& outResult )
{
   // build a bounding sphere used to query for an area of static geometry
   Sphere boundingSphere;
   shapeA.getBoundingSphere( boundingSphere );
   boundingSphere.transform( shapeATransform, boundingSphere );

   // query the static geometry for some line segments
   const GL2DDirectedLineSegment* segments = shapeB.getSegments();
   GL2DDirectedLineSegment transformedSegment;
   IntersectionResult tmpResult;
   outResult.m_penetrationDepth = Float_0;
   for ( uint i = 0; i < 4; ++i )
   {
      transformedSegment.setTransformed( shapeBTransform, segments[i] );
      if ( !circleVsLineSegment( shapeA, shapeATransform, transformedSegment, tmpResult ) )
      {
         continue;
      }

      // find the result with the largest penetration depth
      if ( tmpResult.m_penetrationDepth > outResult.m_penetrationDepth )
      {
         outResult = tmpResult;
      }
   }

   return outResult.m_penetrationDepth > Float_0;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DCollisionAlgorithms::circleVsLineSegments( const GL2DCircleShape& shapeA, const GL2DLineSegmentsShape& shapeB, const Matrix& circleTransform, IntersectionResult* outArrResults )
{
   // build a bounding sphere used to query for an area of static geometry
   Sphere boundingSphere;
   shapeA.getBoundingSphere( boundingSphere );
   boundingSphere.transform( circleTransform, boundingSphere );

   // query the static geometry for some line segments
   Array< const GL2DDirectedLineSegment* > segments( 32 );
   shapeB.getSegments( boundingSphere, segments );
   uint count = segments.size();
   uint resultIdx = 0;
   for ( uint i = 0; i < count && resultIdx < MAX_INTERSECTION_RESULTS; ++i )
   {
      const GL2DDirectedLineSegment& segment = *segments[i];
      if ( circleVsLineSegment( shapeA, circleTransform, segment, outArrResults[resultIdx] ) )
      {
         ++resultIdx;
      }
   }

   return resultIdx;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionAlgorithms::circleVsLineSegment( const GL2DCircleShape& shape, const Matrix& circleTransform, const GL2DDirectedLineSegment& segment, IntersectionResult& outResult )
{
   const Vector& circleOrigin = circleTransform.position();
   Vector a;
   a.setSub( circleOrigin, segment.m_start );

   // is it on the front side of the segment
   FastFloat distFromSegment = a.dot( segment.m_normal );
   const FastFloat radius = shape.getRadius();
   if ( distFromSegment < -radius || distFromSegment > radius )
   {
      // the circle doesn't intersect the segment
      return false;
   }

   FastFloat x = segment.m_direction.dot( a );
   if ( x < Float_0 )
   {
      // the circle is intersecting the start of the segment 
      outResult.m_contactPoint = segment.m_start;
   }
   else if ( x > segment.m_length )
   {
      // the circle is intersecting the end of the segment
      outResult.m_contactPoint = segment.m_end;
   }
   else
   {
      // the circle is intersecting the middle of the segment
      outResult.m_contactPoint.setMulAdd( segment.m_direction, x, segment.m_start );
   }

   outResult.m_contactNormal.setSub( circleOrigin, outResult.m_contactPoint );

   outResult.m_penetrationDepth = outResult.m_contactNormal.length();
   outResult.m_contactNormal.div( outResult.m_penetrationDepth );

   outResult.m_penetrationDepth = radius - outResult.m_penetrationDepth;

   return outResult.m_penetrationDepth > Float_0;
}

///////////////////////////////////////////////////////////////////////////////
