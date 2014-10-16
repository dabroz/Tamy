/// @file   core-Physics\PhysicsShape.h
/// @brief  physics shape base class
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxRigidActor;
   class PxMaterial;
   class PxShape;
};

///////////////////////////////////////////////////////////////////////////////

class PhysicsShape : public ReflectionObject
{
   DECLARE_ALLOCATOR( PhysicsShape, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    */
   PhysicsShape();
   virtual ~PhysicsShape();

   /**
    * Instantiates the shape on the specified PhysX actor.
    *
    * @param actor
    * @param material
    */
   virtual physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const {
      return NULL; 
   }
};

///////////////////////////////////////////////////////////////////////////////
