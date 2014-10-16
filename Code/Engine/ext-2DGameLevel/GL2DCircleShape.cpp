#include "ext-2DGameLevel\GL2DCircleShape.h"
#include "ext-2DGameLevel\GL2DCollisionAlgorithms.h"
#include "core\Sphere.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DCircleShape );
   PARENT( GL2DShape );
   PROPERTY_EDIT( "Radius", float, m_radius );
END_OBJECT( );

///////////////////////////////////////////////////////////////////////////////

GL2DCircleShape::GL2DCircleShape( float radius )
   : m_radius( radius )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DCircleShape::GL2DCircleShape( const GL2DCircleShape& rhs )
   : GL2DShape( rhs )
   , m_radius( rhs.m_radius )
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCircleShape::setRadius( float radius )
{
   NOTIFY_PROPERTY_CHANGE( m_radius );

   m_radius = radius;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCircleShape::setFromPolygonSoup( const Vector* arrVertices, const uint verticesCount, const word* arrIndices, const uint trianglesCount )
{
   NOTIFY_PROPERTY_CHANGE( m_radius );

   m_radius = 0.0f;
   Vector projVector;
   for ( uint i = 0; i < verticesCount; ++i )
   {
      projVector.setSelect< VectorComparison::MASK_XY >( arrVertices[i], Vector_ZERO );
      const float dist = projVector.length().getFloat();
      if ( dist > m_radius )
      {
         m_radius = dist;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DCircleShape::detectNarrowphaseCollision( const GL2DShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   return otherShape->collideAgainstCircle( this, otherBodyTransform, thisTransform, outArrResults );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DCircleShape::collideAgainstStaticGeometry( const GL2DLineSegmentsShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   return GL2DCollisionAlgorithms::circleVsLineSegments( *this, *otherShape, thisTransform, outArrResults );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DCircleShape::collideAgainstCircle( const GL2DCircleShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   bool doesCollide = GL2DCollisionAlgorithms::circleVsCircle( *this, *otherShape, thisTransform, otherBodyTransform, *outArrResults );
   return ( int ) doesCollide;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DCircleShape::collideAgainstBox( const GL2DBoxShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   bool doesCollide = GL2DCollisionAlgorithms::circleVsBox( *this, *otherShape, thisTransform, otherBodyTransform, *outArrResults );
   return ( int ) doesCollide;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCircleShape::rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCircleShape::getBoundingSphere( Sphere& outSphere ) const
{
   outSphere.set( Vector_ZERO, m_radius );
}

///////////////////////////////////////////////////////////////////////////////
