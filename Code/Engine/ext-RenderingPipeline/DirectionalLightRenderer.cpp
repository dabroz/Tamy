#include "ext-RenderingPipeline\DirectionalLightRenderer.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderTree.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\Viewport.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"

// collectors
#include "ext-RenderingPipeline\GeometryStorage.h"
#include "ext-RenderingPipeline\RPDataProxies.h"

// lights
#include "core-Renderer\Light.h"
#include "core-Renderer\DirectionalLight.h"

// math
#include "core\Math.h"

// lookup texture
#include "core-Renderer\ProceduralTexture.h"
#include "ext-RenderingPipeline\LookupTextures.h"


///////////////////////////////////////////////////////////////////////////////

TResourceHandle< Shader > DirectionalLightRenderer::m_directionalLightWithShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/directionalLight.tsh" ) );
TResourceHandle< Shader > DirectionalLightRenderer::m_directionalLightWithoutShadowShader( FilePath( "/Renderer/Shaders/RenderingPipeline/directionalLightNoShadow.tsh" ) );

///////////////////////////////////////////////////////////////////////////////

DirectionalLightRenderer::DirectionalLightRenderer( GeometryStorage* geometryStorage )
   : m_geometryStorage( geometryStorage )
   , m_shadowmapSize( 2048 )
{
   m_textureSampler.setUnified( TAM_CLAMP, TFM_POINT );

   // initialize the roughness lookup texture
   LookupTextures& lookupTextures = TSingleton< LookupTextures >::getInstance();
   m_roughnessLookup = lookupTextures.getCookTorrenceRoughness();

   m_shadowMap = new RenderTarget2DStatic( m_shadowmapSize, m_shadowmapSize, TU_SHADOW_MAP, false );

   // setup basic constants
   m_constantBuffer = ShaderDataBuffer::create< DirectionalLightProperties >();
   DirectionalLightProperties& cascadeProperties = m_constantBuffer->accessData< DirectionalLightProperties >();
   cascadeProperties.m_cascadeOffset[0].v[0] = 0.0f; // cascade 0 offset
   cascadeProperties.m_cascadeOffset[0].v[1] = 0.0f;
   cascadeProperties.m_cascadeOffset[1].v[0] = 0.5f; // cascade 1 offset
   cascadeProperties.m_cascadeOffset[1].v[1] = 0.0f;
   cascadeProperties.m_cascadeOffset[2].v[0] = 0.0f; // cascade 2 offset
   cascadeProperties.m_cascadeOffset[2].v[1] = 0.5f;
   cascadeProperties.m_cascadeOffset[3].v[0] = 0.5f; // cascade 3 offset
   cascadeProperties.m_cascadeOffset[3].v[1] = 0.5f;
   
   cascadeProperties.m_cascadeDistance[0] = 16.0f;
   cascadeProperties.m_cascadeDistance[1] = 64.0f;
   cascadeProperties.m_cascadeDistance[2] = 128.0f;
   cascadeProperties.m_cascadeDistance[3] = 256.0f;
}

///////////////////////////////////////////////////////////////////////////////

DirectionalLightRenderer::~DirectionalLightRenderer()
{
   if ( m_constantBuffer )
   {
      m_constantBuffer->removeReference();
      m_constantBuffer = NULL;
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

void DirectionalLightRenderer::render( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   if ( light->castsShadows() )
   {
      renderWithShadow( renderer, light, gBuffer, outRenderTarget );
   }
   else
   {
      renderWithoutShadow( renderer, light, gBuffer, outRenderTarget );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DirectionalLightRenderer::renderWithShadow( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   // query the geometry to render
   Camera& camera = renderer.getActiveCamera();
   {
      RPVolAxisAlignedBox geometryQueryVolume;
      camera.calculateFrustumAABB( geometryQueryVolume.m_volume );
      geometryQueryVolume.m_volume.setExpanded( geometryQueryVolume.m_volume, Float_2 );
      m_litGeometry.clear();
      m_geometryStorage->queryForShadowMapping( geometryQueryVolume, m_litGeometry );
   }

   SAVE_RENDER_STATE( renderer );

   // render shadow maps
   MemoryAllocator* rtComm = renderer.rtComm();
   new ( rtComm ) RCClearRenderTarget( m_shadowMap );
   new ( rtComm ) RCActivateRenderTarget( m_shadowMap );
   CHANGE_RENDER_STATE( renderer, RSSetBlending( false ) );

   DirectionalLightProperties& cascadeProperties = m_constantBuffer->accessData< DirectionalLightProperties >();
   AxisAlignedBox cascadeCameraVolume[SHADOW_CASCADES_COUNT];
   for ( byte i = 0; i < SHADOW_CASCADES_COUNT; ++i )
   {
      const float nearZ = i == 0 ? 0.0f : cascadeProperties.m_cascadeDistance[i-1];
      const float farZ = cascadeProperties.m_cascadeDistance[i];
      camera.calculateFrustumAABBFromRange( nearZ, farZ, cascadeCameraVolume[i] );
   }

   // set camera related constants
   Matrix invCameraViewMtx;
   {
      invCameraViewMtx.setInverse( camera.getViewMtx() );
      const Matrix& camProjMtx = camera.getProjectionMtx();
      cascadeProperties.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );
   }

   Camera lightCamera( "", Camera::PT_ORTHO );
   renderer.pushCamera( lightCamera );
   for ( byte i = 0; i < SHADOW_CASCADES_COUNT; ++i )
   {
      calculateDirectionalCascadeSegment( i, invCameraViewMtx, cascadeCameraVolume[i], light, lightCamera );

      const Viewport viewport = {
         ( uint ) ( cascadeProperties.m_cascadeOffset[i].v[0] * m_shadowmapSize ),
         ( uint ) ( cascadeProperties.m_cascadeOffset[i].v[1] * m_shadowmapSize ),
         m_shadowmapSize / 2,
         m_shadowmapSize / 2,
         -1.0f,
         1.0f};

      new ( rtComm ) RCSetViewport( viewport );
      renderDirectionalShadowMap( renderer, m_shadowMap );
   }
   new ( rtComm ) RCDeactivateRenderTarget();
   renderer.popCamera();

   // render the light
   CHANGE_RENDER_STATE( renderer, RSSetBlending( true, BLEND_ONE, BLEND_ONE ) );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   new ( rtComm ) RCResetViewport();
   renderCascade( renderer, gBuffer, light, lightCamera, outRenderTarget );

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void DirectionalLightRenderer::calculateDirectionalCascadeSegment( int cascadeIdx, const Matrix& invCameraViewMtx, const AxisAlignedBox& cascadeCameraVolume, const DirectionalLight* light, Camera& outLightCamera )
{
   DirectionalLightProperties& lightProperties = m_constantBuffer->accessData< DirectionalLightProperties >();


   const Matrix& lightGlobalMtx = light->getGlobalMtx();
   AxisAlignedBox shadowmapRenderVolume;
   {
      Matrix lightViewMtx;
      MatrixUtils::calculateViewMtx( lightGlobalMtx, lightViewMtx );
      lightViewMtx.setPosition<3>( Vector_ZERO );

      cascadeCameraVolume.transform( lightViewMtx, shadowmapRenderVolume );
   }

   // remove the jittering effect by constraining the cascade bounds to the pixel boundary
   Vector lightPos;
   {
      Vector shadowVolumeExtents;
      shadowmapRenderVolume.getExtents( shadowVolumeExtents );

      FastFloat fWorldUnitsPerTexel;
      fWorldUnitsPerTexel.setFromFloat(2.0f/( float )m_shadowmapSize);

      Vector vWorldUnitsPerTexel;
      vWorldUnitsPerTexel.setMul( shadowVolumeExtents, fWorldUnitsPerTexel );

      shadowmapRenderVolume.min.div( vWorldUnitsPerTexel );
      shadowmapRenderVolume.min.floor();
      shadowmapRenderVolume.min.mul( vWorldUnitsPerTexel );

      shadowmapRenderVolume.max.div( vWorldUnitsPerTexel );
      shadowmapRenderVolume.max.floor();
      shadowmapRenderVolume.max.mul( vWorldUnitsPerTexel );

      Vector shadowVolumeCenter;
      cascadeCameraVolume.getCenter( shadowVolumeCenter );
      lightPos.setMulAdd( lightGlobalMtx.forwardVec(), FastFloat::fromFloat( shadowVolumeExtents[2] * -0.5f ), shadowVolumeCenter );

      lightPos.div( vWorldUnitsPerTexel );
      lightPos.floor();
      lightPos.mul( vWorldUnitsPerTexel );
   }

   // Use the light matrix as the view-projection mtx.
   // We can calculate it here, 'cause up until this point we didn't know the size of the shadow bounds
   {
      Vector shadowVolumeExtents;
      shadowmapRenderVolume.getExtents( shadowVolumeExtents );

      // we are always shooting the shadow map from the same height - we don't want each cascade to clip
      // objects at different eights now, do we.
      // Plus,. if we do this, we get a consistent depth across all shadow maps
      shadowVolumeExtents[2] = lightProperties.m_cascadeDistance[SHADOW_CASCADES_COUNT-1];

      outLightCamera.setNearPlaneDimensions( shadowVolumeExtents[0], shadowVolumeExtents[1] );
      outLightCamera.setClippingPlanes( 0.0f, shadowVolumeExtents[2] * 2.0f );

      outLightCamera.setLocalMtx( lightGlobalMtx );
      outLightCamera.accessLocalMtx().setPosition<3>( lightPos );
      outLightCamera.updateTransforms();
   }

   {

      // projection properties for calculating inverse projection
      const Matrix& lightProjMtx = outLightCamera.getProjectionMtx();

      Matrix lightViewProjMtx;
      lightViewProjMtx.setMul( outLightCamera.getViewMtx(), lightProjMtx );
      lightProperties.m_camViewToLightProjMtx[cascadeIdx].setMul( invCameraViewMtx, lightViewProjMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DirectionalLightRenderer::renderDirectionalShadowMap( Renderer& renderer, RenderTarget2D* outShadowMap )
{
   // render the shadow map using the visible geometry
   RenderTree* renderTree = renderer.buildRenderTree( m_litGeometry );
   if ( renderTree )
   {
      MemoryAllocator* rtComm = renderer.rtComm();

      // activate the render targets we want to render to and clean it
      renderTree->render( RP_ShadowMap );

      // get rid of the tree
      delete renderTree;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DirectionalLightRenderer::renderCascade( Renderer& renderer, RenderTarget2D* gBuffer, const DirectionalLight* light, const Camera& lightCamera, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_directionalLightWithShadowShader.get();
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

      DirectionalLightProperties& lightProperties = m_constantBuffer->accessData< DirectionalLightProperties >();

      // transform the direction to view space  
      viewMtx.transformNorm( lightWorldSpace.forwardVec(), lightProperties.m_direction );

      // setup the light color and its strength
      lightProperties.m_lightColor.setMul( light->m_color, light->m_strength );
   }

   // render the light
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
   bindPS->setDataBuf( "LightProperties", m_constantBuffer );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();
}

///////////////////////////////////////////////////////////////////////////////

void DirectionalLightRenderer::renderWithoutShadow( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_directionalLightWithoutShadowShader.get();
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

      DirectionalLightProperties& lightProperties = m_constantBuffer->accessData< DirectionalLightProperties >();

      // projection properties for calculating inverse projection
      const Matrix& camProjMtx = activeCamera.getProjectionMtx();
      lightProperties.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );

      // transform the direction to view space  
      viewMtx.transformNorm( lightWorldSpace.forwardVec(), lightProperties.m_direction );

      // setup the light color and its strength
      lightProperties.m_lightColor.setMul( light->m_color, light->m_strength );
   }

   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetBlending( true, BLEND_ONE, BLEND_ONE ) );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

   // render the light
   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( outRenderTarget );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setDepthBuffer( "g_DepthBuffer", gBuffer );
   bindPS->setTexture( "g_Diffuse", gBuffer, m_textureSampler, GBuf_Albedo );
   bindPS->setTexture( "g_Specular", gBuffer, m_textureSampler, GBuf_Specular );
   bindPS->setTexture( "g_BRDF", gBuffer, m_textureSampler, GBuf_BRDF );
   bindPS->setTexture( "g_Normals", gBuffer, m_textureSampler, GBuf_Normals );
   bindPS->setTexture( "g_RoughnessLookup", m_roughnessLookup, m_textureSampler );
   bindPS->setDataBuf( "LightProperties", m_constantBuffer );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////
