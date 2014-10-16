#include "ext-RenderingPipeline\TexturesBlender.h"

// renderer
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\RenderTree.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\RendererQuery.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\TriangleMesh.h"

// objects
#include "core-Renderer\Light.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\PointLight.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\GeometryComponent.h"

// math
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > TexturesBlender::m_alphaShader( FilePath( "/Renderer/Shaders/RenderingPipeline/blendTexturesByAlpha.tsh" ) );
TResourceHandle< Shader > TexturesBlender::m_colorShader( FilePath( "/Renderer/Shaders/RenderingPipeline/blendTexturesByColor.tsh" ) );
TResourceHandle< Shader > TexturesBlender::m_blurShader( FilePath( "/Renderer/Shaders/RenderingPipeline/blur.tsh" ) );

///////////////////////////////////////////////////////////////////////////////

TexturesBlender::TexturesBlender()
{
   m_constantsBuf = new ShaderDataBuffer( sizeof( Vector ) );

   m_samplerSettings.setUnified( TAM_CLAMP, TFM_POINT );
}

///////////////////////////////////////////////////////////////////////////////

TexturesBlender::~TexturesBlender()
{
   m_constantsBuf->removeReference();
   m_constantsBuf = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TexturesBlender::blendByColor( Renderer& renderer, ShaderTexture* tex1, ShaderTexture* tex2, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_colorShader.get();
   if ( !shader )
   {
      return;
   }

   // render
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   Vector halfPixel;
   ShaderUtils::calculateHalfPixel( renderer, outRenderTarget, halfPixel );
   m_constantsBuf->accessData< Vector >() = halfPixel;


   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setTexture( "g_Tex1", tex2, m_samplerSettings );
   bindPS->setTexture( "g_Tex2", tex1, m_samplerSettings );
   bindPS->setDataBuf( "Constants", m_constantsBuf );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void TexturesBlender::blendByAlpha( Renderer& renderer, ShaderTexture* tex1, ShaderTexture* tex2, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_alphaShader.get();
   if ( !shader )
   {
      return;
   }

   // render
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   Vector halfPixel;
   ShaderUtils::calculateHalfPixel( renderer, outRenderTarget, halfPixel );
   m_constantsBuf->accessData< Vector >() = halfPixel;


   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setTexture( "g_Tex1", tex2, m_samplerSettings );
   bindPS->setTexture( "g_Tex2", tex1, m_samplerSettings );
   bindPS->setDataBuf( "Constants", m_constantsBuf );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void TexturesBlender::blur( class Renderer& renderer, ShaderTexture* inTex, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_blurShader.get();
   if ( !shader )
   {
      return;
   }

   // render
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setTexture( "g_Tex", inTex, m_samplerSettings );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////
