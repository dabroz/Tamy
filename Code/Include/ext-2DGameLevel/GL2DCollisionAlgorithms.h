/// @file   ext-2DGameLevel/GL2DCollisionAlgorithms.h
/// @brief  a utility class that contains 2d collision detection algorithms
#pragma once

#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DCircleShape;
class GL2DBoxShape;
class GL2DLineSegmentsShape;
struct GL2DDirectedLineSegment;
struct IntersectionResult;
struct Matrix;

///////////////////////////////////////////////////////////////////////////////

class GL2DCollisionAlgorithms
{
public:
   /**
    * Checks for a collision between two circles.
    *
    * @param shapeA
    * @param shapeB
    * @param shapeATransform
    * @param shapeBTransform
    * @param outArrResults
    * @return 'true' if there's a collision, 'false' otherwise
    */
   static bool circleVsCircle( const GL2DCircleShape& shapeA, const GL2DCircleShape& shapeB, const Matrix& shapeATransform, const Matrix& shapeBTransform, IntersectionResult& outResult );

   /**
   * Checks for a collision between a circle and a box.
   *
   * @param shapeA
   * @param shapeB
   * @param shapeATransform
   * @param shapeBTransform
   * @param outArrResults
   * @return 'true' if there's a collision, 'false' otherwise
   */
   static bool circleVsBox( const GL2DCircleShape& shapeA, const GL2DBoxShape& shapeB, const Matrix& shapeATransform, const Matrix& shapeBTransform, IntersectionResult& outResult );

   /**
   * Checks for a collision between a circle and static geometry.
   *
   * @param shapeA
   * @param shapeB
   * @param circleTransform
   * @param outArrResults
   * @return 'true' if there's a collision, 'false' otherwise
   */
   static uint circleVsLineSegments( const GL2DCircleShape& shapeA, const GL2DLineSegmentsShape& shapeB, const Matrix& circleTransform, IntersectionResult* outArrResults );

   /**
   * Checks for a collision between a circle and single directed ( 1-sided ) line segment.
   *
   * @param shapeA
   * 2param circleTransform
   * @param segment
   * @param outResult
   * @return 'true' if there's a collision, 'false' otherwise
   */
   static bool circleVsLineSegment( const GL2DCircleShape& shape, const Matrix& circleTransform, const GL2DDirectedLineSegment& segment, IntersectionResult& outResult );

};

///////////////////////////////////////////////////////////////////////////////
