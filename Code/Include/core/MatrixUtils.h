/// @file   core/MatrixUtils.h
#pragma once

#include "core\Assert.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

struct Matrix;
struct Vector;
struct FastFloat;

///////////////////////////////////////////////////////////////////////////////

class MatrixUtils
{
public:
   /**
    * Performs a linear interpolation between two matrices.
    *
    * @param start
    * @param end
    * @param lerpDist
    * @param outInterpolatedMtx
    */
   static void lerp( const Matrix& start, const Matrix& end, const FastFloat& lerpDist, Matrix& outInterpolatedMtx );

   /**
    * Generates an perspective projection matrix.
    *
    * @param fov              specified in RADIANS
    * @param aspectRatio
    * @param nearZPlane
    * @param farZPlane
    * @param outProjMtx
    */
   static void generatePrespectiveProjection( float fov, float aspectRatio, float nearZPlane, float farZPlane, Matrix& outProjMtx );

   /**
    * Generates an orthogonal projection matrix.
    *
    * @param viewportWidth
    * @param viewportHeight
    * @param nearZPlane
    * @param farZPlane
    * @param outProjMtx
    */
   static void generateOrthogonalProjection( float viewportWidth, float viewportHeight, float nearZPlane, float farZPlane, Matrix& outProjMtx );

   /**
    * Generates an orthogonal projection matrix that is offset from the view center.
    * It will produce the same results as 'generateOrthogonalProjection', if ( right == -left ) && ( top == -bottom ).
    *
    * @param left
    * @param right
    * @param bottom
    * @param top
    * @param nearZPlane
    * @param farZPlane
    * @param outProjMtx
    */
   static void generateOrthogonalProjectionOffCenter( float left, float right, float bottom, float top, float nearZPlane, float farZPlane, Matrix& outProjMtx );

   /**
    * Generates a camera look-at matrix for a left-handed coordinate system.
    *
    * @param cameraOriginPos
    * @param lookAtPos
    * @param upAxis
    * @param outLookAtMtx
    */
   static void generateLookAtLH( const Vector& cameraOriginPos, const Vector& lookAtPos, const Vector& upAxis, Matrix& outLookAtMtx );

   /**
    * Generates a viewport matrix used to transform a projected position to a viewport position.
    *
    * @param offsetX       viewport left corner offset ( in pixels )
    * @param offsetY       viewport top corner offset ( in pixels )
    * @param width         viewport width
    * @param height         viewport height
    * @param outViewportMtx
    */
   static void generateViewportMatrix( uint offsetX, uint offsetY, uint width, uint height, Matrix& outViewportMtx );

   /**
    * Calculates a matrix that transforms all objects to the view space of the specified input matrix.
    *
    * @param inMtx
    * @param outViewMtx
    */
   static void calculateViewMtx( const Matrix& inMtx, Matrix& outViewMtx );

   /**
    * Regenerates system axes after a rotation operation that could have twisted them.
    */
   static void regenerateAxes( Matrix& inOutTransform );

   /**
    * Changes a reference system for a transformation.
    * What inTransform initially expressed with respect to the global coordinate system, will now become outTransform,
    * which is expressed with respect to destSystem.
    *
    * @param inTransform
    * @param destSystem
    * @param outTransform
    */
   static void changeReferenceSystem( const Matrix& inTransform,  const Matrix& destSystem, Matrix& outTransform );
};

///////////////////////////////////////////////////////////////////////////////
