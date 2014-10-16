/// @file   core-Physics\PCCHitsCallbacksInterface.h
/// @brief  physics character controller collision callback
#pragma once

#include "core\MemoryRouter.h"
#include "characterkinematic\PxController.h"


///////////////////////////////////////////////////////////////////////////////

class PhysicsCharacterController;

///////////////////////////////////////////////////////////////////////////////

class PCCHitsCallbacksInterface : public physx::PxUserControllerHitReport
{
   DECLARE_ALLOCATOR( PCCHitsCallbacksInterface, AM_DEFAULT );

private:
   PhysicsCharacterController&            m_component;

public:
   /**
   * Constructor.
   *
   * @param component
   */
   PCCHitsCallbacksInterface( PhysicsCharacterController& component );

   // -------------------------------------------------------------------------
   // physx::PxUserControllerHitReport implementation
   // -------------------------------------------------------------------------
   void onShapeHit( const physx::PxControllerShapeHit& hit );
   void onControllerHit( const physx::PxControllersHit& hit );
   void onObstacleHit( const physx::PxControllerObstacleHit& hit );
};

///////////////////////////////////////////////////////////////////////////////
