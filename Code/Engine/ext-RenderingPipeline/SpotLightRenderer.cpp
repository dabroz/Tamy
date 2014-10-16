#include "ext-RenderingPipeline\SpotLightRenderer.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderTree.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\Viewport.h"
#include "core-Renderer\ProceduralGeometryRenderer.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"

// collectors
#include "ext-RenderingPipeline\GeometryStorage.h"
#include "ext-RenderingPipeline\RPDataProxies.h"

// lights
#include "core-Renderer\Light.h"
#include "core-Renderer\SpotLight.h"

// math
#include "core\Math.h"

// lookup texture
#include "core-Renderer\ProceduralTexture.h"
#include "ext-RenderingPipeline\LookupTextures.h"


///////////////////////////////////////////////////////////////////////////////

#define SPOT_LIGHT_MESH_FACES_COUNT    12
word g_spotLightMeshIndices [] = {
   0, 1, 2,
   1, 3, 2,

   4, 6, 5,
   6, 7, 5,

   0, 4, 5,
   0, 5, 1,

   1, 5, 7,
   1, 7, 3,

   3, 7, 6,
   3, 6, 2,

   2, 6, 4,
   2, 4, 0
};

///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > SpotLightRenderer::m_spotLightWithShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/spotLightWithShadow.tsh" ) );
TResourceHandle< Shader > SpotLightRenderer::m_spotLightWithoutShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/spotLightNoShadow.tsh" ) );

///////////////////////////////////////////////////////////////////////////////

SpotLightRenderer::SpotLightRenderer( GeometryStorage* geometryStorage )
   : m_geometryStorage( geometryStorage )
   , m_shadowmapSize( 1024 )
{
   m_textureSampler.setUnified( TAM_CLAMP, TFM_POINT );

   // initialize the roughness lookup texture
   LookupTextures& lookupTextures = TSingleton< LookupTextures >::getInstance();
   m_roughnessLookup = lookupTextures.getCookTorrenceRoughness();

   m_shadowMap = new RenderTarget2DStatic( m_shadowmapSize, m_shadowmapSize, TU_SHADOW_MAP, false );

   // setup basic constants
   m_vertexShaderConstants = ShaderDataBuffer::create< Matrix >();
   m_fragmentShaderConstants = ShaderDataBuffer::create< SpotLightProperties >();
}

///////////////////////////////////////////////////////////////////////////////

SpotLightRenderer::~SpotLightRenderer()
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

void SpotLightRenderer::render( Renderer& renderer, const SpotLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   if ( light->castsShadows() )
   {
      renderShadowMap( renderer, light );

      const Shader* shader = m_spotLightWithShadowShader.get();
      renderLight( renderer, light, gBuffer, shader, outRenderTarget );
   }
   else
   {
      const Shader* shader = m_spotLightWithoutShadowShader.get();
      renderLight( renderer, light, gBuffer, shader, outRenderTarget );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SpotLightRenderer::renderShadowMap( Renderer& renderer, const SpotLight* light )
{
   // create the light camera
   Camera lightCamera( "", Camera::PT_PERSPECTIVE );
   {
      lightCamera.setFOV( light->m_spotAngle * 2.0f );
      lightCamera.setClippingPlanes( 1.01f, light->m_range );
      lightCamera.setNearPlaneDimensions( m_shadowmapSize, m_shadowmapSize );
      lightCamera.setLocalMtx( light->getGlobalMtx() );
      lightCamera.updateTransforms();
   }

   // calculate the reverse transformation matrix used by the lighting shader
   {
      Matrix invCameraViewMtx;
      invCameraViewMtx.setInverse( renderer.getActiveCamera().getViewMtx() );

      const Matrix& lightProjMtx = lightCamera.getProjectionMtx();

      Matrix lightViewProjMtx;
      lightViewProjMtx.setMul( lightCamera.getViewMtx(), lightProjMtx );

      SpotLightProperties& lightProperties = m_fragmentShaderConstants->accessData< SpotLightProperties >();
      lightProperties.m_camViewToLightProjMtx.setMul( invCameraViewMtx, lightViewProjMtx );
      lightProperties.m_lightProjParams.set( lightProjMtx( 0, 0 ), lightProjMtx( 1, 1 ), lightProjMtx( 2, 2 ), lightProjMtx( 3, 2 ) );
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
      SAVE_RENDER_STATE( renderer );
      MemoryAllocator* rtComm = renderer.rtComm();

      new ( rtComm ) RCClearRenderTarget( m_shadowMap );
      new ( rtComm ) RCActivateRenderTarget( m_shadowMap );

      CHANGE_RENDER_STATE( renderer, RSSetBlending( false ) );

      renderer.pushCamera( lightCamera );
      RenderTree* renderTree = renderer.buildRenderTree( m_litGeometry );
      if ( renderTree )
      {
         // activate the render targets we want to render to and clean it
         renderTree->render( RP_ShadowMap );

         // get rid of the tree
         delete renderTree;
      }
      renderer.popCamera();
      new ( rtComm ) RCDeactivateRenderTarget();
      RESTORE_RENDER_STATE( renderer );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SpotLightRenderer::renderLight( Renderer& renderer, const SpotLight* light, RenderTarget2D* gBuffer, const Shader* shader, RenderTarget2D* outRenderTarget )
{
   if ( !shader )
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

      SpotLightProperties& lightProperties = m_fragmentShaderConstants->accessData< SpotLightProperties >();

      // projection properties for calculating inverse projection
      const Matrix& camProjMtx = activeCamera.getProjectionMtx();
      lightProperties.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );

      // transform the position to view space  
      const Vector& lightPos = lightWorldSpace.position();
      viewMtx.transform( lightPos, lightProperties.m_position );

      // transform the direction to view space  
      viewMtx.transformNorm( lightWorldSpace.forwardVec(), lightProperties.m_direction );

      lightProperties.m_lightColor.setMul( light->m_color, light->m_strength );
      lightProperties.m_range = light->m_range;
      lightProperties.m_falloff = light->m_falloff;
      lightProperties.m_spotAngle = DEG2RAD( light->m_spotAngle );

      // calculate the transformation matrix for the light mesh
      Matrix& lightTransformMtx = m_vertexShaderConstants->accessData< Matrix >();
      {
         Matrix viewProj;
         viewProj.setMul( activeCamera.getViewMtx(), camProjMtx );
         lightTransformMtx.setMul( lightWorldSpace, viewProj );
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
   bindPS->setDepthBuffer( "g_ShadowMap", m_shadowMap );
   bindPS->setDataBuf( "VertexShaderConstants", m_vertexShaderConstants );
   bindPS->setDataBuf( "LightProperties", m_fragmentShaderConstants );

   // prepare the mesh
   RCRenderProceduralMesh* procMesh = new ( rcComm ) RCRenderProceduralMesh( renderer, 8, SPOT_LIGHT_MESH_FACES_COUNT );
   {
      const float halfBaseEdgeLen = light->m_range * tan( DEG2RAD( light->m_spotAngle * 0.5f ) );
      const float halfBaseDiag = sqrt( 2.0f * halfBaseEdgeLen * halfBaseEdgeLen );

      procMesh->m_vertices[0].set( -halfBaseDiag, -halfBaseDiag, 0.0f, 1.0f );
      procMesh->m_vertices[1].set( halfBaseDiag, -halfBaseDiag, 0.0f, 1.0f );
      procMesh->m_vertices[2].set( -halfBaseDiag, halfBaseDiag, 0.0f, 1.0f );
      procMesh->m_vertices[3].set( halfBaseDiag, halfBaseDiag, 0.0f, 1.0f );
      procMesh->m_vertices[4].set( -halfBaseDiag, -halfBaseDiag, light->m_range, 1.0f );
      procMesh->m_vertices[5].set( halfBaseDiag, -halfBaseDiag, light->m_range, 1.0f );
      procMesh->m_vertices[6].set( -halfBaseDiag, halfBaseDiag, light->m_range, 1.0f );
      procMesh->m_vertices[7].set( halfBaseDiag, halfBaseDiag, light->m_range, 1.0f );

      memcpy( (word*)procMesh->m_indices, g_spotLightMeshIndices, sizeof( word ) * 3 * SPOT_LIGHT_MESH_FACES_COUNT );
   }

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////
