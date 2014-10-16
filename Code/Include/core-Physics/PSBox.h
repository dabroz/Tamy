/// @file   core-Physics\PSBox.h
/// @brief  a box collision shape
#pragma once

#include "core-Physics\PhysicsShape.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class PSBox : public PhysicsShape
{
   DECLARE_ALLOCATOR( PSBox, AM_DEFAULT );
   DECLARE_CLASS();

private:
   Vector                m_halfExtents;

public:
   /**
    * Constructor.
    *
    * @param halfExtents
    */
   PSBox( const Vector& halfExtents = Vector( 0.5f, 0.5f, 0.5f ) );

   // -------------------------------------------------------------------------
   // PhysicsShape implementation
   // -------------------------------------------------------------------------
   physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const;
};

///////////////////////////////////////////////////////////////////////////////
