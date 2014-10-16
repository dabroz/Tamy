#include "ext-RenderingPipeline\PointLightRenderer.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\RenderTargetCube.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderTree.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\Viewport.h"
#include "core-Renderer\CubeTexture.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"

// collectors
#include "ext-RenderingPipeline\GeometryStorage.h"
#include "ext-RenderingPipeline\RPDataProxies.h"

// lights
#include "core-Renderer\Light.h"
#include "core-Renderer\PointLight.h"

// math
#include "core\Math.h"

// lookup texture
#include "core-Renderer\ProceduralTexture.h"
#include "ext-RenderingPipeline\LookupTextures.h"


///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > PointLightRenderer::m_pointLightWithShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/pointLightWithShadow.tsh" ) );
TResourceHandle< Shader > PointLightRenderer::m_pointLightWithoutShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/pointLightNoShadow.tsh" ) );
TResourceHandle< TriangleMesh > PointLightRenderer::m_pointLightMesh( FilePath( "/Renderer/Meshes/pointLightMesh_0.ttm" ) );

///////////////////////////////////////////////////////////////////////////////

PointLightRenderer::PointLightRenderer( GeometryStorage* geometryStorage )
   : m_geometryStorage( geometryStorage )
   , m_shadowmapSize( 1024 )
{
   m_textureSampler.setUnified( TAM_CLAMP, TFM_POINT );

   // initialize the roughness lookup texture
   LookupTextures& lookupTextures = TSingleton< LookupTextures >::getInstance();
   m_roughnessLookup = lookupTextures.getCookTorrenceRoughness();

   m_shadowMap = new RenderTargetCube( m_shadowmapSize, TU_SHADOW_MAP );

   // setup basic constants
   m_vertexShaderConstants = ShaderDataBuffer::create< Matrix >();
   m_fragmentShaderConstants = ShaderDataBuffer::create< PointLightProperties >();

   // setup camera rotations
   {
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector( -1.0f,  0.0f,  0.0f ), Vector(  0.0f,  1.0f,  0.0f ), m_cameraRotation[CubeTexture::Cube_Left] );    // -X
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector(  1.0f,  0.0f,  0.0f ), Vector(  0.0f,  1.0f,  0.0f ), m_cameraRotation[CubeTexture::Cube_Right] );   // +X
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector(  0.0f,  0.0f,  1.0f ), Vector(  0.0f, -1.0f,  0.0f ), m_cameraRotation[CubeTexture::Cube_Rear] );    // +Z
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector(  0.0f,  0.0f, -1.0f ), Vector(  0.0f,  1.0f,  0.0f ), m_cameraRotation[CubeTexture::Cube_Front] );   // -Z
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector(  0.0f,  1.0f,  0.0f ), Vector(  0.0f,  0.0f,  1.0f ), m_cameraRotation[CubeTexture::Cube_Bottom] );  // +Y
      MatrixUtils::generateLookAtLH( Vector_ZERO, Vector(  0.0f, -1.0f,  0.0f ), Vector(  0.0f,  0.0f, -1.0f ), m_cameraRotation[CubeTexture::Cube_Top] );     // -Y
      
   }
}

///////////////////////////////////////////////////////////////////////////////

PointLightRenderer::~PointLightRenderer()
{
   if ( m_vertexShaderConstants )
   {
      m_vertexShaderConstants->removeReference();
      m_vertexShaderConstants = NULL;
   }

   if ( m_fragmentShaderConstants )
   {
      m_fragmentShaderConstants->removeReference();
      m_fragmentShaderConstants = NULL;
   }

   if ( m_roughnessLookup )
   {
      m_roughnessLookup->removeReference();
      m_roughnessLookup = NULL;
   }

   m_shadowMap->removeReference();
   m_shadowMap = NULL;

   m_geometryStorage = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void PointLightRenderer::render( Renderer& renderer, const PointLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   if ( light->castsShadows() )
   {
      renderShadowMap( renderer, light );

      const Shader* shader = m_pointLightWithShadowShader.get();
      renderLight( renderer, shader, light, gBuffer, outRenderTarget );
   }
   else
   {
      const Shader* shader = m_pointLightWithoutShadowShader.get();
      renderLight( renderer, shader, light, gBuffer, outRenderTarget );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PointLightRenderer::renderShadowMap( Renderer& renderer, const PointLight* light )
{
   // clear the shadow map
   MemoryAllocator* rtComm = renderer.rtComm();
   new ( rtComm ) RCClearRenderTargetCube( m_shadowMap );

   // create the light camera
   Camera lightCamera( "", Camera::PT_PERSPECTIVE );
   {
      lightCamera.setFOV( 90.0f ); // we're using a 90 degree FOV to cover the entire side of the cube
      lightCamera.setClippingPlanes( 0.1f, light->m_radius );
      lightCamera.setNearPlaneDimensions( m_shadowmapSize, m_shadowmapSize );
   }

   // set the render state
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetBlending( false ) );


   // activate the light camera - we'll use it to render the shadow maps
   renderer.pushCamera( lightCamera );

   // render the shadow map from 6 different angles
   PointLightProperties& lightProperties = m_fragmentShaderConstants->accessData< PointLightProperties >();
   const Matrix& lightMtx = light->getGlobalMtx();
   for ( byte cubeSideIdx = 0; cubeSideIdx < 6; ++cubeSideIdx )
   {
      // rotate the camera to the specified side
      {
         Matrix& sideLightMtx = lightCamera.accessLocalMtx();
         sideLightMtx = m_cameraRotation[cubeSideIdx];
         sideLightMtx.setPosition<3>( lightMtx.position() );
         lightCamera.updateTransforms();
      }

      // query the geometry to render
      {
         RPVolFrustum geometryQueryVolume;
         lightCamera.calculateFrustum( geometryQueryVolume.m_volume );
         m_litGeometry.clear();
         m_geometryStorage->queryForShadowMapping( geometryQueryVolume, m_litGeometry );
      }

      // render the shadow map
      if ( m_litGeometry.size() > 0 )
      {
         new ( rtComm ) RCActivateRenderTargetCube( m_shadowMap, cubeSideIdx );
         
         RenderTree* renderTree = renderer.buildRenderTree( m_litGeometry );
         if ( renderTree )
         {
            // activate the render targets we want to render to and clean it
            renderTree->render( RP_ShadowMap );

            // get rid of the tree
            delete renderTree;
         }
      }
   }

   new ( rtComm ) RCDeactivateRenderTargetCube();
   renderer.popCamera();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void PointLightRenderer::renderLight( Renderer& renderer, const Shader* shader, const PointLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   TriangleMesh* mesh = m_pointLightMesh.get();
   if ( !shader || !mesh )
   {
      return;
   }

   // set the constants
   {
      Camera& activeCamera = renderer.getActiveCamera();
      const Matrix& viewMtx = activeCamera.getViewMtx();

      Matrix lightWorldSpace;
      if ( light->isAttached() )
      {
         lightWorldSpace = light->getGlobalMtx();
      }
      else
      {
         lightWorldSpace.setIdentity();
      }

      PointLightProperties& lightProperties = m_fragmentShaderConstants->accessData< PointLightProperties >();

      // projection properties for calculating inverse projection
      const Matrix& camProjMtx = activeCamera.getProjectionMtx();
      lightProperties.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );
      lightProperties.m_invCamViewMtx.setInverse( activeCamera.getViewMtx() );

      // transform the position to view space  
      const Vector& lightPos = lightWorldSpace.position();
      viewMtx.transform( lightPos, lightProperties.m_position );

      lightProperties.m_lightColor.setMul( light->m_color, light->m_strength );
      lightProperties.m_radius = light->m_radius;
      lightProperties.m_falloff = light->m_falloff;

      // calculate the transformation matrix for the light mesh
      {
         Matrix scaleMtx, scaleWorldMtx, viewProj;
         scaleMtx.scaleUniform( FastFloat::fromFloat( light->m_radius ) );
         scaleWorldMtx.setMul( scaleMtx, lightWorldSpace );
         viewProj.setMul( activeCamera.getViewMtx(), camProjMtx );
         Matrix& lightTransformMtx = m_vertexShaderConstants->accessData< Matrix >();
         lightTransformMtx.setMul( scaleWorldMtx, viewProj );
      }
   }


   // render the light
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetBlending( true, BLEND_ONE, BLEND_ONE ) );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setDepthBuffer( "g_DepthBuffer", gBuffer );
   bindPS->setTexture( "g_Diffuse", gBuffer, m_textureSampler, GBuf_Albedo );
   bindPS->setTexture( "g_Specular", gBuffer, m_textureSampler, GBuf_Specular );
   bindPS->setTexture( "g_BRDF", gBuffer, m_textureSampler, GBuf_BRDF );
   bindPS->setTexture( "g_Normals", gBuffer, m_textureSampler, GBuf_Normals );
   bindPS->setTexture( "g_RoughnessLookup", m_roughnessLookup, m_textureSampler );
   bindPS->setTexture( "g_ShadowMap", m_shadowMap, m_textureSampler );
   bindPS->setDataBuf( "VertexShaderConstants", m_vertexShaderConstants );
   bindPS->setDataBuf( "LightProperties", m_fragmentShaderConstants );

   mesh->render( renderer );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////
