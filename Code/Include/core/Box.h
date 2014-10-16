/// @file   core/Box.h
/// @brief  box shape
#pragma once

#include "core\MemoryRouter.h"
#include "core\AxisAlignedBox.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

struct Ray;
struct RaycastResult;
struct IntersectionResult;
struct Sphere;
struct Triangle;

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct Box
{
   DECLARE_ALLOCATOR( Box, AM_DEFAULT );

   AxisAlignedBox          m_axisAlignedBox;
   Matrix                  m_worldToLocal;
   Matrix                  m_localToWorld;

   /**
    * Constructor.
    */
   Box();

   /**
    * Constructor.
    *
    * @param min
    * @param max
    * @param transform
    */
   Box( const Vector& min, const Vector& max, const Matrix& transform = Matrix::IDENTITY );

   /**
    * Constructor.
    *
    * @param extents
    * @param transform
    */
   Box( const Vector& extents, const Matrix& transform = Matrix::IDENTITY );

   /**
    * Sets the corner values of the bounding box.
    *
    * @param min
    * @param max
    * @param transform
    */
   void set( const Vector& min, const Vector& max, const Matrix& transform = Matrix::IDENTITY );

   /**
    * Sets the corner values of the bounding box.
    *
    * @param extents
    * @param transform
    */
   void set( const Vector& extents, const Matrix& transform = Matrix::IDENTITY );
   
   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Box( const Box& rhs );

   /**
    * Transforms the box.
    *
    * @param mtx
    * @param transformedBox
    */
   void transform( const Matrix& mtx, Box& transformedBox ) const;

   /**
    * A ray-cast against a box.
    *
    * @param box
    * @param outResult
    */
   bool rayCast( const Ray& ray, RaycastResult& outResult ) const;

   /**
    * Tests an intersection between the box and a sphere.
    *
    * @param sphere
    * @param outResult
    */
   bool testIntersection( const Sphere& sphere, IntersectionResult& outResult ) const;

   /**
    * Tests an intersection between two boxes.
    *
    * @param box
    * @param outArrContactPoints
    * @param maxContactPoints       how many entries can the outArrContactPoints store
    * @return  number of generated contact points. If 0 is returned, the two boxes don't collide
    */
   uint testIntersection( const Box& box, IntersectionResult* outArrContactPoints, uint maxContactPoints ) const;

   /**
   * Tests an intersection between a box and a triangle.
   *
   * @param box
   * @param outArrContactPoints
   * @param maxContactPoints       how many entries can the outArrContactPoints store
   * @return  number of generated contact points. If 0 is returned, the two boxes don't collide
   */
   uint testIntersection( const Triangle& triangle, IntersectionResult* outArrContactPoints, uint maxContactPoints ) const;

private:
   void addUniqueContactPoint( const IntersectionResult& contactPoint, IntersectionResult* outArrContactPoints, uint maxContactPoints, uint& outContactPointsCount ) const;
   FastFloat closestPtSegmentSegment( const Vector& p1, const Vector& q1, const Vector& p2, const Vector& q2, Vector& outC1, Vector& outC2 ) const;
};

///////////////////////////////////////////////////////////////////////////////
