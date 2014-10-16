#include "core-Renderer\ProceduralGeometryRenderer.h"
#include "core-Renderer\Renderer.h"
#include "core\TSContinuousMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

RCRenderProceduralMesh::RCRenderProceduralMesh( Renderer& renderer, uint verticesCount, uint facesCount )
   : m_vertices( verticesCount, renderer.rtMemPool() )
   , m_indices( facesCount * 3, renderer.rtMemPool() )
{
   m_vertices.resizeWithoutInitializing( verticesCount );
   m_indices.resizeWithoutInitializing( facesCount * 3 );
}

///////////////////////////////////////////////////////////////////////////////
