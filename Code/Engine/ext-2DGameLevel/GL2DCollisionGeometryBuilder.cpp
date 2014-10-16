#include "ext-2DGameLevel\GL2DCollisionGeometryBuilder.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\Face.h"
#include "core\PolygonMesh2D.h"
#include "core\MeshAdjacencyMap.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"
#include "ext-2DGameLevel\GL2DCollisionGeometry.h"


///////////////////////////////////////////////////////////////////////////////

GL2DCollisionGeometryBuilder::GL2DCollisionGeometryBuilder( Entity* rootEntity )
   : m_rootEntity( rootEntity )
   , m_createdBody( NULL )
   , m_createdShape( NULL )
{
   m_createdBody = EntityUtils::getFirstChild< GL2DBody >( m_rootEntity );
   if ( m_createdBody )
   {
      m_createdShape = m_createdBody->getShape();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionGeometryBuilder::initialize( const List< SceneNode* >& selectedNodes )
{
   if ( !m_createdShape || selectedNodes.empty() )
   {
      return;
   }

   for ( List< SceneNode* >::const_iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;

      Entity* entity = DynamicCast< Entity >( node );
      if ( entity )
      {
         parseEntity( entity );
      }
      else
      {
         parseGeometry( node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionGeometryBuilder::parseEntity( Entity* entity )
{
   List< SceneNode* > nodesQueue;
   nodesQueue.pushBack( entity );

   while ( !nodesQueue.empty() )
   {
      SceneNode* node = nodesQueue.front();
      nodesQueue.popFront();

      parseGeometry( node );

      Entity* entity = DynamicCast< Entity >( node );
      if ( entity )
      {
         const uint count = entity->m_children.size();
         for ( uint i = 0; i < count; ++i )
         {
            SceneNode* child = entity->m_children[i];
            nodesQueue.pushBack( child );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionGeometryBuilder::parseGeometry( SceneNode* node )
{
   GL2DCollisionGeometry* geometryNode = DynamicCast< GL2DCollisionGeometry >( node );
   if ( !geometryNode )
   {
      return;
   }

   const Matrix& worldMtx = geometryNode->getGlobalMtx();

   const Array< Vector >& vertices = geometryNode->getVertices();
   const Array< GL2DCollisionTriangle >& faces = geometryNode->getFaces();

   // upload & transform the vertex
   const uint firstVtxIdx = m_vertices.size();
   const uint verticesCount = vertices.size();
   m_vertices.resize( firstVtxIdx + verticesCount );
   for ( uint i = 0; i < verticesCount; ++i )
   {
      const Vector& srcVtx = vertices[i];
      Vector& destVtx = m_vertices[firstVtxIdx + i];

      worldMtx.transform( srcVtx, destVtx );
   }

   const uint prevIndicesCount = m_indices.size();
   const uint facesCount = faces.size();
   m_indices.resize( prevIndicesCount + facesCount * 3 );
   uint indexIdx;
   for ( uint i = 0; i < facesCount; ++i )
   {
      const GL2DCollisionTriangle& face = faces[i];
      indexIdx = prevIndicesCount + i * 3;

      // when copying the indices, remap them to the area where the vertices are stored
      m_indices[indexIdx] = firstVtxIdx + face.idx[0];
      m_indices[indexIdx + 1] = firstVtxIdx + face.idx[1];
      m_indices[indexIdx + 2] = firstVtxIdx + face.idx[2];
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionGeometryBuilder::process()
{
   if ( m_createdShape )
   {
      m_createdShape->setFromPolygonSoup( ( Vector* ) m_vertices, m_vertices.size(), ( word* ) m_indices, m_indices.size() / 3 );
   }
}

///////////////////////////////////////////////////////////////////////////////
