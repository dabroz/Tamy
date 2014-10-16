/// @file   TamyEditor/OrbitingCameraController.h
/// @brief  makes the camera orbit around the specified scene node, following it as it moves
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\EulerAngles.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

class TamySceneWidget;

///////////////////////////////////////////////////////////////////////////////

/**
 * Makes the camera orbit around the specified scene node, following it as it moves.
 */
class OrbitingCameraController
{
   DECLARE_ALLOCATOR( OrbitingCameraController, AM_ALIGNED_16 );

private:
   Vector               m_cameraArm;

   EulerAngles          m_orientation;
   float                m_pitchSpeed;
   float                m_rollSpeed;

   const FastFloat      m_orbitDistance;
   const FastFloat      m_topLinearSpeed;
   const float          m_topAngularSpeed;

public:
   /**
    * Constructor.
    *
    * @param orbitDistance
    */
   OrbitingCameraController( float orbitDistance );
   ~OrbitingCameraController();

   /**
    * Updates the controller's state.
    *
    * @param timeElapsed
    * @param focalPoint
    * @param sceneWidget
    */
   void update( float timeElapsed, const Vector& focalPoint, TamySceneWidget& sceneWidget );
};

///////////////////////////////////////////////////////////////////////////////
