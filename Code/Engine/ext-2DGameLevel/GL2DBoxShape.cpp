#include "ext-2DGameLevel\GL2DBoxShape.h"
#include "ext-2DGameLevel\GL2DCollisionAlgorithms.h"
#include "core\Sphere.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DBoxShape );
   PARENT( GL2DShape );
   PROPERTY_EDIT( "Half extents", Vector, m_halfExtents );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GL2DBoxShape::GL2DBoxShape( const Vector& halfExtents )
   : m_halfExtents( halfExtents )
{
   calculateSegments();
}

///////////////////////////////////////////////////////////////////////////////

GL2DBoxShape::GL2DBoxShape( const GL2DBoxShape& rhs )
   : GL2DShape( rhs )
   , m_halfExtents( rhs.m_halfExtents )
{
   memcpy( m_lineSegments, rhs.m_lineSegments, sizeof( GL2DDirectedLineSegment ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBoxShape::onObjectLoaded()
{
   GL2DShape::onObjectLoaded();

   calculateSegments();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBoxShape::onPropertyChanged( ReflectionProperty& property )
{
   GL2DShape::onPropertyChanged( property );

   if ( property.getName() == "m_halfExtents" )
   {
      calculateSegments();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBoxShape::setFromPolygonSoup( const Vector* arrVertices, const uint verticesCount, const word* arrIndices, const uint trianglesCount )
{
   NOTIFY_PROPERTY_CHANGE( m_halfExtents );

   AxisAlignedBox bounds;
   Vector projVector;
   for ( uint i = 0; i < verticesCount; ++i )
   {
      projVector.setSelect< VectorComparison::MASK_XY >( arrVertices[i], Vector_ZERO );
      bounds.include( projVector );
   }

   bounds.getExtents( m_halfExtents );
   m_halfExtents.mul( Float_Inv2 );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DBoxShape::detectNarrowphaseCollision( const GL2DShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   return otherShape->collideAgainstBox( this, otherBodyTransform, thisTransform, outArrResults );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DBoxShape::collideAgainstStaticGeometry( const GL2DLineSegmentsShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   // only circular shapes can collide with line segments
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DBoxShape::collideAgainstCircle( const GL2DCircleShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   bool doesCollide = GL2DCollisionAlgorithms::circleVsBox( *otherShape, *this, otherBodyTransform, thisTransform, *outArrResults );
   return ( uint ) doesCollide;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DBoxShape::collideAgainstBox( const GL2DBoxShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   // boxes are used to model moving, yet mutually 'passable' shapes, so we're not calculating any collisions here
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DBoxShape::rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBoxShape::getBoundingSphere( Sphere& outSphere ) const
{
   outSphere.set( Vector_ZERO, m_halfExtents.length() );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBoxShape::calculateSegments()
{
   m_halfExtents[2] = 0.0f;

   Vector negHalfExtents;
   negHalfExtents.setMul( m_halfExtents, Float_Minus1 );

   Vector points[4];
   points[0] = negHalfExtents;
   points[1].setSelect< VectorComparison::MASK_Y >( negHalfExtents, m_halfExtents );
   points[2] = negHalfExtents;
   points[3].setSelect< VectorComparison::MASK_X >( negHalfExtents, m_halfExtents );
   byte j;
   for ( byte i = 0; i < 4; ++i )
   {
      j = ( i + 1 ) % 4;
      m_lineSegments[i].set( points[i], points[j] );
   }
}

///////////////////////////////////////////////////////////////////////////////
