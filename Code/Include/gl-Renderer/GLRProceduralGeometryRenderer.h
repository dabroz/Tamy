/// @file   gl-Renderer\GLRProceduralGeometryRenderer.h
/// @brief  classes and commands used to render procedural geometry
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class GLRMutableMesh
{
   DECLARE_ALLOCATOR( GLRMutableMesh, AM_DEFAULT );

private:
   const uint                    m_maxVertices;
   const uint                    m_maxIndices;

   uint                          m_vaoID;
   uint                          m_vboVerticesID;
   uint                          m_vboIndicesID;

public:
   /**
    * Constructor.
    */
   GLRMutableMesh();
   ~GLRMutableMesh();

   /**
    * Renders the specified geometry as a triangle mesh.
    *
    * @param vertices
    * @param indices
    */
   void render( const Array< Vector >& vertices, const Array< word >& indices ) const;
};

///////////////////////////////////////////////////////////////////////////////
