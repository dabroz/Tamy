#include "core-Renderer\Camera.h"
#include "core-MVC\Entity.h"
#include "core\Frustum.h"
#include "core\AxisAlignedBox.h"
#include "core\Ray.h"
#include "core\Assert.h"
#include "core\Matrix.h"
#include "core\MatrixUtils.h"
#include "core\MathDefs.h"
#include "core\Profiler.h"


///////////////////////////////////////////////////////////////////////////////

Camera::Camera( const char* name, ProjectionType projectionType )
   : m_projectionType( projectionType )
   , m_fov( DEG2RAD( 60.0f ) )
   , m_aspectRatio( 1.3333f )
   , m_nearPlaneWidth( 1 )
   , m_nearPlaneHeight( 1 / 1.3333f )
   , m_nearZPlane( 1.01f )
   , m_farZPlane( 1024.0f )
   , m_mtxProjectionDirty( true )
{
   m_mtxView.setIdentity();
   m_mtx3DProjection.setIdentity();
}

///////////////////////////////////////////////////////////////////////////////

Camera::~Camera()
{
}

///////////////////////////////////////////////////////////////////////////////

void Camera::onViewportResized( uint width, uint height )
{
   setNearPlaneDimensions( ( float ) width, ( float ) height );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setProjection( ProjectionType projectionType )
{
   m_projectionType = projectionType;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::updateTransforms()
{
   PROFILED();

   // lock the new world transform ( the local transform may change many times during a single frame,
   // but we want the rest of the code to be using the same value that was locked for the duration
   // of the frame )
   m_globalMtx = m_localMtx;

   updateViewMtx();

   if ( m_mtxProjectionDirty )
   {
      updateProjectionMtx();
   }

}

///////////////////////////////////////////////////////////////////////////////

const Matrix& Camera::getViewMtx() const
{
   return m_mtxView;
}

///////////////////////////////////////////////////////////////////////////////

const Matrix& Camera::getProjectionMtx() const
{
   return m_mtx3DProjection;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setNearPlaneDimensions(float width, float height)
{
   if (width < 0) width = 0;
   if (height < 1) height = 1;

   m_nearPlaneWidth = width;
   m_nearPlaneHeight = height;

   m_aspectRatio = width / height; 
   m_mtxProjectionDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getNearPlaneDimensions( float& outWidth, float& outHeight ) const
{
   outWidth = m_nearPlaneWidth;
   outHeight = m_nearPlaneHeight;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setClippingPlanes(float nearZPlane, float farZPlane) 
{
   m_nearZPlane = nearZPlane; 
   m_farZPlane = farZPlane; 
   m_mtxProjectionDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getClippingPlanes( float& outNearZPlane, float& outFarZPlane ) const
{
   outNearZPlane = m_nearZPlane;
   outFarZPlane = m_farZPlane;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setFOV(float angle)
{
   m_fov = DEG2RAD(angle);
   m_mtxProjectionDirty = true;
}

///////////////////////////////////////////////////////////////////////////////

float Camera::getFOV() const 
{
   return RAD2DEG(m_fov);
}

///////////////////////////////////////////////////////////////////////////////

void Camera::updateViewMtx()
{
   PROFILED();

   const Matrix& globalMtx = getGlobalMtx();
   MatrixUtils::calculateViewMtx( globalMtx, m_mtxView );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::updateProjectionMtx()
{  
   PROFILED();

   switch( m_projectionType )
   {
   case PT_PERSPECTIVE:
      {
         MatrixUtils::generatePrespectiveProjection( m_fov, m_aspectRatio, m_nearZPlane, m_farZPlane, m_mtx3DProjection );
         break;
      }

   case PT_ORTHO:
      {
         // we want the viewport to span from -1 to 1 on both axes, so we specify 2 as the size of each of the viewport dimensions
         MatrixUtils::generateOrthogonalProjection( m_nearPlaneWidth, m_nearPlaneHeight, m_nearZPlane, m_farZPlane, m_mtx3DProjection );

         break;
      }
   }

   m_mtxProjectionDirty = false;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::calculateFrustum( Frustum& outFrustum )
{
   const Matrix& viewMtx = getViewMtx();
   const Matrix& projMtx = getProjectionMtx();

   Matrix combinedMtx;
   combinedMtx.setMul( viewMtx, projMtx );
   combinedMtx.transpose();

   Vector planeVec;

   planeVec.setAdd( combinedMtx.position(), combinedMtx.sideVec() );
   planeVec.store( outFrustum.planes[FP_LEFT].m_quad );
   outFrustum.planes[FP_LEFT].normalize();

   planeVec.setSub( combinedMtx.position(), combinedMtx.sideVec() );
   planeVec.store( outFrustum.planes[FP_RIGHT].m_quad );
   outFrustum.planes[FP_RIGHT].normalize();

   planeVec.setAdd( combinedMtx.position(), combinedMtx.upVec() );
   planeVec.store( outFrustum.planes[FP_BOTTOM].m_quad );
   outFrustum.planes[FP_BOTTOM].normalize();

   planeVec.setSub( combinedMtx.position(), combinedMtx.upVec() );
   planeVec.store( outFrustum.planes[FP_TOP].m_quad );
   outFrustum.planes[FP_TOP].normalize();

   planeVec.setSub( combinedMtx.position(), combinedMtx.forwardVec() );
   planeVec.store( outFrustum.planes[FP_FAR].m_quad );
   outFrustum.planes[FP_FAR].normalize();

   combinedMtx.forwardVec().store( outFrustum.planes[FP_NEAR].m_quad );
   outFrustum.planes[FP_NEAR].normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::calculateFrustumAABB( AxisAlignedBox& outAABB ) const
{
   // calculate a bounding box around the frustum in camera's local space
   AxisAlignedBox localSpaceAABB;
   {
      switch( m_projectionType )
      {
         case PT_PERSPECTIVE:
         {
            float y = tan( m_fov * 0.5f ) * m_farZPlane;
            float x = y * m_aspectRatio;

            localSpaceAABB.min.set( -x, -y, m_nearZPlane );
            localSpaceAABB.max.set(  x,  y, m_farZPlane );
            break;
         }

         case PT_ORTHO:
         {
            float x = m_nearPlaneWidth * 0.5f;
            float y = m_nearPlaneHeight * 0.5f;
            localSpaceAABB.min.set( -x, -y, m_nearZPlane );
            localSpaceAABB.max.set(  x,  y, m_farZPlane );
            break;
         }
      }
   }


   // transform the box to global space
   const Matrix& globalMtx = getGlobalMtx();
   localSpaceAABB.transform( globalMtx, outAABB );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::calculateFrustumAABBFromRange( float nearZ, float farZ, AxisAlignedBox& outAABB ) const
{
   nearZ = max2( m_nearZPlane, nearZ );
   farZ = min2( m_farZPlane, farZ );

   // calculate a bounding box around the frustum in camera's local space
   AxisAlignedBox localSpaceAABB;
   {
      switch ( m_projectionType )
      {
      case PT_PERSPECTIVE:
      {
         float y = tan( m_fov * 0.5f ) * farZ;
         float x = y * m_aspectRatio;

         localSpaceAABB.min.set( -x, -y, nearZ );
         localSpaceAABB.max.set( x, y, farZ );
         break;
      }

      case PT_ORTHO:
      {
         float x = m_nearPlaneWidth * 0.5f;
         float y = m_nearPlaneHeight * 0.5f;
         localSpaceAABB.min.set( -x, -y, nearZ );
         localSpaceAABB.max.set( x, y, farZ );
         break;
      }
      }
   }


   // transform the box to global space
   const Matrix& globalMtx = getGlobalMtx();
   localSpaceAABB.transform( globalMtx, outAABB );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::splitFrustum( const Array< float >& clippingRanges, Array< Vector >& outFrustumPoints ) const
{
   if ( clippingRanges.size() < 2 )
   {
      // not enough ranges defined
      return;
   }
   ASSERT_MSG( outFrustumPoints.size() >= ( clippingRanges.size() - 1 ) * 8, "Not enough room reserved in outFrustumPoints array to accomodate all results" );

   const Matrix& projMtx = getProjectionMtx();
   Matrix viewProj, invViewProj;
   viewProj.setMul( getViewMtx(), projMtx );
   invViewProj.setInverse( viewProj );

   // initial step
   float nearZClipSpace, farZClipSpace;
   Vector tmpVec;
   projMtx.transform4( Vector( 0.0f, 0.0f, clippingRanges[0], 1.0f ), tmpVec );
   farZClipSpace = tmpVec[3] != 0.0f ? tmpVec[2] / tmpVec[3] : 0.0f;

   Vector clipSpacePoints[8] = {
      Vector( -1.0f,  1.0f, 0.0f ),
      Vector(  1.0f,  1.0f, 0.0f ),
      Vector(  1.0f, -1.0f, 0.0f ),
      Vector( -1.0f, -1.0f, 0.0f ),
      Vector( -1.0f,  1.0f, 0.0f ),
      Vector(  1.0f,  1.0f, 0.0f ),
      Vector(  1.0f, -1.0f, 0.0f ),
      Vector( -1.0f, -1.0f, 0.0f )
   };

   uint outputPointIdx = 0;
   const uint rangesCount = clippingRanges.size();
   for ( uint i = 1; i < rangesCount; ++i )
   {
      nearZClipSpace = farZClipSpace; // reuse the range calculated during the previous step

      projMtx.transform4( Vector( 0.0f, 0.0f, clippingRanges[i], 1.0f ), tmpVec );
      farZClipSpace = tmpVec[3] != 0.0f ? tmpVec[2] / tmpVec[3] : 0.0f;

      for ( byte b = 0; b < 4; ++b )
      {
         clipSpacePoints[b][2] = nearZClipSpace;
         invViewProj.transform( clipSpacePoints[b], tmpVec );
         outFrustumPoints[outputPointIdx++].setDiv( tmpVec, tmpVec.getComponent( 3 ) );
      }

      for ( byte b = 4; b < 8; ++b )
      {
         clipSpacePoints[b][2] = farZClipSpace;
         invViewProj.transform( clipSpacePoints[b], tmpVec );
         outFrustumPoints[outputPointIdx++].setDiv( tmpVec, tmpVec.getComponent( 3 ) );
      }

   }
}

///////////////////////////////////////////////////////////////////////////////

void Camera::createRay( float viewportX, float viewportY, Ray& outRay )
{
   // now I treat the mouse position as if it was located on the near clipping plane
   Vector mouseOnNearPlane;
   mouseOnNearPlane.set( viewportX, viewportY, -m_nearZPlane );

   // these 3d coords are in the viewport space - now I need to transform them into world space
   const Matrix& viewMtx = getViewMtx();
   const Matrix& projMtx = getProjectionMtx();

   Matrix combinedMtx;
   combinedMtx.setMul( viewMtx, projMtx );
   combinedMtx.invert();

   // once I have the backwards transformation, I use it on the 3D mouse coord
   Vector projMouseOnNearPlane;
   combinedMtx.transform( mouseOnNearPlane, projMouseOnNearPlane );

   // now I just need to find the vector between this point and the camera world space position and normalize it, and I should get my direction:
   const Matrix& globalMtx = getGlobalMtx();
   outRay.origin = globalMtx.position();
   outRay.direction.setSub( projMouseOnNearPlane, outRay.origin );
   outRay.direction.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::lookAt( Entity& node, const FastFloat& distance, const Vector& upVec )
{
   const Matrix& targetNodeMtx = node.getGlobalMtx();
   const Vector& targetNodePos = targetNodeMtx.position();
   const Vector& targetNodeLookVec = targetNodeMtx.forwardVec();

   // I used to normalize the look vector here, but is there a need to do it really? we assume
   // it's always normalized
   ASSERT_MSG( targetNodeLookVec.isNormalized(), "Look vector not normalized" );
   FastFloat negDist;
   negDist.setNeg( distance );
   Vector newPosition;
   newPosition.setMul( targetNodeLookVec, negDist );
   newPosition.add( targetNodePos );

   Matrix lookAtMtx;
   MatrixUtils::generateLookAtLH( newPosition, targetNodePos, upVec, lookAtMtx );
   setLocalMtx( lookAtMtx );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setLocalMtx( const Matrix& localMtx )
{
   m_localMtx = localMtx;
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setRightVec( const Vector& vec )
{
   m_localMtx.setSideVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setUpVec( const Vector& vec )
{
   m_localMtx.setUpVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setLookVec( const Vector& vec )
{
   m_localMtx.setForwardVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::setPosition( const Vector& vec )
{
   m_localMtx.setPosition<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getRightVec( Vector& outRightVec ) const
{
   outRightVec = m_localMtx.sideVec();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getUpVec( Vector& outUpVec ) const
{
   outUpVec = m_localMtx.upVec();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getLookVec( Vector& outLookVec ) const
{
   outLookVec = m_localMtx.forwardVec();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getPosition( Vector& outPos ) const
{
   outPos = m_localMtx.position();
}

///////////////////////////////////////////////////////////////////////////////

void Camera::getGlobalVectors( Vector& outRightVec, Vector& outUpVec, Vector& outLookVec, Vector& outPos ) const
{
   m_globalMtx.getVectors( outRightVec, outUpVec, outLookVec, outPos );
}

///////////////////////////////////////////////////////////////////////////////
