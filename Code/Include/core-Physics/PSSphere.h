/// @file   core-Physics\PSSphere.h
/// @brief  spherical collision shape
#pragma once

#include "core-Physics\PhysicsShape.h"


///////////////////////////////////////////////////////////////////////////////

class PSSphere : public PhysicsShape
{
   DECLARE_ALLOCATOR( PSSphere, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float             m_radius;

public:
   /**
    * Constructor.
    *
    * @param radius
    */
   PSSphere( float radius = 1.0f );

   // -------------------------------------------------------------------------
   // PhysicsShape implementation
   // -------------------------------------------------------------------------
   physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const;
};

///////////////////////////////////////////////////////////////////////////////
