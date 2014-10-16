#include "ext-RenderingPipeline\SceneRenderer.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderTree.h"

// math
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

SceneRenderer::SceneRenderer( Array< GeometryComponent* >* geometry )
   : m_geometry( geometry )
{
}

///////////////////////////////////////////////////////////////////////////////

SceneRenderer::~SceneRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////

void SceneRenderer::renderColor( Renderer& renderer, RenderTarget2D* outTarget )
{
   render( renderer, RP_Default, outTarget );
}

///////////////////////////////////////////////////////////////////////////////

void SceneRenderer::renderColorOverride( Renderer& renderer, const struct Color& colorOverride, RenderTarget2D* outTarget )
{
   RenderingContext& context = renderer.accessContext();
   context.m_color = colorOverride;

   render( renderer, RP_Selection, outTarget );
}

///////////////////////////////////////////////////////////////////////////////

void SceneRenderer::renderObjectPointers( Renderer& renderer, RenderTarget2D* outTarget )
{
   render( renderer, RP_SelectionQuery, outTarget );
}

///////////////////////////////////////////////////////////////////////////////

void SceneRenderer::render( Renderer& renderer, RenderProfileId profile, RenderTarget2D* outTarget )
{
   // build a tree sorting the nodes by the attributes
   RenderTree* renderTree = renderer.buildRenderTree( *m_geometry );
   if ( renderTree )
   {
      MemoryAllocator* rtComm = renderer.rtComm();

      // activate the render targets we want to render to and clean it
      new ( rtComm ) RCActivateRenderTarget( outTarget );
      renderTree->render( profile );

      // get rid of the tree
      delete renderTree;

      // unbind render targets
      new ( rtComm ) RCDeactivateRenderTarget();
   }
}

///////////////////////////////////////////////////////////////////////////////
