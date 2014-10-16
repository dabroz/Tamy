/// @file   core-Renderer\ProceduralGeometryRenderer.h
/// @brief  render command that allows to render procedurally generated geometry
#pragma once

#include "core-Renderer\RenderCommand.h"
#include "core\Array.h"
#include "core\types.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class RCRenderProceduralMesh : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRenderProceduralMesh, AM_DEFAULT );

public:
   Array< Vector >         m_vertices;
   Array< word >           m_indices;

public:
   /**
    * Constructor.
    *
    * @param renderer
    * @param verticesCount
    * @param facesCount
    */
   RCRenderProceduralMesh( Renderer& renderer, uint verticesCount, uint facesCount );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& owner ) override;
};

///////////////////////////////////////////////////////////////////////////////
