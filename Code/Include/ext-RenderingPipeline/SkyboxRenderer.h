/// @file   ext-RenderingPipeline\SkyboxRenderer.h
/// @brief  a skybox renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\ResourceHandle.h"
#include "core\Quaternion.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class SkyboxRenderer
{
   DECLARE_ALLOCATOR( SkyboxRenderer, AM_DEFAULT );

private:
   static TResourceHandle< class Shader >                m_skyboxShader;
   static TResourceHandle< class TriangleMesh >          m_skyboxMesh;

   class ShaderDataBuffer*                               m_constantsBuf;
   TextureSamplerSettings                                m_albedoSamplerSettings;
   TextureSamplerSettings                                m_cubeMapSamplerSettings;
   class RenderableCubeTexture*                          m_renderTexture;

   Quaternion                                            m_skyboxRot;

public:
   /**
    * Constructor.
    */
   SkyboxRenderer();
   ~SkyboxRenderer();

   /**
    * Assigns the skybox a new texture.
    *
    * @param texture
    */
   void setTexture( RenderableCubeTexture* texture );

   /**
    * Renders the skybox.
    *
    * @param renderer
    * @param gBuffer
    * @param outRenderTarget
    */
   void render( class Renderer& renderer, class RenderTarget2D* gBuffer, class RenderTarget2D* outRenderTarget );
};

///////////////////////////////////////////////////////////////////////////////
