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
   , m_splitFrustumVertices( 4 * 8 )
   , m_clippingRanges( 5 )
{
   m_textureSampler.setUnified( TAM_CLAMP, TFM_POINT );
   m_noiseMapSampler.setUnified( TAM_WRAP, TFM_LINEAR );

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
   
   cascadeProperties.m_cascadeDistance[0] = 8.0f;
   cascadeProperties.m_cascadeDistance[1] = 16.0f;
   cascadeProperties.m_cascadeDistance[2] = 48.0f;
   cascadeProperties.m_cascadeDistance[3] = 128.0f;

   m_clippingRanges.resize( 5, 0.0f );
   m_clippingRanges[1] = cascadeProperties.m_cascadeDistance[0];
   m_clippingRanges[2] = cascadeProperties.m_cascadeDistance[1];
   m_clippingRanges[3] = cascadeProperties.m_cascadeDistance[2];
   m_clippingRanges[4] = cascadeProperties.m_cascadeDistance[3];

   m_splitFrustumVertices.resize( 4 * 8, Vector_ZERO );

   // initialize the random sampling texture
   m_noiseMap = new ProceduralTexture( 16, 16 );
   {
      PTVector* randomTextureBuf = ( PTVector* ) m_noiseMap->accessImageBuffer();

      Vector tmpPixel;
      for ( int i = 0; i < 256; ++i )
      {
         tmpPixel.set( randRange( -1.0f, 1.0 ), randRange( -1.0f, 1.0 ), 0.0f );
         tmpPixel.normalize();

         PTVector& pixel = randomTextureBuf[i];
         pixel.fromVectorAsNormal( tmpPixel );
      }
   }

   // initialize the samping kernel
   calculateKernel( MAX_CSD_KERNEL_SIZE );
}

///////////////////////////////////////////////////////////////////////////////

DirectionalLightRenderer::~DirectionalLightRenderer()
{
   m_noiseMap->removeReference();
   m_noiseMap = NULL;

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

void DirectionalLightRenderer::calculateKernel( uint size )
{
   size = min2<uint>( size, MAX_CSD_KERNEL_SIZE );

   DirectionalLightProperties& settings = m_constantBuffer->accessData< DirectionalLightProperties >();
   settings.m_sampleKernelSize = size;

   for ( uint i = 0; i < size; ++i )
   {
      settings.m_kernel[i].set(
         randRange( -1.0f, 1.0 ),
         randRange( -1.0f, 1.0 ),
         randRange( 0.0f, 1.0 ) );

      settings.m_kernel[i].normalize();

      float scale = float( i ) / float( size );
      scale = lerp( 0.1f, 1.0f, scale * scale );
      settings.m_kernel[i].mul( FastFloat::fromFloat( scale ) );
   }
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
   MemoryAllocator* rtComm = renderer.rtComm();
   Camera lightCamera( "", Camera::PT_ORTHO );

   // render shadow maps
   {
      new( rtComm ) RCClearRenderTarget( m_shadowMap );
      new ( rtComm ) RCActivateRenderTarget( m_shadowMap );
      CHANGE_RENDER_STATE( renderer, RSSetBlending( false ) );

      // set camera related constants
      DirectionalLightProperties& cascadeProperties = m_constantBuffer->accessData< DirectionalLightProperties >();
      {
         const Matrix& camViewMtx = camera.getViewMtx();
         cascadeProperties.m_invViewMtx.setInverse( camViewMtx );

         const Matrix& camProjMtx = camera.getProjectionMtx();
         cascadeProperties.m_camProjParams.set( camProjMtx( 0, 0 ), camProjMtx( 1, 1 ), camProjMtx( 2, 2 ), camProjMtx( 3, 2 ) );
      }

      // split the camera frustum
      camera.splitFrustum( m_clippingRanges, m_splitFrustumVertices );

      renderer.pushCamera( lightCamera );
      for ( byte i = 0; i < SHADOW_CASCADES_COUNT; ++i )
      {
         calculateDirectionalCascadeSegment( i, light, lightCamera );

         const Viewport viewport = {
            ( uint ) ( cascadeProperties.m_cascadeOffset[i].v[0] * m_shadowmapSize ),
            ( uint ) ( cascadeProperties.m_cascadeOffset[i].v[1] * m_shadowmapSize ),
            m_shadowmapSize / 2,
            m_shadowmapSize / 2,
            -1.0f,
            1.0f };

         new ( rtComm ) RCSetViewport( viewport );
         renderDirectionalShadowMap( renderer, m_shadowMap );
      }
      new ( rtComm ) RCDeactivateRenderTarget();
      renderer.popCamera();
   }

   // render the light
   {
      CHANGE_RENDER_STATE( renderer, RSSetBlending( true, BLEND_ONE, BLEND_ONE ) );
      CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );

      new ( rtComm ) RCResetViewport();
      renderCascade( renderer, gBuffer, light, lightCamera, outRenderTarget );
   }

   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////


void DirectionalLightRenderer::calculateDirectionalCascadeSegment( int cascadeIdx, const DirectionalLight* light, Camera& outLightCamera )
{
   DirectionalLightProperties& cascadeProperties = m_constantBuffer->accessData< DirectionalLightProperties >();

   AxisAlignedBox lightSpaceBounds;
   Matrix invLightViewMtx;
   {  
      // First, we need to define the bounding box around the cascade frustum in world space.
      const uint startVtx = cascadeIdx * 8;
      const uint endVtx = startVtx + 8;
      AxisAlignedBox worldSpaceBounds;
      for ( int i = startVtx; i < endVtx; ++i )
      {
         worldSpaceBounds.include( m_splitFrustumVertices[i] );
      }

      cascadeProperties.m_cascadeMin[cascadeIdx] = worldSpaceBounds.min;
      cascadeProperties.m_cascadeMax[cascadeIdx] = worldSpaceBounds.max;

      // The next thing to do is tell how large are those bounds when looking down
      // the direction of the light.
      // In order to do that, we'll simply transform the bounds to the light's view space
      const Matrix& lightGlobalMtx = light->getGlobalMtx();
      Matrix lightViewMtx;
      MatrixUtils::calculateViewMtx( lightGlobalMtx, lightViewMtx );
      lightViewMtx.setPosition<3>( Vector_ZERO );
      invLightViewMtx.setInverse( lightViewMtx );
      worldSpaceBounds.transform( lightViewMtx, lightSpaceBounds );
   }
   
   // pad the bounds so that they always have the same size
   {
      const uint diagonalStartVtx = cascadeIdx * 8;
      const uint diagonalEndVtx = diagonalStartVtx + 7;

      Vector diagonal;
      diagonal.setSub( m_splitFrustumVertices[diagonalStartVtx], m_splitFrustumVertices[diagonalEndVtx] );

      Vector diagonalLength;
      diagonalLength.setBroadcast( diagonal.length() );

      // The offset calculated will pad the orthogonal projection so that it is always the same size 
      // and big enough to cover the entire cascade interval.
      Vector lightSpaceVolumeExtents;
      lightSpaceBounds.getExtents( lightSpaceVolumeExtents );

      Vector broaderOffset;
      broaderOffset.setMulAdd( lightSpaceVolumeExtents, Float_MinusInv2, diagonalLength );

      lightSpaceBounds.min.sub( broaderOffset );
      lightSpaceBounds.max.add( broaderOffset );
   }
   

   // quantize the rendering bounds to remove the shadow edge shimmering effect
   {
      Vector lightSpaceVolumeExtents;
      lightSpaceBounds.getExtents( lightSpaceVolumeExtents );

      Vector worldSpaceTexelSize;
      worldSpaceTexelSize.setDiv( lightSpaceVolumeExtents, FastFloat::fromFloat( 0.5f * m_shadowmapSize ) );

      Vector remainder;
      remainder.setFloatRemainder( lightSpaceBounds.min, worldSpaceTexelSize );
      lightSpaceBounds.min.sub( remainder );

      remainder.setFloatRemainder( lightSpaceBounds.max, worldSpaceTexelSize );
      lightSpaceBounds.max.sub( remainder );
   }

   DirectionalLightProperties& lightProperties = m_constantBuffer->accessData< DirectionalLightProperties >();
   {
      // next step is to tell determine the extents of the bounds and 
      // the central point, where we'll place the camera
      Vector lightSpaceVolumeExtents, lightSpaceVolumeCenter;
      lightSpaceBounds.getExtents( lightSpaceVolumeExtents );
      lightSpaceBounds.getCenter( lightSpaceVolumeCenter );
      
      Vector worldSpaceVolumeCenterFromLightSpace;
      invLightViewMtx.transform( lightSpaceVolumeCenter, worldSpaceVolumeCenterFromLightSpace );

      // we are always shooting the shadow map from the same height - we don't want each cascade to clip
      // objects at different heights now, do we.
      // Plus,if we do this, we get a consistent depth across all shadow maps
      const float farClippingPlaneZ = lightProperties.m_cascadeDistance[SHADOW_CASCADES_COUNT - 1];

      outLightCamera.setNearPlaneDimensions( lightSpaceVolumeExtents[0], lightSpaceVolumeExtents[1] );
      outLightCamera.setClippingPlanes( 0.0f, farClippingPlaneZ * 2.0f );

      // and finally - define the light camera's transform - having it look in the direction
      // of the light, but being position directly in the center of the rendered volume
      const Matrix& lightGlobalMtx = light->getGlobalMtx();
      outLightCamera.setLocalMtx( lightGlobalMtx );
      outLightCamera.accessLocalMtx().setPosition<3>( worldSpaceVolumeCenterFromLightSpace );
      outLightCamera.updateTransforms();
   }

   {
      // projection properties for calculating inverse projection
      const Matrix& lightProjMtx = outLightCamera.getProjectionMtx();
      
      Matrix lightViewProjMtx;
      lightViewProjMtx.setMul( outLightCamera.getViewMtx(), lightProjMtx );
      lightProperties.m_camViewToLightProjMtx[cascadeIdx].setMul( cascadeProperties.m_invViewMtx, lightViewProjMtx );
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
   bindPS->setTexture( "g_NoiseMap", m_noiseMap, m_noiseMapSampler );
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
