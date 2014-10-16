#include "ext-RenderingPipeline\ScreenspaceReflectionsRenderer.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\RenderStateCommand.h"


///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > ScreenspaceReflectionsRenderer::m_shader( FilePath( "/Renderer/Shaders/RenderingPipeline/screenspaceReflections.tsh" ) );

///////////////////////////////////////////////////////////////////////////////

ScreenspaceReflectionsRenderer::ScreenspaceReflectionsRenderer()
{
   m_textureSampler.setUnified( TAM_CLAMP, TFM_POINT );

   m_constants = ShaderDataBuffer::create< ScreenspaceReflectionParams >();
}

///////////////////////////////////////////////////////////////////////////////

ScreenspaceReflectionsRenderer::~ScreenspaceReflectionsRenderer()
{
   if ( m_constants )
   {
      m_constants->removeReference();
      m_constants = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ScreenspaceReflectionsRenderer::render( Renderer& renderer, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_shader.get();
   if ( !shader )
   {
      return;
   }

   // set the constants
   ScreenspaceReflectionParams& constants = m_constants->accessData< ScreenspaceReflectionParams >();
   {
      Camera& camera = renderer.getActiveCamera();

      const Matrix& camProjMtx = camera.getProjectionMtx();
      constants.m_camProjMtx = camProjMtx;
      constants.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );
   }

   // render
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setTexture( "g_Albedo", gBuffer, m_textureSampler, GBuf_Albedo );
   bindPS->setTexture( "g_Normals", gBuffer, m_textureSampler, GBuf_Normals );
   bindPS->setDepthBuffer( "g_Depth", gBuffer );
   bindPS->setDataBuf( "Constants", m_constants );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////
