#include "core-Physics\PhysicsRigidBody.h"

// physics system
#include "core-Physics\PhysicsSystem.h"
#include "core-Physics\PhysicsWorld.h"
#include "core-Physics\PhysicsMaterial.h"
#include "core-Physics\PXMathConverter.h"

// physx
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxShape.h"
#include "PxMaterial.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxSceneLock.h"
#include "extensions\PxRigidBodyExt.h"

// shapes
#include "core-Physics\PSSphere.h"

// scene
#include "core-MVC\Entity.h"

// utils
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PhysicsRigidBody );
   PARENT( PhysicsObject );
   PROPERTY_EDIT( "Material", PhysicsMaterial*, m_material );
   PROPERTY_EDIT( "Shape", PhysicsShape*, m_shape );
   PROPERTY_EDIT( "Mass", float, m_mass );
   PROPERTY_EDIT( "Density", float, m_density );
   PROPERTY_EDIT( "Max angular velocity", float, m_maxAngularVelocity );
   PROPERTY_EDIT( "Linear damping", float, m_linearDamping );
   PROPERTY_EDIT( "Angular damping", float, m_angularDamping );
   PROPERTY_EDIT( "Obey gravity", bool, m_obeyGravity );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PhysicsRigidBody::PhysicsRigidBody( const char* name )
   : PhysicsObject( name )
   , m_material( NULL )
   , m_shape( new PSSphere( 1.f ) )
   , m_mass( 1.f )
   , m_density( 1.0f )
   , m_maxAngularVelocity( 10000.f )
   , m_linearDamping( 0.1f )
   , m_angularDamping( 0.1f )
   , m_obeyGravity( true )
   , m_body( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsRigidBody::PhysicsRigidBody( const PhysicsRigidBody& rhs )
   : PhysicsObject( rhs )
   , m_material( rhs.m_material )
   , m_mass( rhs.m_mass )
   , m_density( rhs.m_density )
   , m_maxAngularVelocity( rhs.m_maxAngularVelocity )
   , m_linearDamping( rhs.m_linearDamping )
   , m_angularDamping( rhs.m_angularDamping )
   , m_obeyGravity( rhs.m_obeyGravity )
   , m_shape( NULL )
   , m_body( NULL )
   , m_dynamicBody( NULL )
{
   // clone the shape
   if ( rhs.m_shape )
   {
      const SerializableReflectionType& type = rhs.m_shape->getVirtualRTTI();
      m_shape = type.instantiate( *rhs.m_shape );
   }
   else
   {
      m_shape = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

PhysicsRigidBody::~PhysicsRigidBody()
{
   if ( m_shape )
   {
      m_shape->removeReference();
      m_shape = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::setMass( float mass )
{
   NOTIFY_PROPERTY_CHANGE( m_mass );
   m_mass = mass;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::setDensity( float density )
{
   NOTIFY_PROPERTY_CHANGE( m_density );
   m_density = density;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::setObeyGravity( bool obeyGravity )
{
   NOTIFY_PROPERTY_CHANGE( m_obeyGravity );
   m_obeyGravity = obeyGravity;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addToWorld( PhysicsWorld& world )
{
   PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
   physx::PxPhysics* physicsCore = physSystem.getPhysXInterface();

   physx::PxMaterial* material = NULL;
   if ( !m_material || !m_material->getPhysXMaterial() )
   {
      LOG( "Physics material not assigned to body %s - using a default one", getSceneNodeName().c_str() );

      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
      PhysicsMaterial* defaultMat = resMgr.create< PhysicsMaterial >( FilePath( "/Physics/Materials/defaultPhysMat.tpm" ) );
      material = defaultMat->getPhysXMaterial();
   }
   else
   {
      material = m_material->getPhysXMaterial();
   }

   physx::PxTransform pxTransform;
   PxMathConverter::convert( getGlobalMtx(), pxTransform );

   if ( m_mass <= 0.f )
   {
      m_body = physicsCore->createRigidStatic( pxTransform );

      // give the body a shape
      m_shape->instantiate( *m_body, *material );

      world.assignCollisionGroup( m_body, Collision_Static );

   }
   else
   {
      m_dynamicBody = physicsCore->createRigidDynamic( pxTransform );
      m_body = m_dynamicBody;

      // set instance properties
      m_dynamicBody->setMass( m_mass );
      m_dynamicBody->setMaxAngularVelocity( m_maxAngularVelocity );
      m_dynamicBody->setLinearDamping( m_linearDamping );
      m_dynamicBody->setAngularDamping( m_angularDamping );

      // give the body a shape and update its inertia
      m_shape->instantiate( *m_body, *material );
      physx::PxRigidBodyExt::updateMassAndInertia( *m_dynamicBody, m_density );

      world.assignCollisionGroup( m_body, Collision_Dynamic );
   }

   // set additional flags
   m_body->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, !m_obeyGravity );

   // add the actor to the world
   {
      physx::PxSceneWriteLock scopedLock( *world.m_scene );
      world.m_scene->addActor( *m_body );
   }

   // cache the parent entity's transform
   m_cachedTransform = getGlobalMtx();
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::removeFromWorld( PhysicsWorld& world )
{
   if ( m_body )
   {
      physx::PxSceneWriteLock scopedLock( *world.m_scene );

      world.m_scene->removeActor( *m_body );

      m_body->release();
      m_body = NULL;
      m_dynamicBody = NULL;
   }

   m_material = NULL;

   // reset parent entity's matrix
   Entity* parent = getParent();
   parent->setLocalMtx( m_cachedTransform );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::updateTransforms()
{
   if ( m_body )
   {
      Entity* parent = getParent();

      physx::PxSceneReadLock scopedLock( *m_body->getScene() );

      Matrix globalMtx;
      PxMathConverter::convert( m_body->getGlobalPose(), globalMtx );
      parent->setLocalMtx( globalMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::setLinearVelocity( const Vector& velocity )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxVel;
      PxMathConverter::convert( velocity, pxVel );
      m_dynamicBody->setLinearVelocity( pxVel );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::getLinearVelocity( Vector& outVelocity )
{
   if ( m_dynamicBody )
   {
      PxMathConverter::convert( m_dynamicBody->getLinearVelocity(), outVelocity );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::setAngularVelocity( const Vector& velocity )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxVel;
      PxMathConverter::convert( velocity, pxVel );
      m_dynamicBody->setAngularVelocity( pxVel );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::getAngularVelocity( Vector& outVelocity )
{
   if ( m_dynamicBody )
   {
      PxMathConverter::convert( m_dynamicBody->getAngularVelocity(), outVelocity );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForce( const Vector& force, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxForce;
      PxMathConverter::convert( force, pxForce );

      m_dynamicBody->addForce( pxForce, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForceImpulse( const Vector& impulse, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse;
      PxMathConverter::convert( impulse, pxImpulse );

      m_dynamicBody->addForce( pxImpulse, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addTorque( const Vector& force, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxTorque;
      PxMathConverter::convert( force, pxTorque );

      m_dynamicBody->addTorque( pxTorque, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addTorqueImpulse( const Vector& impulse, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse;
      PxMathConverter::convert( impulse, pxImpulse );

      m_dynamicBody->addTorque( pxImpulse, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForceAtPos( const Vector& force, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxForce, pxPos;
      PxMathConverter::convert( force, pxForce );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addForceAtPos( *m_dynamicBody, pxForce, pxPos, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForceImpulseAtPos( const Vector& impulse, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse, pxPos;
      PxMathConverter::convert( impulse, pxImpulse );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addForceAtPos( *m_dynamicBody, pxImpulse, pxPos, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForceAtLocalPos( const Vector& force, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxForce, pxPos;
      PxMathConverter::convert( force, pxForce );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addForceAtLocalPos( *m_dynamicBody, pxForce, pxPos, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addForceImpulseAtLocalPos( const Vector& impulse, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse, pxPos;
      PxMathConverter::convert( impulse, pxImpulse );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addForceAtLocalPos( *m_dynamicBody, pxImpulse, pxPos, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addLocalForceAtPos( const Vector& force, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxForce, pxPos;
      PxMathConverter::convert( force, pxForce );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addLocalForceAtPos( *m_dynamicBody, pxForce, pxPos, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addLocalForceImpulseAtPos( const Vector& impulse, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse, pxPos;
      PxMathConverter::convert( impulse, pxImpulse );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addLocalForceAtPos( *m_dynamicBody, pxImpulse, pxPos, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////


void PhysicsRigidBody::addLocalForceAtLocalPos( const Vector& force, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxForce, pxPos;
      PxMathConverter::convert( force, pxForce );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addLocalForceAtLocalPos( *m_dynamicBody, pxForce, pxPos, physx::PxForceMode::eFORCE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsRigidBody::addLocalForceImpulseAtLocalPos( const Vector& impulse, const Vector& pos, bool autowake )
{
   if ( m_dynamicBody )
   {
      physx::PxVec3 pxImpulse, pxPos;
      PxMathConverter::convert( impulse, pxImpulse );
      PxMathConverter::convert( pos, pxPos );

      physx::PxRigidBodyExt::addLocalForceAtLocalPos( *m_dynamicBody, pxImpulse, pxPos, physx::PxForceMode::eIMPULSE, autowake );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool PhysicsRigidBody::isSleeping() const
{
   if ( m_dynamicBody )
   {
      return m_dynamicBody->isSleeping();
   }
   else
   {
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
