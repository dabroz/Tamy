#include "ext-RenderingPipeline\SkyboxRenderer.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\RenderableTexture.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\RenderableCubeTexture.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > SkyboxRenderer::m_skyboxShader( FilePath( "/Renderer/Shaders/RenderingPipeline/skybox.tsh" ) );
TResourceHandle< TriangleMesh > SkyboxRenderer::m_skyboxMesh( FilePath( "/Renderer/Meshes/skyBoxMesh_0.ttm" ) );

///////////////////////////////////////////////////////////////////////////////

SkyboxRenderer::SkyboxRenderer()
   : m_renderTexture( NULL )
{
   m_samplerSettings.setUnified( TAM_CLAMP, TFM_LINEAR );

   m_constantsBuf = new ShaderDataBuffer( sizeof( Matrix ) );

   // pre-compute the rotation of the skybox
   m_skyboxRot.setAxisAngle( Vector_OX, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
}

///////////////////////////////////////////////////////////////////////////////

SkyboxRenderer::~SkyboxRenderer()
{
   m_renderTexture = NULL;

   m_constantsBuf->removeReference();
   m_constantsBuf = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkyboxRenderer::setTexture( RenderableCubeTexture* texture )
{
   m_renderTexture = texture;
}

///////////////////////////////////////////////////////////////////////////////

void SkyboxRenderer::render( class Renderer& renderer, class RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_skyboxShader.get();
   TriangleMesh* mesh = m_skyboxMesh.get();
   if ( !shader || !mesh )
   {
      return;
   }

   const Camera& camera = renderer.getActiveCamera();
   Vector cameraPos;
   camera.getPosition( cameraPos );

   Matrix skyboxTransform;
   skyboxTransform.setRotation( m_skyboxRot );
   skyboxTransform.setPosition<3>( cameraPos );

   Matrix modelToView, modelToProj;
   modelToView.setMul( skyboxTransform, camera.getViewMtx() );
   modelToProj.setMul( modelToView, camera.getProjectionMtx() );

   m_constantsBuf->accessData< Matrix >() = modelToProj;

   // render
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, true ) );

   MemoryAllocator* rcComm = renderer.rtComm();
   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setDataBuf( "Constants", m_constantsBuf );
   bindPS->setTexture( "g_Tex1", m_renderTexture, m_samplerSettings );

   mesh->render( renderer );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );

}

///////////////////////////////////////////////////////////////////////////////
