#include "OrbitingCameraController.h"
#include "TamySceneWidget.h"
#include "core-Renderer\Camera.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

OrbitingCameraController::OrbitingCameraController( float orbitDistance )
   : m_pitchSpeed( 0.0f )
   , m_rollSpeed( 0.0f )
   , m_orbitDistance( FastFloat::fromFloat( orbitDistance ) )
   , m_topLinearSpeed( FastFloat::fromFloat( 20.0f ) )
   , m_topAngularSpeed( 90.0f )
{
   m_orientation.set( 0.0f, 45.0f, 0.0f );
}

///////////////////////////////////////////////////////////////////////////////

OrbitingCameraController::~OrbitingCameraController()
{
}

///////////////////////////////////////////////////////////////////////////////

void OrbitingCameraController::update( float timeElapsed, const Vector& focalPoint, TamySceneWidget& sceneWidget )
{
   if ( fabs( timeElapsed ) < 1e-7f )
   {
      return;
   }

   Camera& camera = sceneWidget.getCamera();
   FastFloat invDt = FastFloat::fromFloat( 1.0f / timeElapsed );
   FastFloat dt = FastFloat::fromFloat( timeElapsed );

   // update orientation of the camera's arm
   Vector desiredCameraPos;
   {
      UserInputController& uic = sceneWidget.getUserInputController();
      if ( uic.isKeyPressed( VK_RBUTTON ) )
      {
         // accelerate
         const Point& mousePos = uic.getMousePos();

         const float w = (float)sceneWidget.width();
         const float h = (float)sceneWidget.height();
         const float x = (float)mousePos.x;
         const float y = (float)mousePos.y;
         float rollSpeed = ( 0.5f - x / w );
         float pitchSpeed = ( 0.5f - y / h );
         if ( rollSpeed > -0.3 && rollSpeed < 0.3 )
         {
            rollSpeed = 0.0f;
         }
         if ( pitchSpeed > -0.3 && pitchSpeed < 0.3 )
         {
            pitchSpeed = 0.0f;
         }

         m_rollSpeed = clamp< float >( m_rollSpeed + rollSpeed, -m_topAngularSpeed, m_topAngularSpeed );
         m_pitchSpeed = clamp< float >( m_pitchSpeed + pitchSpeed, -m_topAngularSpeed, m_topAngularSpeed );
      }
      else
      {
         // decelerate
         float rollSpeedSign = m_rollSpeed >= 0.0f ? 1.0f : -1.0f;
         float rollDecceleration = min2< float >( 1.0f, fabs( m_rollSpeed ) );
         m_rollSpeed -= rollSpeedSign * rollDecceleration;

         float pitchSpeedSign = m_pitchSpeed >= 0.0f ? 1.0f : -1.0f;
         float pitchDecceleration = min2< float >( 1.0f, fabs( m_pitchSpeed ) );
         m_pitchSpeed -= pitchSpeedSign * pitchDecceleration;
      }


      // apply new orientation
      m_orientation[EulerAngles::Roll] += m_rollSpeed * timeElapsed;
      m_orientation[EulerAngles::Pitch] += m_pitchSpeed * timeElapsed;
      m_orientation.normalize();

      // make sure pitch doesn't exceed certain limits
      m_orientation[EulerAngles::Pitch] = clamp< float >( m_orientation[EulerAngles::Pitch], 10.0f, 70.0f );

      Quaternion rollQuat;
      rollQuat.setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( m_orientation.getRoll() ) ) );
      
      Quaternion pitchQuat;
      pitchQuat.setAxisAngle( Vector_OX, FastFloat::fromFloat( DEG2RAD( m_orientation.getPitch() ) ) );

      Quaternion orientQuat;
      orientQuat.setMul( pitchQuat, rollQuat );
      
      orientQuat.transform( Vector_OY, desiredCameraPos );
      desiredCameraPos.mul( m_orbitDistance );
   }

   // update camera position
   Vector newCameraPos;
   {
      Vector cameraPos;
      camera.getPosition( cameraPos );

      Vector cameraDisplacement;
      cameraDisplacement.setSub( desiredCameraPos, cameraPos );

      Vector cameraVelocity;
      VectorUtil::limitLength( cameraDisplacement, m_topLinearSpeed, cameraVelocity );

      FastFloat timeScale;
      timeScale.setMul( dt, Float_5 );

      newCameraPos.setMulAdd( cameraVelocity, timeScale, cameraPos );
   }

   // set the new transform on the camera
   {
      Matrix newCameraMtx;
      MatrixUtils::generateLookAtLH( newCameraPos, focalPoint, Vector_OZ, newCameraMtx );

      camera.setLocalMtx( newCameraMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

