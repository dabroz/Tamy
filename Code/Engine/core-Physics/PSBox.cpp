#include "core-Physics\PSBox.h"

// physx
#include "PxRigidActor.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "geometry\PxBoxGeometry.h"

// utils
#include "core-Physics\PxMathConverter.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PSBox );
   PARENT( PhysicsShape );
   PROPERTY_EDIT( "Extents", Vector, m_halfExtents );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PSBox::PSBox( const Vector& halfExtents )
   : m_halfExtents( halfExtents )
{
}

///////////////////////////////////////////////////////////////////////////////

physx::PxShape* PSBox::instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const
{
   physx::PxVec3 pxHalfExtents;
   PxMathConverter::convert( m_halfExtents, pxHalfExtents );

   physx::PxShape* shape = actor.createShape( physx::PxBoxGeometry( pxHalfExtents ), material );
   return shape;
}

///////////////////////////////////////////////////////////////////////////////
