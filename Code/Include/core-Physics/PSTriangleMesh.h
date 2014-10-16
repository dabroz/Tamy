/// @file   core-Physics\PSTriangleMesh.h
/// @brief  a collision shape based on a TriangleMesh resource
#pragma once

#include "core-Physics\PhysicsShape.h"


///////////////////////////////////////////////////////////////////////////////

class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

class PSTriangleMesh : public PhysicsShape
{
   DECLARE_ALLOCATOR( PSTriangleMesh, AM_DEFAULT );
   DECLARE_CLASS();

private:
   TriangleMesh*          m_mesh;

public:
   /**
    * Constructor.
    *
    * @param mesh
    */
   PSTriangleMesh( TriangleMesh* mesh = NULL );

   // -------------------------------------------------------------------------
   // PhysicsShape implementation
   // -------------------------------------------------------------------------
   physx::PxShape* instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const;
};

///////////////////////////////////////////////////////////////////////////////
