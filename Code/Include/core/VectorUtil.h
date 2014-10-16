/// @file	core\VectorUtil.h
/// @brief  standalone mathematical vector operations
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct FastFloat;

///////////////////////////////////////////////////////////////////////////////

class VectorUtil
{
public:
   /**
    * Calculates a vector perpendicular to the specified one.
    *
    * @param inVec
    * @para outPerpVec
    */
   static void calculatePerpendicularVector( const Vector& inVec, Vector& outPerpVec );

   /**
   * Calculates a vector perpendicular to the specified one ( in the 2D space, in the XY plane - the Z component will be ignored ).
   *
   * @param inVec
   * @para outPerpVec
   */
   static void calculatePerpendicularVector2D( const Vector& inVec, Vector& outPerpVec );

   /**
    * Limits the vector's length to the specified value.
    *
    * @param inVec
    * @param maxLen
    * @param outVec
    */
   static void limitLength( const Vector& inVec, const FastFloat& maxLen, Vector& outVec );

   /**
    * A tool for calculating the shortest angle ( -PI, PI ) between two vectors, around the specified rotation axis.
    *
    * @param startVec
    * @param endVec
    * @param rotationAxis     assumed to be a unit vector
    * @param outAngle
    */
   static void calcAngle( const Vector& startVec, const Vector& endVec, const Vector& rotationAxis, FastFloat& outAngle );

   /**
    * A tool for calculating the shortest angle ( -PI, PI ) between two vectors, around the specified rotation axis.
    * CAUTION: this method assumes that all vectors are unit vectors
    *
    *
    * @param startVec         assumed to be a unit vector
    * @param endVec           assumed to be a unit vector
    * @param rotationAxis     assumed to be a unit vector
    * @param outAngle
    */
   static void calcAngleNormalized( const Vector& startVec, const Vector& endVec, const Vector& rotationAxis, FastFloat& outAngle );

};

///////////////////////////////////////////////////////////////////////////////
