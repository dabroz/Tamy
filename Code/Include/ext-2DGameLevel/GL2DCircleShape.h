/// @file   ext-2DGameLevel/GL2DCircleShape.h
/// @brief  a colliding circular shape
#pragma once

#include "ext-2DGameLevel\GL2DShape.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DCircleShape : public GL2DShape
{
   DECLARE_ALLOCATOR( GL2DCircleShape, AM_DEFAULT );
   DECLARE_CLASS( );

private:
   // static data
   float          m_radius;

public:
   /**
    * Constructor.
    *
    * @param radius
    */
   GL2DCircleShape( float radius = 1.0f );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   GL2DCircleShape( const GL2DCircleShape& rhs );

   /**
    * Sets the new radius of the circle.
    */
   void setRadius( float radius );

   /**
    * Returns the radius of the circle.
    */
   inline const FastFloat getRadius() const
   {
      return FastFloat::fromFloat( m_radius );
   }

   // -------------------------------------------------------------------------
   // GL2DShape implementation
   // -------------------------------------------------------------------------
   void setFromPolygonSoup( const Vector* arrVertices, const uint verticesCount, const word* arrIndices, const uint trianglesCount );
   bool rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const;
   void getBoundingSphere( Sphere& outSphere ) const;
   uint detectNarrowphaseCollision( const GL2DShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const;
   uint collideAgainstStaticGeometry( const GL2DLineSegmentsShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const;
   uint collideAgainstCircle( const GL2DCircleShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const;
   uint collideAgainstBox( const GL2DBoxShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const;
};

///////////////////////////////////////////////////////////////////////////////
