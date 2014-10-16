/// @file   ext-RenderingPipeline\PointLightRenderer.h
/// @brief  a point light renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class PointLight;
class Renderer;
class RenderTarget2D;
class RenderTargetCube;
class ProceduralTexture;
class GeometryComponent;
class ShaderDataBuffer;
class Camera;
class GeometryStorage;
class Shader;
class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

struct PointLightProperties
{
   Vector                           m_camProjParams;
   Matrix                           m_invCamViewMtx;
   //----------------------------------- (16 byte boundary)
   Vector                           m_position;
   //----------------------------------- (16 byte boundary)
   Color                            m_lightColor;
   //----------------------------------- (16 byte boundary)
   float                            m_radius;
   float                            m_falloff;
   int                              m_padding[2];
   //----------------------------------- (16 byte boundary)
};

///////////////////////////////////////////////////////////////////////////////

class PointLightRenderer
{
   DECLARE_ALLOCATOR( PointLightRenderer, AM_DEFAULT );

private:
   static TResourceHandle< Shader >                m_pointLightWithShadowShader;
   static TResourceHandle< Shader >                m_pointLightWithoutShadowShader;
   static TResourceHandle< TriangleMesh >          m_pointLightMesh;

   uint                                            m_shadowmapSize;

   GeometryStorage*                                m_geometryStorage;
   ShaderDataBuffer*                               m_vertexShaderConstants;
   ShaderDataBuffer*                               m_fragmentShaderConstants;
   RenderTargetCube*                               m_shadowMap;

   TextureSamplerSettings                          m_textureSampler;
   ProceduralTexture*                              m_roughnessLookup;

   Array< GeometryComponent* >                     m_litGeometry;

   Matrix                                          m_cameraRotation[6];

public:
   /**
    * Constructor.
    */
   PointLightRenderer( GeometryStorage* geometryStorage );
   ~PointLightRenderer();

   /**
   * Renders the light.
   *
   * @param renderer
   * @param light
   * @param gBuffer
   * @param outRenderTarget
   */
   void render( Renderer& renderer, const PointLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );

private:
   void renderShadowMap( Renderer& renderer, const PointLight* light );
   void renderLight( Renderer& renderer, const Shader* shader, const PointLight* light, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );
};

///////////////////////////////////////////////////////////////////////////////
