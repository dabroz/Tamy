#include "TamyCameraController.h"
#include "core-Renderer\Camera.h"
#include "core\Math.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

TamyCameraController::TamyCameraController( Camera& controlledNode )
   : m_controlledNode( controlledNode )
{
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::setPosition( const Vector& vec )
{
   Vector currentGlobalPos;
   getPosition( currentGlobalPos );
   Matrix newLocalMtx = m_controlledNode.getLocalMtx();

   Vector newPos;
   newPos.setSub( vec, currentGlobalPos );
   newPos.setAdd( newLocalMtx.position(), newPos );
   newLocalMtx.setPosition<3>( newPos );

   m_controlledNode.setLocalMtx( newLocalMtx );
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::getRightVec( Vector& outVec ) const
{
   const Matrix& globalMtx = m_controlledNode.getGlobalMtx();
   outVec = globalMtx.sideVec();
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::getUpVec( Vector& outVec ) const
{
   const Matrix& globalMtx = m_controlledNode.getGlobalMtx();
   outVec = globalMtx.upVec();
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::getLookVec( Vector& outVec ) const
{
   const Matrix& globalMtx = m_controlledNode.getGlobalMtx();
   outVec = globalMtx.forwardVec();
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::getPosition( Vector& outVec ) const
{
   const Matrix& globalMtx = m_controlledNode.getGlobalMtx();
   outVec = globalMtx.position();
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::regenerateVectors( Vector& rightVec, Vector& upVec, Vector& lookVec ) const
{
   lookVec.normalize();
   upVec.setCross( lookVec, rightVec );
   upVec.normalize();
   rightVec.setCross( upVec, lookVec );
   rightVec.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void TamyCameraController::calcRotationMtx( float pitch, float yaw, const Matrix& currRotationMtx, Matrix& outMtx )
{
   Vector rightVec = currRotationMtx.sideVec();
   Vector lookVec = currRotationMtx.forwardVec();

   Quaternion currRotationQuat;
   currRotationMtx.getRotation( currRotationQuat );

   if ( pitch )
   {
      Vector lookVec2D;
      lookVec2D.setCross( rightVec, Quad_0010 );

      FastFloat currPitch = Float_0;
      {
         FastFloat currPitchDot = lookVec2D.dot( lookVec );
         currPitchDot.setClamped( currPitchDot, Float_Minus1, Float_1 );
         currPitchDot.acos();

         FastFloat dot;
         dot.setNeg( lookVec.dot( Quad_0010 ) );
         currPitch.setFlipSign( currPitchDot, dot );
         currPitch.mul( Float_Rad2Deg );
      }

      static FastFloat maxPosPitch = FastFloat::fromFloat( 89.0f );
      static FastFloat maxNegPitch = FastFloat::fromFloat( -89.0f );

      FastFloat newPitch;
      newPitch.setAdd( currPitch, FastFloat::fromFloat( pitch ) );
      newPitch.setClamped( newPitch, maxNegPitch, maxPosPitch );
      newPitch.mul( Float_Deg2Rad );

      Quaternion rotQ;
      rotQ.setAxisAngle( rightVec, newPitch );

      Vector tmpVec;
      rotQ.transform( lookVec2D, lookVec );
   }

   if ( yaw )
   {
      FastFloat simdYaw;
      simdYaw.setFromFloat( yaw );
      simdYaw.mul( Float_Deg2Rad );

      Quaternion rotQ;
      rotQ.setAxisAngle( Quad_0010, simdYaw );

      Vector tmpVec;
      rotQ.transform( rightVec, tmpVec ); rightVec = tmpVec;
      rotQ.transform( lookVec, tmpVec ); lookVec = tmpVec;
   }

   // regenerate vectors
   Vector upVec = Quad_0010;
   regenerateVectors( rightVec, upVec, lookVec );

   outMtx.setSideVec<3>( rightVec );
   outMtx.setUpVec<3>( upVec );
   outMtx.setForwardVec<3>( lookVec );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UnconstrainedMotionController::UnconstrainedMotionController( Camera& controlledNode )
   : TamyCameraController( controlledNode )
{
}

///////////////////////////////////////////////////////////////////////////////

void UnconstrainedMotionController::move( const Vector& transVec )
{
   Vector currPos;
   getPosition( currPos );
   currPos.add( transVec );
   setPosition( currPos );
}

///////////////////////////////////////////////////////////////////////////////

void UnconstrainedMotionController::rotate( float pitch, float yaw )
{
   calcRotationMtx( pitch, yaw, m_controlledNode.getGlobalMtx(), m_controlledNode.accessLocalMtx() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

OrbitingMotionController::OrbitingMotionController( Camera& controlledNode )
   : TamyCameraController( controlledNode )
{
}

///////////////////////////////////////////////////////////////////////////////

void OrbitingMotionController::move( const Vector& transVec )
{
   Vector currPos;
   getPosition( currPos );
   currPos.add( transVec );
   setPosition( currPos );
}

///////////////////////////////////////////////////////////////////////////////

void OrbitingMotionController::rotate( float pitch, float yaw )
{
   const Matrix& globalMtx = m_controlledNode.getGlobalMtx();

   // get the orbit distance - it's the distance to the XY plane that passes through the origin
   // along the current look vector
   
   FastFloat orbitDist;
   {
      Ray ray( globalMtx.position(), globalMtx.forwardVec() );
      Plane plane; plane.set( Vector_OZ );
      RaycastResult result;
      bool wasHit = ray.rayCast( plane, result );
      
      if ( wasHit )
      {
         orbitDist.setMin( result.m_dist, FastFloat::fromFloat( 100.0f ) );
      }
      else
      {
         // the camera was looking up and not at the ground - so hover around a fixed point in space
         orbitDist.setFromFloat( 10.0f );
      }
   }

   // get rotation pivot
   Vector rotationPivot;
   rotationPivot.setMulAdd( globalMtx.forwardVec(), orbitDist, globalMtx.position() );

   // calculate the new camera position
   Matrix newRotMtx;
   calcRotationMtx( pitch, yaw, m_controlledNode.getGlobalMtx(), newRotMtx );

   Vector newCamPos;
   newCamPos.setMulAdd( newRotMtx.forwardVec(), orbitDist * Float_Minus1, rotationPivot );

   // calculate the look at matrix
   Matrix& camMtx = m_controlledNode.accessLocalMtx();
   camMtx = newRotMtx;
   camMtx.setPosition<3>( newCamPos );
}

///////////////////////////////////////////////////////////////////////////////
