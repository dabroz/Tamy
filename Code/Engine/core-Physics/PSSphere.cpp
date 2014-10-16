#include "core-Physics\PSSphere.h"

// physx
#include "PxRigidActor.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "geometry\PxSphereGeometry.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PSSphere );
   PARENT( PhysicsShape );
   PROPERTY_EDIT( "Radius", float, m_radius );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PSSphere::PSSphere( float radius )
   : m_radius( radius )
{
}

///////////////////////////////////////////////////////////////////////////////

physx::PxShape* PSSphere::instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const
{
   physx::PxShape* shape = actor.createShape( physx::PxSphereGeometry( m_radius ), material );
   return shape;
}

///////////////////////////////////////////////////////////////////////////////
