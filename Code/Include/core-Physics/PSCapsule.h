/// @file   core-Physics\PSCapsule.h
/// @brief  a capsule collision shape
#pragma once

#include "core-Physics\PhysicsShape.h"


///////////////////////////////////////////////////////////////////////////////

class PSCapsule : public PhysicsShape
{
   DECLARE_ALLOCATOR( PSCapsule, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                m_height;
   float                m_radius;

public:
   /**
   * Constructor.
   *
   * @param height
   * @param radius
   */
   PSCapsule( float height = 1.8f, float radius = 0.4f );

   // -------------------------------------------------------------------------
   // PhysicsShape implementation
   // -------------------------------------------------------------------------
   physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const;
};

///////////////////////////////////////////////////////////////////////////////
