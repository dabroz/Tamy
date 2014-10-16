/// @file   ext-RenderingPipeline\PointLightRenderer.h
/// @brief  a point light renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class SpotLight;
class Renderer;
class RenderTarget2D;
class ProceduralTexture;
class GeometryComponent;
class ShaderDataBuffer;
class Camera;
class GeometryStorage;
class Shader;
class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

struct SpotLightProperties
{
   Vector                           m_camProjParams;
   Vector                           m_lightProjParams;
   Matrix                           m_camViewToLightProjMtx;
   //----------------------------------- (16 byte boundary)
   Vector                           m_position;
   //----------------------------------- (16 byte boundary)
   Vector                           m_direction;
   //----------------------------------- (16 byte boundary)
   Color                            m_lightColor;
   //----------------------------------- (16 byte boundary)
   float                            m_range;
   float                            m_falloff;
   float                            m_spotAngle;
   int                              m_padding;
   //----------------------------------- (16 byte boundary)
};

///////////////////////////////////////////////////////////////////////////////

class SpotLightRenderer
{
   DECLARE_ALLOCATOR( SpotLightRenderer, AM_DEFAULT );

private:
   static TResourceHandle< Shader >                m_spotLightWithShadowShader;
   static TResourceHandle< Shader >                m_spotLightWithoutShadowShader;

   uint                                            m_shadowmapSize;

   GeometryStorage*                                m_geometryStorage;
   ShaderDataBuffer*                               m_vertexShaderConstants;
   ShaderDataBuffer*                               m_fragmentShaderConstants;
   RenderTarget2D*                                 m_shadowMap;

   TextureSamplerSettings                          m_textureSampler;
   ProceduralTexture*                              m_roughnessLookup;

   Array< GeometryComponent* >                     m_litGeometry;

public:
   /**
    * Constructor.
    */
   SpotLightRenderer( GeometryStorage* geometryStorage );
   ~SpotLightRenderer();

   /**
    * Renders the light.
    *
    * @param renderer
    * @param light
    * @param gBuffer
    * @param outRenderTarget
    */
   void render( Renderer& renderer, const SpotLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );

private:
   void renderLight( Renderer& renderer, const SpotLight* light, RenderTarget2D* gBuffer, const Shader* shader, RenderTarget2D* outRenderTarget );
   void renderShadowMap( Renderer& renderer, const SpotLight* spotLight );
};

///////////////////////////////////////////////////////////////////////////////
