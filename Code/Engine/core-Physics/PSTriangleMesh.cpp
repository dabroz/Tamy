#include "core-Physics\PSTriangleMesh.h"

// physx
#include "PxRigidActor.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "PxPhysics.h"
#include "geometry\PxTriangleMeshGeometry.h"
#include "geometry\PxTriangleMesh.h"
#include "extensions\PxDefaultStreams.h"

// physx - cooking
#include "cooking\PxTriangleMeshDesc.h"
#include "cooking\PxCooking.h"

// physics
#include "core-Physics\PhysicsSystem.h"

// mesh
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\Face.h"

// utils
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PSTriangleMesh );
   PARENT( PhysicsShape );
   PROPERTY_EDIT( "Mesh", TriangleMesh*, m_mesh );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PSTriangleMesh::PSTriangleMesh( TriangleMesh* mesh )
   : m_mesh( mesh )
{
}

///////////////////////////////////////////////////////////////////////////////

physx::PxShape* PSTriangleMesh::instantiate( physx::PxRigidActor& actor, physx::PxMaterial& material ) const
{
   if ( !m_mesh )
   {
      return NULL;
   }

   PhysicsSystem& physicsSys = TSingleton< PhysicsSystem >::getInstance();

   // prepare temporary arrays of faces and vertices
   const Array< LitVertex >& vertices = m_mesh->getVertices();
   const Array< Face >& faces = m_mesh->getFaces();

   const uint verticesCount = vertices.size();
   Array< physx::PxVec3 > arrVertices( verticesCount );
   arrVertices.resizeWithoutInitializing( verticesCount );
   for ( uint i = 0; i < verticesCount; ++i )
   {
      const LitVertex& vtx = vertices[i];
      const TVector<4>& coords = vtx.m_coords;
      physx::PxVec3& pxVtx = arrVertices[i];

      pxVtx.x = coords[0];
      pxVtx.y = coords[1];
      pxVtx.z = coords[2];
   }

   const uint facesCount = faces.size();
   Array< physx::PxU32 > arrTriangles( facesCount * 3 );
   arrTriangles.resizeWithoutInitializing( facesCount * 3 );
   uint coordIdx = 0;
   for ( uint i = 0; i < facesCount; ++i )
   {
      const Face& face = faces[i];
      arrTriangles[coordIdx++] = face.idx[0];
      arrTriangles[coordIdx++] = face.idx[1];
      arrTriangles[coordIdx++] = face.idx[2];
   }

   // cook the mesh
   physx::PxTriangleMeshDesc meshDesc;
   meshDesc.points.count = verticesCount;
   meshDesc.points.stride = sizeof( physx::PxVec3 );
   meshDesc.points.data = ( physx::PxVec3* )arrVertices;

   meshDesc.triangles.count = facesCount;
   meshDesc.triangles.stride = sizeof( physx::PxU32 ) * 3;
   meshDesc.triangles.data = ( physx::PxU32* )arrTriangles;

   physx::PxDefaultMemoryOutputStream writeBuffer;
   physx::PxCooking* cookingInterface = physicsSys.getCookingInterface();
   bool status = cookingInterface->cookTriangleMesh( meshDesc, writeBuffer );

   if ( !status )
   {
      // cooking failed
      LOG( "Triangle mesh cooking failed" );
      return NULL;
   }

   physx::PxDefaultMemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );

   physx::PxTriangleMeshGeometry triGeom;
   physx::PxPhysics* physicsInterface = physicsSys.getPhysXInterface();
   triGeom.triangleMesh = physicsInterface->createTriangleMesh( readBuffer );

   // create a shape
   physx::PxShape* shape = actor.createShape( triGeom, material );
   return shape;
}

///////////////////////////////////////////////////////////////////////////////
