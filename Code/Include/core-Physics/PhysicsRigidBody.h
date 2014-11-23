/// @file   core-Physics\PhysicsRigidBody.h
/// @brief  a rigid body implementation
#pragma once

#include "core-Physics\PhysicsObject.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxRigidActor;
   class PxRigidDynamic;
   class PxScene;
}

class PhysicsMaterial;
class PhysicsShape;

///////////////////////////////////////////////////////////////////////////////

/**
 * NOTE ABOUT TRANSFORMS:
 * A rigid body will affect the parent entity's transform, treating it as if it was
 * a body that's attached to the scene's root.
 *
 * Make sure the entity IS attached in that configuration - there may be other entities
 * separating it in the hierarchy from the root, but they should not affect the
 * entity's transform ( all of them should have an identity transform ) - otherwise
 * the object won't be transformed correctly.
 */
class PhysicsRigidBody : public PhysicsObject
{
   DECLARE_ALLOCATOR( PhysicsRigidBody, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // runtime data
   physx::PxRigidActor*                         m_body;

private:
   // static data
   PhysicsMaterial*                             m_material;
   PhysicsShape*                                m_shape;
   float                                        m_mass;
   float                                        m_density;
   float                                        m_maxAngularVelocity;
   float                                        m_linearDamping;
   float                                        m_angularDamping;
   bool                                         m_obeyGravity;

   physx::PxRigidDynamic*                       m_dynamicBody;

private:
   // runtime data
   Matrix                                       m_cachedTransform;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   PhysicsRigidBody( const char* name = "PhysicsRigidBody" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   PhysicsRigidBody( const PhysicsRigidBody& rhs );
   ~PhysicsRigidBody();

   // -------------------------------------------------------------------------
   // Properties setup
   // -------------------------------------------------------------------------

   /**
    * Sets the new mass of the body.
    *
    * @param mass
    */
   void setMass( float mass );

   /**
    * Returns the mass of the body.
    */
   inline float getMass() const {
      return m_mass;
   }

   /**
   * Sets the new density of the body.
   *
   * @param density
   */
   void setDensity( float density );

   /**
   * Returns the density of the body.
   */
   inline float getDensity() const {
      return m_density;
   }

   /**
    * Tells if the body is dynamic and capable of movement.
    */
   inline bool isDynamic() const {
      return m_mass > 0.f;
   }

   /**
    * Makes the body obey/ignore the laws of gravity.
    *
    * @param obeyGravity
    */
   void setObeyGravity( bool obeyGravity );

   /**
    * Tells whether the body obeys the laws of gravity.
    */
   inline bool doesObeyGravity() const {
      return m_obeyGravity;
   }

   // -------------------------------------------------------------------------
   // Forces and velocities
   // -------------------------------------------------------------------------
   /**
    * Sets the linear velocity.
    *
    * @param velocity
    */
   void setLinearVelocity( const Vector& velocity );

   /**
    * Returns the linear velocity.
    *
    * @param outVelocity
    */
   void getLinearVelocity( Vector& outVelocity );

   /**
    * Sets the angular velocity.
    *
    * @param velocity
    */
   void setAngularVelocity( const Vector& velocity );

   /**
    * Returns the angular velocity.
    *
    * @param outVelocity
    */
   void getAngularVelocity( Vector& outVelocity );

   /**
    * Applies a force to the body, applied to the center of the body's mass.
    *
    * @param force
    * @param autowake         should the body be waken if it was asleep?
    */
   void addForce( const Vector& force, bool autowake = true );

   /**
   * Applies a force impulse to the body, applied to the center of the body's mass.
   *
   * @param impulse
   * @param autowake          should the body be waken if it was asleep?
   */
   void addForceImpulse( const Vector& impulse, bool autowake = true );

   /**
    * Applies a torque to the body.
    *
    * @param torque
    * @param autowake         should the body be waken if it was asleep?
    */
   void addTorque( const Vector& torque, bool autowake = true );

   /**
    * Applies a torque impulse to the body.
    *
    * @param impulse
    * @param autowake         should the body be waken if it was asleep?
    */
   void addTorqueImpulse( const Vector& impulse, bool autowake = true );

   /**
    * Applies a force to the body at the specified position expressed in world space coords.
    *
    * @param force
    * @param pos
    * @param autowake         should the body be waken if it was asleep?
    */
   void addForceAtPos( const Vector& force, const Vector& pos, bool autowake = true );

   /**
    * Applies a force impulse to the body at the specified position expressed in world space coords.
    *
    * @param impulse
    * @param pos
    * @param autowake         should the body be waken if it was asleep?
   */
   void addForceImpulseAtPos( const Vector& impulse, const Vector& pos, bool autowake = true );

   /**
   * Applies a force to the body at the specified position expressed in local space coords.
   *
   * @param force
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addForceAtLocalPos( const Vector& force, const Vector& pos, bool autowake = true );

   /**
   * Applies a force impulse to the body at the specified position expressed in local space coords.
   *
   * @param impulse
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addForceImpulseAtLocalPos( const Vector& impulse, const Vector& pos, bool autowake = true );

   /**
   * Applies a force ( expressed in local space coords ), to the body at the specified position expressed in world space coords.
   *
   * @param force
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addLocalForceAtPos( const Vector& force, const Vector& pos, bool autowake = true );

   /**
   * Applies a force impulse ( expressed in local space coords ), to the body at the specified position expressed in world space coords.
   *
   * @param impulse
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addLocalForceImpulseAtPos( const Vector& impulse, const Vector& pos, bool autowake = true );

   /**
   * Applies a force ( expressed in local space coords ), to the body at the specified position expressed in local space coords.
   *
   * @param force
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addLocalForceAtLocalPos( const Vector& force, const Vector& pos, bool autowake = true );

   /**
   * Applies a force impulse ( expressed in local space coords ), to the body at the specified position expressed in local space coords.
   *
   * @param impulse
   * @param pos
   * @param autowake         should the body be waken if it was asleep?
   */
   void addLocalForceImpulseAtLocalPos( const Vector& impulse, const Vector& pos, bool autowake = true );

   /**
    * Checks if the body is sleeping.
    */
   bool isSleeping() const;

   // -------------------------------------------------------------------------
   // PhysicsObject implementation
   // -------------------------------------------------------------------------
   void addToWorld( PhysicsWorld& world ) override;
   void removeFromWorld( PhysicsWorld& world ) override;

   // -------------------------------------------------------------------------
   // Component implementation
   // -------------------------------------------------------------------------
   void updateTransforms() override;
};

///////////////////////////////////////////////////////////////////////////////
