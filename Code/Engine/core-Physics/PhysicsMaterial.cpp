#include "core-Physics\PhysicsMaterial.h"

// physx
#include "PxMaterial.h"
#include "PxPhysics.h"

// physics system
#include "core-Physics\PhysicsSystem.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( PhysicsMaterial, tpm, AM_BINARY );
   PROPERTY_EDIT( "Static friction", float, m_staticFriction );
   PROPERTY_EDIT( "Dynamic friction", float, m_dynamicFriction );
   PROPERTY_EDIT( "Restitution", float, m_restitution );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

PhysicsMaterial::PhysicsMaterial( const FilePath& path )
   : Resource( path )
   , m_staticFriction( 0.5f )
   , m_dynamicFriction( 0.5f )
   , m_restitution( 0.5f )
   , m_material( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsMaterial::~PhysicsMaterial()
{
   PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
   physSystem.removeMaterial( this );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::setStaticFriction( float friction )
{
   m_staticFriction = friction;

   if ( m_material )
   {
      m_material->setStaticFriction( m_staticFriction );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::setDynamicFriction( float friction )
{
   m_dynamicFriction = friction;

   if ( m_material )
   {
      m_material->setDynamicFriction( m_dynamicFriction );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::setRestitution( float restitution )
{
   m_restitution = restitution;

   if ( m_material )
   {
      m_material->setRestitution( m_restitution );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::replaceContents( Resource& rhs )
{
   PhysicsMaterial& rhsPhysicsMaterial = static_cast< PhysicsMaterial& >( rhs );

   m_staticFriction = rhsPhysicsMaterial.m_staticFriction;
   m_dynamicFriction = rhsPhysicsMaterial.m_dynamicFriction;
   m_restitution = rhsPhysicsMaterial.m_restitution;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::onResourceLoaded( ResourcesManager& mgr )
{
   Resource::onResourceLoaded( mgr );

   PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
   physSystem.addMaterial( this );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::createPhysXMaterial()
{
   if ( !m_material )
   {
      // this is a brand new material

      PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
      physx::PxPhysics* physicsCore = physSystem.getPhysXInterface();

      m_material = physicsCore->createMaterial( m_staticFriction, m_dynamicFriction, m_restitution );
      if ( !m_material )
      {
         ASSERT_MSG( false, "createMaterial failed!" );
      }
   }
   else
   {
      // update the existing material
      m_material->setStaticFriction( m_staticFriction );
      m_material->setDynamicFriction( m_dynamicFriction );
      m_material->setRestitution( m_restitution );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterial::releasePhysXMaterial()
{
   if ( m_material )
   {
      m_material->release();
      m_material = NULL;
   }

}

///////////////////////////////////////////////////////////////////////////////
