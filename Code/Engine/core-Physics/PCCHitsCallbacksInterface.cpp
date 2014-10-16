#include "core-Physics\PCCHitsCallbacksInterface.h"
#include "core-Physics\PhysicsCharacterController.h"


///////////////////////////////////////////////////////////////////////////////

PCCHitsCallbacksInterface::PCCHitsCallbacksInterface( PhysicsCharacterController& component )
   : m_component( component )
{
}

///////////////////////////////////////////////////////////////////////////////

void PCCHitsCallbacksInterface::onShapeHit( const physx::PxControllerShapeHit& hit )
{
   m_component.onShapeHit( hit );
}

///////////////////////////////////////////////////////////////////////////////

void PCCHitsCallbacksInterface::onControllerHit( const physx::PxControllersHit& hit )
{
   m_component.onControllerHit( hit );
}

///////////////////////////////////////////////////////////////////////////////

void PCCHitsCallbacksInterface::onObstacleHit( const physx::PxControllerObstacleHit& hit )
{
   m_component.onObstacleHit( hit );
}

///////////////////////////////////////////////////////////////////////////////
