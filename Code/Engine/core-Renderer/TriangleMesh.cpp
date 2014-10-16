#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\MeshUtils.h"
#include "core.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE_NO_PARENT( TriangleMesh, ttm, AM_BINARY );
   PARENT( GeometryResource );
   PROPERTY( Array<LitVertex>, m_vertices );
   PROPERTY( Array<Face>, m_faces );
   PROPERTY( Array<VertexWeight>, m_vertexWeights );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::TriangleMesh()
   : m_boundsDirty( true )
{
   m_renderResource = RenderResource::instantiate< TriangleMesh >( this );
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::TriangleMesh( const FilePath& resourceName, const Array<LitVertex>& vertices, const Array<Face>& faces )
   : GeometryResource( resourceName )
   , m_vertices(vertices)
   , m_faces(faces)
   , m_boundsDirty( false )
{
   // normalize vertex positions ( set W to 1, otherwise the projection will break the mesh )
   const uint count = m_vertices.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_vertices[i].m_coords[3] = 1.0f;
   }

   m_renderResource = RenderResource::instantiate< TriangleMesh >( this );

   calculateBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::~TriangleMesh()
{
   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::replaceContents( Resource& rhs )
{
   TriangleMesh& rhsTriangleMesh = static_cast< TriangleMesh& >( rhs );
   m_vertices = rhsTriangleMesh.m_vertices;
   m_faces = rhsTriangleMesh.m_faces;
   m_vertexWeights = rhsTriangleMesh.m_vertexWeights;

   m_boundsDirty = true;
   m_renderResource->setDirty();

}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::clear()
{
   m_vertices.clear();
   m_faces.clear();
   m_vertexWeights.clear();

   // mark the render resource as dirty
   m_boundsDirty = true;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

const AxisAlignedBox& TriangleMesh::getBoundingVolume()
{
   if ( m_boundsDirty ) 
   {
      calculateBoundingVolume();
   }

   return m_boundingVol;
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::calculateBoundingVolume()
{
   unsigned int verticesCount = m_vertices.size();
   Vector tmpVec;
   for (unsigned int i = 0; i < verticesCount; ++i)
   {
      tmpVec.load( m_vertices[i].m_coords );
      m_boundingVol.include( tmpVec );
   }

   m_boundsDirty = false;
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::render( Renderer& renderer )
{
   new ( renderer.rtComm() ) RCRenderTriangleMesh( this );
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::calculateTangents()
{
   MeshUtils::calculateVertexTangents( m_faces, m_vertices );
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::setVertices( const LitVertex* arrVertices, uint verticesCount )
{
   m_vertices.clear();
   m_vertices.resize( verticesCount );
   m_boundingVol.reset();

   Vector point;
   for ( uint i = 0; i < verticesCount; ++i )
   {
      m_vertices[i] = arrVertices[i];

      // recalculate the bounding box
      point.load( m_vertices[i].m_coords );
      m_boundingVol.include( point );
   }

   // mark the render resource as dirty
   m_boundsDirty = true;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::setFaces( const Face* arrFaces, uint facesCount )
{
   m_faces.clear();
   m_faces.resize( facesCount );

   for ( uint i = 0; i < facesCount; ++i )
   {
      m_faces[i] = arrFaces[i];
   }

   // mark the render resource as dirty
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::setVertexWeights( const VertexWeight* arrWeights, uint verticesCount )
{
   m_vertexWeights.clear();
   m_vertexWeights.resize( verticesCount );

   for ( uint i = 0; i < verticesCount; ++i )
   {
      m_vertexWeights[i] = arrWeights[i];
   }

   // mark the render resource as dirty
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::merge( const TriangleMesh& rhs, const Matrix& transform )
{
   // copy the mesh data
   const uint oldVerticesCount = m_vertices.size();
   const uint oldFacesCount = m_faces.size();
   const uint oldWeightsCount = m_vertexWeights.size();
  
   m_vertices.copyFrom( rhs.m_vertices );
   m_faces.copyFrom( rhs.m_faces );
   m_vertexWeights.copyFrom( rhs.m_vertexWeights );

   const uint newVerticesCount = m_vertices.size();
   const uint newFacesCount = m_faces.size();
   const uint newWeightsCount = m_vertexWeights.size();

   // transform the copied vertices
   Vector tmpVecIn, tmpVecOut;
   for ( uint i = oldVerticesCount; i < newVerticesCount; ++i )
   {
      LitVertex& vtx = m_vertices[i];

      // position
      tmpVecIn.load( vtx.m_coords );
      transform.transform( tmpVecIn, tmpVecOut );
      tmpVecOut.store( vtx.m_coords );

      // normal
      tmpVecIn.load( vtx.m_normal );
      transform.transformNorm( tmpVecIn, tmpVecOut );
      tmpVecOut.store( vtx.m_normal );

      // tangent
      tmpVecIn.load( vtx.m_tangent );
      transform.transformNorm( tmpVecIn, tmpVecOut );
      tmpVecOut.store( vtx.m_tangent );
   }

   // remap vertex indices of the new faces to the new range
   for ( uint i = oldFacesCount; i < newFacesCount; ++i )
   {
      Face& face = m_faces[i];
      face.idx[0] += oldVerticesCount;
      face.idx[1] += oldVerticesCount;
      face.idx[2] += oldVerticesCount;
   }

   // remap weight indices to the new range
   for ( uint i = oldWeightsCount; i < newWeightsCount; ++i )
   {
      VertexWeight& weightDef = m_vertexWeights[i];
      for ( byte j = 0; j < 4; ++j )
      {
         if ( weightDef.m_indices[j] >= 0 )
         {
            weightDef.m_indices[j] += oldVerticesCount;
         }
      }
   }

   // mark the render resource as dirty
   m_boundsDirty = true;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCRenderTriangleMesh::RCRenderTriangleMesh( const TriangleMesh* mesh )
   : m_mesh( mesh ) 
{ 
}

///////////////////////////////////////////////////////////////////////////////

RCRenderTriangleMesh::~RCRenderTriangleMesh() 
{ 
}

///////////////////////////////////////////////////////////////////////////////
