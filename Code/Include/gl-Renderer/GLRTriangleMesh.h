/// @file   gl-Renderer\GLRTriangleMesh.h
/// @brief  OpenGL representation of a TriangleMesh
#pragma once

#include "core-Renderer\TriangleMesh.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class GLRenderer;
class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

class GLRTriangleMesh
{
   DECLARE_ALLOCATOR( GLRTriangleMesh, AM_DEFAULT );

private:
   GLRenderer&                   m_renderer;
   const TriangleMesh*           m_mesh;

   uint                          m_vaoID;
   uint                          m_vboVerticesID;
   uint                          m_vboIndicesID;
   uint                          m_vboVertexWeights;

   uint                          m_verticesCount;
   uint                          m_facesCount;

public:
   /**
    * Constructor.
    *
    * @param renderer
    * @param mesh
    */
   GLRTriangleMesh( GLRenderer& renderer, const TriangleMesh* mesh );
   ~GLRTriangleMesh();

   /**
    * Renders the mesh.
    */
   void render();

   /**
    * Refreshes the resource to reflect the changes in the corresponding engine resource.
    */
   void refresh();

   // -------------------------------------------------------------------------
   // IRenderResourceStorage interface
   // -------------------------------------------------------------------------
   const TriangleMesh* getEngineResource() {
      return m_mesh;
   }

private:
   void initialize();
   void deinitialize();
};

///////////////////////////////////////////////////////////////////////////////
