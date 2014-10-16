/// @file   ext-2DGameLevel\GL2DBoxShape.h
/// @brief  a rectangular collision shape
#pragma once

#include "ext-2DGameLevel\GL2DShape.h"
#include "ext-2DGameLevel\GL2DDirectedLineSegment.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct GL2DDirectedLineSegment;

///////////////////////////////////////////////////////////////////////////////

class GL2DBoxShape : public GL2DShape
{
   DECLARE_ALLOCATOR( GL2DBoxShape, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   Vector                              m_halfExtents;

   // runtime data
   GL2DDirectedLineSegment             m_lineSegments[4];

public:
   /**
    * Constructor.
    *
    * @param halfExtents
    */
   GL2DBoxShape( const Vector& halfExtents = Vector_ONE );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   GL2DBoxShape( const GL2DBoxShape& rhs );

   /**
    * Returns pre-calculated line segments the box is made of.
    */
   inline const GL2DDirectedLineSegment* getSegments() const {
      return m_lineSegments;
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

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
   void onPropertyChanged( ReflectionProperty& property );

private:
   /**
    * Recalculates the directed segments.
    */
   void calculateSegments();
};

///////////////////////////////////////////////////////////////////////////////
