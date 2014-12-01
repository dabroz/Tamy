/// @file   ext-RenderingPipeline\DirectionalLightRenderer.h
/// @brief  directional light renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Vector.h"
#include "core\TVector.h"
#include "core\Matrix.h"
#include "core\Color.h"

///////////////////////////////////////////////////////////////////////////////

#define SHADOW_CASCADES_COUNT				4

///////////////////////////////////////////////////////////////////////////////

class DirectionalLight;
class Renderer;
class RenderTarget2D;
struct AxisAlignedBox;
class ProceduralTexture;
class GeometryComponent;
class ShaderDataBuffer;
class Camera;
class GeometryStorage;
class Shader;

///////////////////////////////////////////////////////////////////////////////

#define                             MAX_CSD_KERNEL_SIZE 32

///////////////////////////////////////////////////////////////////////////////

struct DirectionalLightProperties
{
   Vector                           m_camProjParams;
   Matrix                           m_invViewMtx;
   Matrix                           m_camViewToLightProjMtx[SHADOW_CASCADES_COUNT];
   TVector<2>                       m_cascadeOffset[SHADOW_CASCADES_COUNT];
   Vector				               m_cascadeMin[SHADOW_CASCADES_COUNT];
   Vector				               m_cascadeMax[SHADOW_CASCADES_COUNT];
   Vector                           m_cascadeDistance;
   //----------------------------------- (16 byte boundary)
   Vector                           m_kernel[MAX_CSD_KERNEL_SIZE];
   //----------------------------------- (16 byte boundary)
   Vector                           m_direction;
   //----------------------------------- (16 byte boundary)
   Color                            m_lightColor;
   //----------------------------------- (16 byte boundary)
   int                              m_sampleKernelSize;
   int                              m_padding[3];
   //----------------------------------- (16 byte boundary)
};

///////////////////////////////////////////////////////////////////////////////

class DirectionalLightRenderer
{
   DECLARE_ALLOCATOR( DirectionalLightRenderer, AM_DEFAULT );

private:
   static TResourceHandle< Shader >       m_directionalLightWithShadowShader;
   static TResourceHandle< Shader >       m_directionalLightWithoutShadowShader;

   uint                                   m_shadowmapSize;

   GeometryStorage*                       m_geometryStorage;
   ShaderDataBuffer*                      m_constantBuffer;
   RenderTarget2D*                        m_shadowMap;

   TextureSamplerSettings                 m_textureSampler;
   ProceduralTexture*                     m_roughnessLookup;

   TextureSamplerSettings                 m_noiseMapSampler;
   class ProceduralTexture*               m_noiseMap;

   Array< float >                         m_clippingRanges;
   Array< Vector >                        m_splitFrustumVertices;
   Array< GeometryComponent* >            m_litGeometry;

public:
   /**
    * Constructor.
    */
   DirectionalLightRenderer( GeometryStorage* geometryStorage );
   ~DirectionalLightRenderer();

   /**
    * Renders the light.
    *
    * @param renderer
    * @param light
    * @param gBuffer
    * @param outRenderTarget
    */
   void render( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );

private:
   void renderWithShadow( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );
   void renderWithoutShadow( Renderer& renderer, const DirectionalLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );

   void calculateDirectionalCascadeSegment( int cascadeIdx, const DirectionalLight* light, Camera& outLightCamera );
   void renderDirectionalShadowMap( Renderer& renderer, RenderTarget2D* outShadowMap );
   void renderCascade( Renderer& renderer, RenderTarget2D* gBuffer, const DirectionalLight* light, const Camera& lightCamera, RenderTarget2D* outRenderTarget );
   void calculateKernel( uint size );
};

///////////////////////////////////////////////////////////////////////////////
