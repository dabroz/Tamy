/// @file   core-Physics\PhysicsCharacterController.h
/// @brief  a physics-based character controller
#pragma once

#include "core-Physics\PhysicsObject.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxControllerManager;
   class PxController;

   struct PxControllerShapeHit;
   struct PxControllersHit;
   struct PxControllerObstacleHit;
}

class PCCHitsCallbacksInterface;
class PhysicsMaterial;

///////////////////////////////////////////////////////////////////////////////

class PhysicsCharacterController : public PhysicsObject
{
   DECLARE_ALLOCATOR( PhysicsCharacterController, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   PhysicsMaterial*                 m_material;
   float                            m_height;
   float                            m_radius;
   float                            m_jumpHeight;
   float                            m_slopeLimit;
   float                            m_maxObstacleHeight;

   // runtime data
   physx::PxController*             m_controller;
   PCCHitsCallbacksInterface*       m_hitsCallback;
   Vector                           m_linearVelocity;
   Vector                           m_angularVelocity;
   Quaternion                       m_orientation;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   PhysicsCharacterController( const char* name = "PhysicsCharacterController" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   PhysicsCharacterController( const PhysicsCharacterController& rhs );
   ~PhysicsCharacterController();

   // -------------------------------------------------------------------------
   // Movement
   // -------------------------------------------------------------------------

   /**
    * Sets the new desired linear velocity of the character.
    *
    * @param velocity
    */
   void setLinearVelocity( const Vector& velocity );

   /**
   * Sets the new desired angular velocity of the character.
   *
   * @param velocity
   */
   void setAngularVelocity( const Vector& velocity );

   /**
    * Accumulates the specified linear velocity value, adding it to the velocity that's
    * currently set on the character.
    *
    * @param velocity
    */
   void addLinearVelocity( const Vector& velocity );

   /**
    * 'Teleports' the character to the specified position.
    *
    * @param pos
    */
   void teleport( const Vector& pos );

   /**
    * Returns the linear velocity of the character.
    */
   inline const Vector& getLinearVelocity() const {
      return m_linearVelocity;
   };

   /**
   * Returns the angular velocity of the character.
   */
   inline const Vector& getAngularVelocity() const {
      return m_angularVelocity;
   };

   // -------------------------------------------------------------------------
   // PhysicsObject implementation
   // -------------------------------------------------------------------------
   void addToWorld( PhysicsWorld& world );
   void removeFromWorld( PhysicsWorld& world );
   void tick( float deltaTime );

   // -------------------------------------------------------------------------
   // Component implementation
   // -------------------------------------------------------------------------
   void updateTransforms();

   // -------------------------------------------------------------------------
   // controller hits callback interface
   // -------------------------------------------------------------------------
   void onShapeHit( const physx::PxControllerShapeHit& hit );
   void onControllerHit( const physx::PxControllersHit& hit );
   void onObstacleHit( const physx::PxControllerObstacleHit& hit );
};

///////////////////////////////////////////////////////////////////////////////
