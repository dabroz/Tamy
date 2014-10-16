#include "core\Box.h"
#include "core\RaycastResult.h"
#include "core\Ray.h"
#include "core\Sphere.h"
#include "core\IntersectionResult.h"


///////////////////////////////////////////////////////////////////////////////

Box::Box()
{
}

///////////////////////////////////////////////////////////////////////////////

Box::Box( const Vector& min, const Vector& max, const Matrix& transform )
   : m_axisAlignedBox( min, max )
   , m_localToWorld( transform )
{
   m_worldToLocal.setInverse( transform );
}

///////////////////////////////////////////////////////////////////////////////

Box::Box( const Vector& extents, const Matrix& transform )
{
   m_axisAlignedBox.max.setMul( extents, Float_Inv2 );
   m_axisAlignedBox.min.setMul( m_axisAlignedBox.max, Float_Minus1 );

   m_localToWorld = transform;
   m_worldToLocal.setInverse( transform );
}

///////////////////////////////////////////////////////////////////////////////

void Box::set( const Vector& min, const Vector& max, const Matrix& transform )
{
   m_axisAlignedBox.set( min, max );
   m_localToWorld = transform;
   m_worldToLocal.setInverse( transform );
}

///////////////////////////////////////////////////////////////////////////////

void Box::set( const Vector& extents, const Matrix& transform )
{
   m_axisAlignedBox.max.setMul( extents, Float_Inv2 );
   m_axisAlignedBox.min.setMul( m_axisAlignedBox.max, Float_Minus1 );

   m_localToWorld = transform;
   m_worldToLocal.setInverse( transform );
}

///////////////////////////////////////////////////////////////////////////////

Box::Box( const Box& rhs )
   : m_axisAlignedBox( rhs.m_axisAlignedBox )
   , m_localToWorld( rhs.m_localToWorld )
   , m_worldToLocal( rhs.m_worldToLocal )
{
}

///////////////////////////////////////////////////////////////////////////////

void Box::transform( const Matrix& mtx, Box& transformedBox ) const
{
   transformedBox.m_axisAlignedBox = m_axisAlignedBox;

   transformedBox.m_localToWorld.setMul( m_localToWorld, mtx );
   transformedBox.m_worldToLocal.setInverse( transformedBox.m_localToWorld );
}

///////////////////////////////////////////////////////////////////////////////

bool Box::rayCast( const Ray& ray, RaycastResult& outResult ) const
{
   // transform ray to the box's local space
   Ray rayLocalSpace;
   ray.transform( m_worldToLocal, rayLocalSpace );

   return m_axisAlignedBox.rayCast( rayLocalSpace, outResult );
}

///////////////////////////////////////////////////////////////////////////////

bool Box::testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const
{
   Sphere sphereLS;
   sphereLS.radius = sphere.radius;

   m_worldToLocal.transform( sphere.origin, sphereLS.origin );

   IntersectionResult contactPointLocalSpace;
   bool result = sphereLS.testIntersection( m_axisAlignedBox, contactPointLocalSpace );
   if ( result )
   {
      // transform the collision point back to world space      
      m_localToWorld.transform( contactPointLocalSpace.m_contactPoint, outResult.m_contactPoint );
      m_localToWorld.transformNorm( contactPointLocalSpace.m_contactNormal, outResult.m_contactNormal );
      outResult.m_penetrationDepth = contactPointLocalSpace.m_penetrationDepth;
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

uint Box::testIntersection( const Box& box, IntersectionResult* outArrContactPoints, uint maxContactPoints ) const
{
   // get vertices of both boxes
   Vector aHalfExtents;
   m_axisAlignedBox.getExtents( aHalfExtents );
   aHalfExtents.mul( Float_Inv2 );

   Matrix transAToB, transBToA;
   transBToA.setMul( box.m_localToWorld, m_worldToLocal );
   transAToB.setMul( m_localToWorld, box.m_worldToLocal );

   // get vertices of both boxes
   Vector boxAVertices[8];
   Vector boxBVertices[8];
   m_axisAlignedBox.getVertices( boxAVertices );
   box.m_axisAlignedBox.getVertices( boxBVertices );

   // all axis aligned boxes return the same config of edges that matches the configuration of vertices returned by getVertices method
   uint boxEdges[24];
   AxisAlignedBox::getEdges( boxEdges );

   // transform vertices of B to this box's reference space
   for ( int i = 0; i < 8; ++i )
   {
      transBToA.transform( boxBVertices[i], boxBVertices[i] );
   }

   // calculate candidate separation axes
   Vector axes[6] = { Vector_OX, Vector_OY, Vector_OZ, Vector_OX, Vector_OY, Vector_OZ };
   for ( int i = 3; i < 6; ++i )
   {
      transBToA.transformNorm( axes[i], axes[i] );
   }

   // calculate penetration along each of the axes
   FastFloat minExtA, maxExtA, minExtB, maxExtB, d, penetrationDepth;
   Vector tmp;
   Vector vectorsDispl;
   vectorsDispl.setSub( box.m_localToWorld.position( ), m_localToWorld.position( ) );
 
   struct GenericContact
   {
      Vector      m_axis;
      FastFloat   m_penetrationDepth;
      FastFloat   m_minExtA;
      FastFloat   m_maxExtA;
      FastFloat   m_minExtB;
      FastFloat   m_maxExtB;
   };

   GenericContact genericContact;
   genericContact.m_penetrationDepth = Float_INF;

   for ( int i = 0; i < 6; ++i )
   {
      // find minimum and maximum extents for both boxes
      minExtA = Float_INF;
      maxExtA = -Float_INF;        
      minExtB = Float_INF;
      maxExtB = -Float_INF;
      

      const Vector& axis = axes[i];
      for ( int j = 0; j < 8; ++j )
      {
         // check box A
         {
            d = axis.dot( boxAVertices[j] );
            if ( d < minExtA )
            {
               minExtA = d;
            }
            if ( d > maxExtA )
            {
               maxExtA = d;
            }
         }

         // check box B
         {
            d = axis.dot( boxBVertices[j] );
            if ( d < minExtB )
            {
               minExtB = d;
            }
            if ( d > maxExtB )
            {
               maxExtB = d;
            }
         }
      }

      if ( maxExtA < minExtB || maxExtB < minExtA )
      {
         // the two boxes don't collide
         return 0;
      }

      // calculate the penetration dist
      const FastFloat pd0 = maxExtA - minExtB;
      const FastFloat pd1 = maxExtB - minExtA;
      penetrationDepth.setMin( pd0, pd1 );

      if ( genericContact.m_penetrationDepth > penetrationDepth )
      {
         genericContact.m_penetrationDepth = penetrationDepth;
         genericContact.m_axis = axis;
         genericContact.m_minExtA = minExtA;
         genericContact.m_maxExtA = maxExtA;
         genericContact.m_minExtB = minExtB;
         genericContact.m_maxExtB = maxExtB;
      }
   }

   // If we got this far, it means that the two boxes are colliding and we found an axis
   // which ensures the shortest way to undo the collision.
   // Now we need to generate contact points - and this is a bit tricky.
   // Boxes may have penetrating vertices, but they can also be touching with edges only - no
   // vertex intersection involved.
   // The first one is easy to determine, however if we ended up here ( so there's definitely a collision there ),
   // and we still haven't found any intersecting vertices, we'll assume that it's the edges that intersect, and we're gonna
   // generate a single, generic contact point and apply it to the center of the box.
   uint contactPointsCount = 0;
   IntersectionResult tmpContactPoint;
   for ( int i = 0; i < 8 && contactPointsCount < maxContactPoints; ++i )
   {
      // is a point from box A inside box B
      {
         const Vector& point = boxAVertices[i];
         Vector transformedPoint, transformedAxis;
         transAToB.transform( point, transformedPoint );
         transAToB.transformNorm( genericContact.m_axis, transformedAxis );
         if ( box.m_axisAlignedBox.calcContactPoint( transformedPoint, transformedAxis, tmpContactPoint ) )
         {
            box.m_localToWorld.transformNorm( tmpContactPoint.m_contactNormal, tmpContactPoint.m_contactNormal );
            box.m_localToWorld.transform( transformedPoint, tmpContactPoint.m_contactPoint );

            addUniqueContactPoint( tmpContactPoint, outArrContactPoints, maxContactPoints, contactPointsCount );
         }
      }

      // is a point from box A inside box B
      if ( contactPointsCount < maxContactPoints )
      {
         const Vector& transformedPoint = boxBVertices[i];
         if ( m_axisAlignedBox.calcContactPoint( transformedPoint, genericContact.m_axis, tmpContactPoint ) )
         {
            m_localToWorld.transformNorm( tmpContactPoint.m_contactNormal, tmpContactPoint.m_contactNormal );
            tmpContactPoint.m_contactNormal.mul( Float_Minus1 );
            m_localToWorld.transform( tmpContactPoint.m_contactPoint, tmpContactPoint.m_contactPoint );

            addUniqueContactPoint( tmpContactPoint, outArrContactPoints, maxContactPoints, contactPointsCount );
         }
      }
   }

   if ( contactPointsCount > 0 )
   {
      // we found vertex collisions - that's it
      return contactPointsCount;
   }

   // 

   // We haven't found any intersecting vertices - this means we're dealing with an edge-to-edge, or a face-to-face
   // contact.

   // We need to determine which edges are intersecting the boxes and calculate intersection points
   uint intersectingBoxAEdges[4];
   uint intersectingBoxBEdges[4];
   uint intersectingBoxAEdgesCount = 0;
   uint intersectingBoxBEdgesCount = 0;
   {
      Ray ray;
      uint firstVtxIdx, lastVtxIdx;
      RaycastResult raycastResult;
      Vector segmentDispl;

      // test edges of box A against box B
      for ( uint i = 0; i < 12; ++i )
      {
         uint edgeStartIdx = i * 2;
         firstVtxIdx = boxEdges[edgeStartIdx];
         lastVtxIdx = boxEdges[edgeStartIdx + 1];

         segmentDispl.setSub( boxAVertices[lastVtxIdx], boxAVertices[firstVtxIdx] );
         const FastFloat segmentLen = segmentDispl.length();
 
         ray.origin = boxAVertices[firstVtxIdx];
         ray.direction.setDiv( segmentDispl, segmentLen );
         ray.transform( transAToB, ray );

         if ( ray.rayCast( box.m_axisAlignedBox, raycastResult ) && raycastResult.m_dist > Float_0 && raycastResult.m_dist < segmentLen )
         {
            // is the end point closer than the returned distance - 'cause if so, then there's no collision with the line segment
            ASSERT_MSG( intersectingBoxAEdgesCount < 4, "Not more than 4 edge contact points should be generated for a box" );
            intersectingBoxAEdges[intersectingBoxAEdgesCount++] = edgeStartIdx;
         }
      }

      // and now - edges of B against box A
      for ( uint i = 0; i < 12; ++i )
      {
         uint edgeStartIdx = i * 2;
         firstVtxIdx = boxEdges[edgeStartIdx];
         lastVtxIdx = boxEdges[edgeStartIdx + 1];

         segmentDispl.setSub( boxBVertices[lastVtxIdx], boxBVertices[firstVtxIdx] );
         const FastFloat segmentLen = segmentDispl.length( );

         ray.origin = boxBVertices[firstVtxIdx];
         ray.direction.setDiv( segmentDispl, segmentLen );

         if ( ray.rayCast( m_axisAlignedBox, raycastResult ) && raycastResult.m_dist > Float_0 && raycastResult.m_dist < segmentLen )
         {
            // is the end point closer than the returned distance - 'cause if so, then there's no collision with the line segment
            ASSERT_MSG( intersectingBoxBEdgesCount < 4, "Not more than 4 edge contact points should be generated for a box" );
            intersectingBoxBEdges[intersectingBoxBEdgesCount++] = edgeStartIdx;
         }
      }
   }

   // Generate contact points.
   // The vertices we're gonna be working with at this point need to be transformed to world space
   uint boxAFirstVtxIdx, boxALastVtxIdx, boxBFirstVtxIdx, boxBLastVtxIdx, boxAEdgeIdx, boxBEdgeIdx;
   Vector cpB, a1, a2, b1, b2;
   for ( int i = 0; i < intersectingBoxAEdgesCount; ++i )
   {
      boxAEdgeIdx = intersectingBoxAEdges[i];
      boxAFirstVtxIdx = boxEdges[boxAEdgeIdx];
      boxALastVtxIdx = boxEdges[boxAEdgeIdx + 1];

      m_localToWorld.transform( boxAVertices[boxAFirstVtxIdx], a1 );
      m_localToWorld.transform( boxAVertices[boxALastVtxIdx], a2 );

      for ( int j = 0; j < intersectingBoxBEdgesCount; ++j )
      {
         boxBEdgeIdx = intersectingBoxBEdges[j];
         boxBFirstVtxIdx = boxEdges[boxBEdgeIdx];
         boxBLastVtxIdx = boxEdges[boxBEdgeIdx + 1];

         // box's B vertices are already transformed to box's A space, so all we need to do to get 
         // them to World space is apply box's A's local->world transform
         m_localToWorld.transform( boxBVertices[boxBFirstVtxIdx], b1 );
         m_localToWorld.transform( boxBVertices[boxBLastVtxIdx], b2 );

         tmpContactPoint.m_penetrationDepth = closestPtSegmentSegment( a1, a2, b1, b2, tmpContactPoint.m_contactPoint, cpB );
         tmpContactPoint.m_penetrationDepth.sqrt();
         tmpContactPoint.m_contactNormal.setSub( cpB, tmpContactPoint.m_contactPoint );
         tmpContactPoint.m_contactNormal.div( tmpContactPoint.m_penetrationDepth );

         addUniqueContactPoint( tmpContactPoint, outArrContactPoints, maxContactPoints, contactPointsCount );
      }
   }

   
   return contactPointsCount;
}

///////////////////////////////////////////////////////////////////////////////

uint Box::testIntersection( const Triangle& triangle, IntersectionResult* outArrContactPoints, uint maxContactPoints ) const
{
   // TODO:
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

void Box::addUniqueContactPoint( const IntersectionResult& contactPoint, IntersectionResult* outArrContactPoints, uint maxContactPoints, uint& outContactPointsCount ) const
{
   if ( contactPoint.m_penetrationDepth < Float_1e_4 )
   {
      // this contact point is too shallow
      return;
   }

   // check for duplicates
   Vector a, b;
   FastFloat val;
   for ( uint i = 0; i < outContactPointsCount; ++i )
   {
      const IntersectionResult& existingContactPoint = outArrContactPoints[i];
      a.setSub( existingContactPoint.m_contactPoint, contactPoint.m_contactPoint );
      b.setSub( existingContactPoint.m_contactNormal, contactPoint.m_contactNormal );
      a.add( b );
      val.setAbs( a.dot( a ) + ( existingContactPoint.m_penetrationDepth - contactPoint.m_penetrationDepth ) );

      if ( val < Float_1e_3 )
      {
         // this point is almost exactly like the one we're trying to insert - consider it a duplicate and exit
         return;
      }
   }
   
   // if we got this far, it means that this point is genuine
   outArrContactPoints[outContactPointsCount] = contactPoint;
   ++outContactPointsCount;
}

///////////////////////////////////////////////////////////////////////////////

FastFloat Box::closestPtSegmentSegment( const Vector& p1, const Vector& q1, const Vector& p2, const Vector& q2, Vector& outC1, Vector& outC2 ) const
{
   // NOTE: This implementation was copied from the book "Real Time Collision Detection" by Christer Ericson, chapter 5.1.9
   FastFloat s, t;

   Vector d1; d1.setSub( q1, p1 ); // Direction vector of segment S1
   Vector d2; d2.setSub( q2, p2 ); // Direction vector of segment S2
   Vector r; r.setSub( p1, p2 );
   const FastFloat a = d1.dot( d1 ); // Squared length of segment S1, always nonnegative
   const FastFloat e = d2.dot( d2 ); // Squared length of segment S2, always nonnegative
   const FastFloat f = d2.dot( r );

   // Check if either or both segments degenerate into points
   if ( a <= Float_Inv3 && e <= Float_Inv3 )
   {
      // Both segments degenerate into points
      s = Float_0;
      t = Float_0;
      outC1 = p1;
      outC2 = p2;

      Vector x;
      x.setSub( outC1, outC2 );
      return x.dot( x );
   }

   if ( a <= Float_Inv3 )
   {
      // First segment degenerates into a point
      s = Float_0;
      t = f / e; // s = 0 => t = (b*s + f) / e = f / e
      t.setClamped( t, Float_0, Float_1 );
   }
   else 
   {
      const FastFloat c = d1.dot( r );
      if ( e <= Float_Inv3 ) 
      {
         // Second segment degenerates into a point
         t = Float_0;
         s.setClamped( -c / a, Float_0, Float_1 ); // t = 0 => s = (b*t - c) / a = -c / a
      }
      else 
      {
         // The general non-degenerate case starts here
         const FastFloat b = d1.dot( d2 );
         const FastFloat denom = a*e - b*b; // Always nonnegative

         // If segments not parallel, compute closest point on L1 to L2 and
         // clamp to segment S1. Else pick arbitrary s (here 0)
         if ( denom != Float_0 ) 
         {
            s.setClamped( ( b*f - c*e ) / denom, Float_0, Float_1 );
         }
         else
         {
            s = Float_0;
         }

         // Compute point on L2 closest to S1(s) using
         // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
         t = ( b*s + f ) / e;

         // If t in [0,1] done. Else clamp t, recompute s for the new value
         // of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
         // and clamp s to [0, 1]
         if ( t < Float_0 )
         {
            t = Float_0;
            s.setClamped( -c / a, Float_0, Float_1 );
         }
         else if ( t > Float_1 )
         {
            t = Float_1;
            s.setClamped( ( b - c ) / a, Float_0, Float_1 );
         }
      }
   }

   outC1.setMulAdd( d1, s, p1 );
   outC2.setMulAdd( d2, t, p2 );
   
   Vector x;
   x.setSub( outC1, outC2 );
   return x.dot( x );
}

///////////////////////////////////////////////////////////////////////////////
