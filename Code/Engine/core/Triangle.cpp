#include "core.h"
#include "core\Triangle.h"
#include "core\Matrix.h"
#include "core\Plane.h"
#include "core\AxisAlignedBox.h"
#include "core\MathDataStorage.h"
#include "core\RaycastResult.h"
#include "core\Ray.h"
#include "core\IntersectionResult.h"


///////////////////////////////////////////////////////////////////////////////

Triangle::Triangle( )
{
}

///////////////////////////////////////////////////////////////////////////////

Triangle::Triangle( const Triangle& rhs )
{
   v[0] = rhs.v[0];
   v[1] = rhs.v[1];
   v[2] = rhs.v[2];

   e[0] = rhs.e[0];
   e[1] = rhs.e[1];
   e[2] = rhs.e[2];

   en[0] = rhs.en[0];
   en[1] = rhs.en[1];
   en[2] = rhs.en[2];

   m_trianglePlane.setFromPoints( v[0], v[1], v[2] );
}

///////////////////////////////////////////////////////////////////////////////

Triangle::Triangle( const Vector& pt1, const Vector& pt2, const Vector& pt3 )
{
   setFromVertices( pt1, pt2, pt3 );
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::setFromVertices( const Vector& pt1, const Vector& pt2, const Vector& pt3 )
{
   v[0] = pt1;
   v[1] = pt2;
   v[2] = pt3;

   // calculate the edges
   e[0].setSub( v[1], v[0] );
   e[1].setSub( v[2], v[1] );
   e[2].setSub( v[0], v[2] );

   // calculate the edge normals
   Vector tmpPerpVec;
   tmpPerpVec.setCross( e[1], e[0] );
   tmpPerpVec.preCross( e[0] );
   en[0].setNormalized( tmpPerpVec );

   tmpPerpVec.setCross( e[2], e[1] );
   tmpPerpVec.preCross( e[1] );
   en[1].setNormalized( tmpPerpVec );

   tmpPerpVec.setCross( e[0], e[2] );
   tmpPerpVec.preCross( e[2] );
   en[2].setNormalized( tmpPerpVec );

   // set the plane
   m_trianglePlane.setFromPoints( v[0], v[1], v[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::transform( const Matrix& mtx, Triangle& transformedTriangle ) const
{
   Vector newV[3];
   mtx.transform( v[0], newV[0] );
   mtx.transform( v[1], newV[1] );
   mtx.transform( v[2], newV[2] );

   transformedTriangle.setFromVertices( newV[0], newV[1], newV[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::calculateBoundingBox( AxisAlignedBox& outBoundingBox ) const
{
   outBoundingBox.include( v[0] );
   outBoundingBox.include( v[1] );
   outBoundingBox.include( v[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::splitEdge( const FastFloat& percentage, unsigned int startVtxIdx, unsigned int endVtxIdx, Vector& outEdge ) const
{
   const Vector& v1 = v[startVtxIdx];
   Vector edge;
   edge.setSub( v[endVtxIdx], v1 );

   outEdge.setMulAdd( edge, percentage, v1 );
}

///////////////////////////////////////////////////////////////////////////////

PlaneClassification Triangle::classifyAgainsPlane( const Plane& plane ) const
{
    const FastFloat dist1 = plane.dotCoord( v[0] );
    const FastFloat dist2 = plane.dotCoord( v[1] );
    const FastFloat dist3 = plane.dotCoord( v[2] );

    if ( ( dist1 < Float_0 ) && ( dist2 < Float_0 ) && ( dist3 < Float_0 ) )
    {
        return PPC_BACK;
    }
    else if ( ( dist1 > Float_0 ) && ( dist2 > Float_0 ) && ( dist3 > Float_0 ) )
    {
        return PPC_FRONT;
    }
    else if ( ( dist1 == Float_0 ) && ( dist2 == Float_0 ) && ( dist3 == Float_0 ) )
    {
        return PPC_COPLANAR;
    }
    else
    {
        return PPC_SPANNING;
    }
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Triangle::distanceToPlane( const Plane& plane ) const
{
   FastFloat posEps, negEps;
   posEps = Float_1e_4;
   negEps.setNeg( Float_1e_4 );

   int frontCount = 0;
   int backCount = 0;
   for (unsigned int i = 0; i < 3; ++i)
   {
      Vector vtx = v[i];
      vtx[3] = 1;

      const FastFloat dist = plane.dotCoord( vtx );
      if ( dist > posEps )
      {
         ++frontCount;
      }
      else if ( dist < negEps )
      {
         ++backCount;
      }
   }

   if ( frontCount == 3 ) 
   {
      return Float_1;
   }
   else if ( backCount == 3 ) 
   {
      return Float_Minus1;
   }
   else 
   {
      return Float_0;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::split( const Plane& plane, Array< Triangle* >& front, Array< Triangle* >& back )
{
   FastFloat posDistanceEps, negDistanceEps;
   posDistanceEps = Float_0;
   negDistanceEps.setNeg( Float_0 );

   Array<Vector> frontPoly(4);
   Array<Vector> backPoly(4);

   // split the triangle into two polygons
   for (unsigned int i = 0; i < 3; ++i)
   {
      const Vector& analyzedVtxPos = vertexPos(i);
      const Vector& analyzedVtx = vertex(i);
      const FastFloat prevDist = plane.dotCoord( analyzedVtxPos );

      if ( prevDist > posDistanceEps )
      {
         frontPoly.push_back( analyzedVtx );
      }
      else if ( prevDist < negDistanceEps )
      {
         backPoly.push_back( analyzedVtx );
      }
      else
      {
         frontPoly.push_back( analyzedVtx );
         backPoly.push_back( analyzedVtx );
      }

      unsigned int nextVtxIdx = ( i + 1 ) % 3;
      const FastFloat nextDist = plane.dotCoord( vertexPos( nextVtxIdx ) );
      FastFloat distDot;
      distDot.setMul( nextDist, prevDist );

      if ( distDot < negDistanceEps )
      {
         FastFloat distToV1, distToV2, percentage;
         distToV1.setAbs( prevDist );
         distToV2.setAbs( nextDist );
         percentage.setAdd( distToV1, distToV2 );
         percentage.reciprocal();
         percentage.mul( distToV1 );
         Vector midVtx;
         splitEdge( percentage, i, nextVtxIdx, midVtx );
         frontPoly.push_back( midVtx );
         backPoly.push_back( midVtx );
      }
   }

   // triangulate both
   triangulatePoly( frontPoly, front );
   triangulatePoly( backPoly, back );
}

///////////////////////////////////////////////////////////////////////////////

void Triangle::triangulatePoly( const Array< Vector >& poly, Array< Triangle* >& output )
{
   unsigned int verticesCount = poly.size();
   switch(verticesCount)
   {
   case 3:
      {
         output.push_back( new Triangle(poly[0], poly[1], poly[2] ) );
         break;
      }

   case 4:
      {
         output.push_back( new Triangle( poly[0], poly[1], poly[2] ) );
         output.push_back( new Triangle( poly[0], poly[2], poly[3] ) );
         break;
      }
   default:
      break;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Triangle::operator==(const Triangle& rhs) const
{
   bool match = true;
   Vector tmpDir;
   for (int i = 0; i < 3; ++i)
   {
      tmpDir.setSub( v[i], rhs.v[i] );
      const FastFloat dist = tmpDir.length();
      if (dist > Float_1e_3) 
      {
         match = false;
         break;
      }
   }

   return match;
}

///////////////////////////////////////////////////////////////////////////////

bool Triangle::operator!=(const Triangle& rhs) const
{
   Vector tmpDir;
   for (int i = 0; i < 3; ++i)
   {
      tmpDir.setSub( v[i], rhs.v[i] );
      const FastFloat dist = tmpDir.length();
      if (dist > Float_1e_3) 
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool Triangle::rayCast( const Ray& ray, RaycastResult& outResult ) const
{
   // first check if the ray intersects the plane the triangle is on.
   Plane trianglePlane;
   trianglePlane.setFromPoints( vertexPos( 0 ), vertexPos( 1 ), vertexPos( 2 ) );

   if ( ray.rayCast( trianglePlane, outResult ) == false )
   {
      return false;
   }

   // now test to see if the intersection point is located inside of the triangle
   Vector tmpDirToIntersectionPt;
   Vector tmpCrossProducts[3];
   for ( int i = 0; i < 3; ++i )
   {
      tmpDirToIntersectionPt.setSub( outResult.m_contactPt, vertexPos( i ) );
      tmpCrossProducts[i].setCross( tmpDirToIntersectionPt, edge( i ) );
   }

   for ( int i = 0; i < 3; ++i )
   {
      int nextIdx = ( i + 1 ) % 3;
      const FastFloat dot = tmpCrossProducts[i].dot( tmpCrossProducts[nextIdx] );
      if ( dot < Float_0 )
      {
         // the point lies outside the triangle
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Triangle::testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const
{
   const FastFloat distance = m_trianglePlane.dotCoord( sphere.origin );
   if ( distance < Float_0 || distance > sphere.radius )
   {
      return false;
   }
   
   m_trianglePlane.getNormal( outResult.m_contactNormal );
   outResult.m_contactPoint.setMulAdd( outResult.m_contactNormal, -distance, sphere.origin );
   outResult.m_penetrationDepth = sphere.radius - distance;


   // we're gonna need to move the vertices to sphere's model coordinate system
   Vector vsm[3];
   vsm[0].setSub( sphere.origin, v[0] );
   vsm[1].setSub( sphere.origin, v[1] );
   vsm[2].setSub( sphere.origin, v[2] );
   const FastFloat rr = sphere.radius * sphere.radius;

   // next - test if the sphere center lies outside of the triangle edges
   {
      Vector ne[3];
      ne[0].setNormalized( e[0] );
      ne[1].setNormalized( e[1] );
      ne[2].setNormalized( e[2] );

      const FastFloat d1 = vsm[0].dot( ne[0] );
      const FastFloat d2 = vsm[1].dot( ne[1] );
      const FastFloat d3 = vsm[2].dot( ne[2] );

      Vector Q[3];
      Q[0].setMulAdd( ne[0], d1, v[0] );
      Q[1].setMulAdd( ne[1], d2, v[1] );
      Q[2].setMulAdd( ne[2], d3, v[2] );

      // displacements from projection points to the sphere's origin
      Vector QS[3];
      QS[0].setSub( sphere.origin, Q[0] );
      QS[1].setSub( sphere.origin, Q[1] );
      QS[2].setSub( sphere.origin, Q[2] );

      // displacements from projection points to the  opposite vertex
      Vector QO[3];
      QO[0].setSub( v[2], Q[0] );
      QO[1].setSub( v[0], Q[1] );
      QO[2].setSub( v[1], Q[2] );

      Vector test[3];
      test[0].set( QS[0].dot( QS[0] ), QS[1].dot( QS[1] ), QS[2].dot( QS[2] ) );
      test[1].setBroadcast( rr );
      test[2].set( QS[0].dot( QO[0] ), QS[1].dot( QO[1] ), QS[2].dot( QO[2] ) );

      VectorComparison compResult[3];
      test[0].greater( test[1], compResult[0] );
      test[2].less( Vector_ZERO, compResult[1] );
      compResult[2].setAnd( compResult[0], compResult[1] );
      if ( compResult[2].isAnySet< VectorComparison::MASK_XYZ >( ) )
      {
         return false;
      }


      // select the lowest penetration
      Vector x, y;
      y.setBroadcast( FastFloat::fromFloat( FLT_MAX ) );
      x.setSelect( compResult[1], test[0], y );
      x.selectMin<4>( compResult[1] );
      int minIdx = compResult[1].toIndex();
      if ( minIdx < 3 )
      {
         outResult.m_contactPoint = Q[minIdx];
         outResult.m_contactNormal.setNormalized( QS[minIdx] );
         outResult.m_penetrationDepth.setSqrt( test[0].getComponent( minIdx ) );
         outResult.m_penetrationDepth = sphere.radius - outResult.m_penetrationDepth;
      }
   }

   // last - test if the sphere center lies outside of the triangle vertices
   {
      FastFloat vtxPenetrationDepth[3];
      vtxPenetrationDepth[0] = vsm[0].dot( vsm[0] );
      vtxPenetrationDepth[1] = vsm[1].dot( vsm[1] );
      vtxPenetrationDepth[2] = vsm[2].dot( vsm[2] );

      const FastFloat ab = vsm[0].dot( vsm[1] );
      const FastFloat ac = vsm[0].dot( vsm[2] );
      const FastFloat bc = vsm[1].dot( vsm[2] );

      Vector test[4];
      test[0].set( vtxPenetrationDepth[0], vtxPenetrationDepth[1], vtxPenetrationDepth[2] );
      test[1].setBroadcast( rr );
      test[2].set( ab, ab, ac );
      test[3].set( ac, bc, bc );

      VectorComparison compResult[3];
      test[0].greater( test[1], compResult[0] );
      test[2].greater( test[0], compResult[1] );
      test[3].greater( test[0], compResult[2] );

      compResult[1].setAnd( compResult[1], compResult[2] );
      compResult[0].setAnd( compResult[0], compResult[1] );

      if ( compResult[0].isAnySet< VectorComparison::MASK_XYZ >( ) )
      {
         return false;
      }

      // select the lowest penetration
      Vector x, y;
      y.setBroadcast( FastFloat::fromFloat( FLT_MAX ) );
      x.setSelect( compResult[1], test[0], y );
      x.selectMin<4>( compResult[1] );
      int minIdx = compResult[1].toIndex( );
      if ( minIdx < 3 )
      {
         outResult.m_contactPoint = v[minIdx];
         outResult.m_contactNormal.setSub( sphere.origin, outResult.m_contactPoint );
         outResult.m_contactNormal.normalize();
         outResult.m_penetrationDepth.setSqrt( vtxPenetrationDepth[minIdx] );
         outResult.m_penetrationDepth = sphere.radius - outResult.m_penetrationDepth;
      }
   }

   return outResult.m_penetrationDepth > Float_0;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const Triangle& t)
{
   os << "{" << t.v[0] << " " << t.v[1] << " " << t.v[2] << "}";
   return os;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const Triangle& triangle )
{
   stream << triangle.v[0];
   stream << triangle.v[1];
   stream << triangle.v[2];

   stream << triangle.e[0];
   stream << triangle.e[1];
   stream << triangle.e[2];

   stream << triangle.en[0];
   stream << triangle.en[1];
   stream << triangle.en[2];

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, Triangle& triangle )
{
   stream >> triangle.v[0];
   stream >> triangle.v[1];
   stream >> triangle.v[2];

   stream >> triangle.e[0];
   stream >> triangle.e[1];
   stream >> triangle.e[2];

   stream >> triangle.en[0];
   stream >> triangle.en[1];
   stream >> triangle.en[2];

   triangle.m_trianglePlane.setFromPoints( triangle.v[0], triangle.v[1], triangle.v[2] );

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
