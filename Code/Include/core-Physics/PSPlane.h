/// @file   core-Physics\PSPlane.h
/// @brief  a planar collision shape
#pragma once

#include "core-Physics\PhysicsShape.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class PSPlane : public PhysicsShape
{
   DECLARE_ALLOCATOR( PSPlane, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    */
   PSPlane();

   // -------------------------------------------------------------------------
   // PhysicsShape implementation
   // -------------------------------------------------------------------------
   physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const;
};

///////////////////////////////////////////////////////////////////////////////
