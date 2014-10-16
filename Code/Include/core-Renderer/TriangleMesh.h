/// @file   core-Renderer\TriangleMesh.h
/// @brief  geometry of a mesh composed of triangles
#ifndef _TRIANGLE_MESH_H
#define _TRIANGLE_MESH_H


#include "Core\Array.h"
#include "core\Resource.h"
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\Face.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\RenderResource.h"
#include "core\AxisAlignedBox.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class Shader;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is an abstract representation of a mesh geometry composed of
 * triangles.
 * Because such a mesh will preferably be stored in the graphics card's
 * memory for speed reasons, and this can only be achieved by using 
 * some low level 3D lib (DirectX, OpenGL), thus we'll put
 * this responsibility on the TriangleMeshImpl.
 */
class TriangleMesh : public GeometryResource
{
   DECLARE_ALLOCATOR( TriangleMesh, AM_ALIGNED_16 );
   DECLARE_RESOURCE()

public:
   RenderResource*               m_renderResource;

private:
   Array< LitVertex >            m_vertices;
   Array< Face >                 m_faces;
   Array< VertexWeight >         m_vertexWeights;

   bool                          m_boundsDirty;
   AxisAlignedBox                m_boundingVol;

public:
   /**
    * Default constructor.
    */
   TriangleMesh();

   /**
    * Constructor.
    *
    * @param resourceName
    * @param vertices   mesh vertices
    * @param faces      mesh faces
    */
   TriangleMesh( const FilePath& resourceName, const Array<LitVertex>& vertices, const Array<Face>& faces );
   ~TriangleMesh();

   /**
    * Resets the contents of the mesh.
    */
   void clear();

   /**
    * Returns an array of vertices.
    *
    * @return model vertices
    */
   inline const Array< LitVertex >& getVertices() const;

   /**
    * Returns the faces of the model.
    *
    * @return  model faces
    */
   inline const Array<Face>& getFaces() const;

   /**
    * Checks if the mesh has any vertex weights defined.
    */
   inline bool hasVertexWeights() const { return !m_vertexWeights.empty(); }

   /**
    * Returns vertex weights of this mesh.
    */
   inline const Array< VertexWeight >& getVertexWeights() const { return m_vertexWeights; }

   /**
    * Calculates texture-space tangent vectors. 
    */
   void calculateTangents();

   /**
    * Sets new vertices on the mesh.
    *
    * @param arrVertices
    * @param verticesCount
    */
   void setVertices( const LitVertex* arrVertices, uint verticesCount );

   /**
    * Sets new faces on the mesh.
    *
    * @param arrVertices
    * @param verticesCount
    */
   void setFaces( const Face* arrFaces, uint facesCount );

   /**
    * Sets new vertex weights on the mesh.
    *
    * @param arrWeights
    * @param verticesCount
    */
   void setVertexWeights( const VertexWeight* arrWeights, uint verticesCount );

   /**
    * Merges the contents of the specified mesh with this mesh.
    *
    * @param rhs
    * @param transform        a transform that allows us to transform
    *                         the vertices of the merged mesh
    */
   void merge( const TriangleMesh& rhs, const Matrix& transform );

   // -------------------------------------------------------------------------
   // GeometryResource implementation
   // -------------------------------------------------------------------------
   const AxisAlignedBox& getBoundingVolume();
   void render( Renderer& renderer );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );

private:
   /**
    * Recalculates the bounding volume.
    */
   void calculateBoundingVolume();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Triangle mesh rendering command.
 */
class RCRenderTriangleMesh : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRenderTriangleMesh, AM_DEFAULT );

private:
   const TriangleMesh*           m_mesh;

public:
   /**
    * Constructor.
    *
    * @param mesh
    */
   RCRenderTriangleMesh( const TriangleMesh* mesh);
   ~RCRenderTriangleMesh();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\TriangleMesh.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _TRIANGLE_MESH_H
