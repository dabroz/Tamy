/// @file   ext-2DGameLevel/GL2DLineSegmentsShape.h
/// @brief  a static 2d shape that consists of multiple '1-sided' line segments
#pragma once

#include "ext-2DGameLevel\GL2DShape.h"
#include "core\Array.h"
#include "core\Vector.h"
#include "core\Sphere.h"
#include "core\AxisAlignedBox.h"
#include "ext-2DGameLevel\GL2DDirectedLineSegment.h"


///////////////////////////////////////////////////////////////////////////////

#define GL2D_MAX_SEGMENTS_PER_NODE 64
#define GL2D_MIN_ALLOWED_NODE_AREA Float_4

///////////////////////////////////////////////////////////////////////////////

struct Transform;
struct GL2DSegmentsTreeNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * A static 2d shape that consists of multiple '1-sided' line segments.
 */
class GL2DLineSegmentsShape : public GL2DShape
{
   DECLARE_ALLOCATOR( GL2DLineSegmentsShape, AM_DEFAULT );
   DECLARE_CLASS( );

private:
   Array< GL2DDirectedLineSegment >    m_segments;

   GL2DSegmentsTreeNode*               m_rootNode;

public:
   /**
    * Default constructor.
    */
   GL2DLineSegmentsShape();

   /**
   * Copy constructor.
   *
   * @param rhs
   */
   GL2DLineSegmentsShape( const GL2DLineSegmentsShape& rhs);
   ~GL2DLineSegmentsShape();

   /**
    * Returns the line segments the geometry consists of.
    *
    * @param boundingSphere      area of the geometry we want the segments from
    */
   void getSegments( const Sphere& boundingSphere, Array< const GL2DDirectedLineSegment* >& outSegments ) const;

   // -------------------------------------------------------------------------
   // Geometry building methods
   // -------------------------------------------------------------------------

   /**
    * Builds the shape after all segments have been added.
    * This needs to be called, otherwise the shape won't work.
    */
   void build();

   /**
    * Adds a rectangle.
    * 
    * @param transform
    * @param halfExtents
    */
   void addRect( const Transform& transform, const Vector& halfExtents );

   /**
    * Adds a single line segment.
    *
    * @param start
    * @param end
    */
   void addSegment( const Vector& start, const Vector& end );

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

struct GL2DSegmentsTreeNode : public ReflectionObject
{
   DECLARE_ALLOCATOR( GL2DSegmentsTreeNode, AM_DEFAULT );
   DECLARE_CLASS( );

public:
   uint                                m_startIdx;
   uint                                m_count;
   AxisAlignedBox                      m_bounds;

   GL2DSegmentsTreeNode*               m_child;
   GL2DSegmentsTreeNode*               m_sibling;

public:
   /**
    * Default constructor.
    */
   GL2DSegmentsTreeNode();

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   GL2DSegmentsTreeNode( const GL2DSegmentsTreeNode& rhs );
   ~GL2DSegmentsTreeNode();

   /**
    * Calculates the area the node occupies.
    */
   const FastFloat calcArea() const;

   /**
    * Subdivides the node into 4 equal parts.
    */
   void subdivide();
};

///////////////////////////////////////////////////////////////////////////////
