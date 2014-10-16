#include "core-Physics\PSCapsule.h"

// physx
#include "PxRigidActor.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "geometry\PxCapsuleGeometry.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PSCapsule );
   PARENT( PhysicsShape );
   PROPERTY_EDIT( "Radius", float, m_radius );
   PROPERTY_EDIT( "Height", float, m_height );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PSCapsule::PSCapsule( float radius, float height )
   : m_radius( radius )
   , m_height( height )
{
}

///////////////////////////////////////////////////////////////////////////////

physx::PxShape* PSCapsule::instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const
{
   physx::PxShape* shape = actor.createShape( physx::PxCapsuleGeometry( m_radius, m_height * 0.5f ), material );
   return shape;
}

///////////////////////////////////////////////////////////////////////////////
