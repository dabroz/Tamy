#include "core.h"
#include "core\MatrixUtils.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::lerp( const Matrix& start, const Matrix& end, const FastFloat& lerpDist, Matrix& outInterpolatedMtx )
{
   ASSERT_MSG( lerpDist <= Float_1, "The value for advancement should be <= 1");
   ASSERT_MSG( lerpDist >= Float_0, "The value for advancement should be >= 0");

   // translation
   Vector startPos = start.position();
   Vector endPos = end.position();

   Vector resultPos;
   resultPos.setSub( endPos, startPos );
   resultPos.mul( lerpDist );
   resultPos.add( startPos );

   Matrix translationMtx;
   translationMtx.setTranslation( resultPos );

   // rotation
   Quaternion startQuat, endQuat;
   start.getRotation( startQuat );
   end.getRotation( endQuat );

   Quaternion resultQuat;
   resultQuat.setSlerp( startQuat, endQuat, lerpDist );

   Matrix rotationMtx;
   rotationMtx.setRotation( resultQuat );

   // calculate the final result
   outInterpolatedMtx.setMul( rotationMtx, translationMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generatePrespectiveProjection( float fov, float aspectRatio, float nearZPlane, float farZPlane, Matrix& outProjMtx )
{
   const float yScale = 1.0f / tan( fov / 2.0f );
   const float xScale = yScale / aspectRatio;

   outProjMtx.setIdentity();
   outProjMtx( 0, 0 ) = xScale;
   outProjMtx( 1, 1 ) = yScale;
   outProjMtx( 2, 2 ) = farZPlane / ( farZPlane - nearZPlane );
   outProjMtx( 2, 3 ) = 1.0f;
   outProjMtx( 3, 2 ) = -( nearZPlane * farZPlane ) / ( farZPlane - nearZPlane );
   outProjMtx( 3, 3 ) = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateOrthogonalProjection( float viewportWidth, float viewportHeight, float nearZPlane, float farZPlane, Matrix& outProjMtx )
{
   outProjMtx.setIdentity();
   outProjMtx( 0, 0 ) = 2.0f / viewportWidth;
   outProjMtx( 1, 1 ) = 2.0f / viewportHeight;
   outProjMtx( 2, 2 ) = 1.0f / ( farZPlane - nearZPlane );
   outProjMtx( 3, 2 ) = -nearZPlane / ( farZPlane - nearZPlane );
   outProjMtx( 3, 3 ) = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateOrthogonalProjectionOffCenter( float left, float right, float bottom, float top, float nearZPlane, float farZPlane, Matrix& outProjMtx )
{
   outProjMtx.setIdentity();
   outProjMtx( 0, 0 ) = 2.0f / ( right - left );
   outProjMtx( 3, 0 ) = -( right + left ) / ( right - left );

   outProjMtx( 1, 1 ) = 2.0f / ( top - bottom );
   outProjMtx( 3, 1 ) = -( top + bottom ) / ( top - bottom );

   outProjMtx( 2, 2 ) = 1.0f / ( farZPlane - nearZPlane );
   outProjMtx( 3, 2 ) = -nearZPlane / ( farZPlane - nearZPlane );

   outProjMtx( 3, 3 ) = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateLookAtLH( const Vector& cameraOriginPos, const Vector& lookAtPos, const Vector& upAxis, Matrix& outLookAtMtx )
{
   Vector lookVec;
   lookVec.setSub( lookAtPos, cameraOriginPos );
   lookVec.normalize();

   Vector sideVec;
   sideVec.setCross( upAxis, lookVec );
   sideVec.normalize();
   if ( sideVec.isOk() )
   {
      Vector newUpAxis;
      newUpAxis.setCross( lookVec, sideVec );
      newUpAxis.normalize();

      ASSERT( newUpAxis.isOk() );
      ASSERT( lookVec.isOk() );
      ASSERT( cameraOriginPos.isOk() );

      outLookAtMtx.setSideVec<3>( sideVec );
      outLookAtMtx.setUpVec<3>( newUpAxis );
      outLookAtMtx.setForwardVec<3>( lookVec );
      outLookAtMtx.setPosition<3>( cameraOriginPos );
      outLookAtMtx.setColumn( 3, Quad_0001 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateViewportMatrix( uint offsetX, uint offsetY, uint width, uint height, Matrix& outViewportMtx )
{
   outViewportMtx.setIdentity();
   outViewportMtx( 0, 0 ) = (float)width;
   outViewportMtx( 1, 1 ) = -(float)height;
   outViewportMtx( 3, 0 ) = (float)offsetX;
   outViewportMtx( 3, 1 ) = (float)( height + offsetY );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::calculateViewMtx( const Matrix& inMtx, Matrix& outViewMtx )
{
   Vector rightVec, upVec, lookVec, position;
   inMtx.getVectors( rightVec, upVec, lookVec, position );

   // create a view matrix
   outViewMtx.setIdentity();
   outViewMtx.setSideVec<3>( rightVec );
   outViewMtx.setUpVec<3>( upVec );
   outViewMtx.setForwardVec<3>( lookVec );
   outViewMtx.transpose();

   FastFloat d;
   
   d.setNeg( position.dot( rightVec ) );
   outViewMtx( 3, 0 ) = d.getFloat();

   d.setNeg( position.dot( upVec ) );
   outViewMtx( 3, 1 ) = d.getFloat();

   d.setNeg( position.dot( lookVec ) );
   outViewMtx( 3, 2 ) = d.getFloat();

   outViewMtx.setColumn( 3, Quad_0001 );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::regenerateAxes( Matrix& inOutTransform )
{
   Vector sideVec = inOutTransform.sideVec();
   Vector upVec = inOutTransform.upVec();
   Vector forwardVec = inOutTransform.forwardVec();

   forwardVec.normalize();
   upVec.setCross( forwardVec, sideVec );
   upVec.normalize();
   sideVec.setCross( upVec, forwardVec );
   sideVec.normalize();

   inOutTransform.setRows( sideVec, upVec, forwardVec, inOutTransform.position() );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::changeReferenceSystem( const Matrix& inTransform, const Matrix& destSystem, Matrix& outTransform )
{
   Matrix inDestSystem;
   inDestSystem.setInverse( destSystem );

   outTransform.setMul( destSystem, inTransform );
   outTransform.mul( inDestSystem );
}

///////////////////////////////////////////////////////////////////////////////
