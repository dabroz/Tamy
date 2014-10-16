/// @file   ext-2DGameLevel/GL2DShape.h
/// @brief  2d shape interface
#pragma once

#include "core\ReflectionObject.h"
#include "ext-2DGameLevel\Defines.h"


///////////////////////////////////////////////////////////////////////////////

struct IntersectionResult;
class GL2DLineSegmentsShape;
class GL2DCircleShape;
class GL2DBoxShape;
struct Ray;
struct RaycastResult;
struct Matrix;
struct Sphere;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

class GL2DShape : public ReflectionObject
{
   DECLARE_ALLOCATOR( GL2DShape, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Default constructor.
    */
   GL2DShape();
   virtual ~GL2DShape();

   /**
    * Initializes shape's geometry using a polygon soup.
    *
    * @param arrVertices
    * @param verticesCount
    * @param arrIndices
    * @param trianglesCount
    */
   virtual void setFromPolygonSoup( const Vector* arrVertices, const uint verticesCount, const word* arrIndices, const uint trianglesCount ) = 0;

   /**
   * Checks if the body collides with the specified ray.
   *
   * @param ray
   * @param transform
   * @param outResult
   * @return                       'true' if a collision takes place, 'false' otherwise
   */
   virtual bool rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const = 0;

   /**
   * Returns a bounding sphere that encapsulates the shape.
   *
   * @param outSphere
   */
   virtual void getBoundingSphere( Sphere& outSphere ) const = 0;

   /**
    * Tests a collision with another, generic 2D shape ( a visitor pattern ).
    *
    * @param otherShape
    * @param thisTransform
    * @param otherBodyTransform
    * @param outResult
    * @return number of returned results
    */
   virtual uint detectNarrowphaseCollision( const GL2DShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const = 0;

   /**
    * Tests intersection of this shape and a line segment shape.
    *
    * @param otherShape
    * @param thisTransform
    * @param otherBodyTransform
    * @param outResult
    * @return number of returned results
    */
   virtual uint collideAgainstStaticGeometry( const GL2DLineSegmentsShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const = 0;

   /**
    * Tests intersection of this shape and a circular shape.
    *
    * @param otherShape
    * @param thisTransform
    * @param otherBodyTransform
    * @param outResult
    * @return number of returned results
    */
   virtual uint collideAgainstCircle( const GL2DCircleShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const = 0;

   /**
    * Tests intersection of this shape and a box shape.
    *
    * @param otherShape
    * @param thisTransform
    * @param otherBodyTransform
    * @param outResult
    * @return number of returned results
    */
   virtual uint collideAgainstBox( const GL2DBoxShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
