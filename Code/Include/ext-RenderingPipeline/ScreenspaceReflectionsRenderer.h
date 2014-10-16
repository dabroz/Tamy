/// @file   ext-RenderingPipeline\ScreenspaceReflectionsRenderer.h
/// @brief  screen space reflections renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Vector.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderTarget2D;
class ShaderDataBuffer;
class Shader;

///////////////////////////////////////////////////////////////////////////////

struct ScreenspaceReflectionParams
{
   Vector            m_camProjParams;
   Matrix            m_camProjMtx;
};

///////////////////////////////////////////////////////////////////////////////

class ScreenspaceReflectionsRenderer
{
   DECLARE_ALLOCATOR( ScreenspaceReflectionsRenderer, AM_DEFAULT );

private:
   static TResourceHandle< Shader >                m_shader;

   TextureSamplerSettings                          m_textureSampler;
   ShaderDataBuffer*                               m_constants;

public:
   /**
    * Constructor.
    */
   ScreenspaceReflectionsRenderer();
   ~ScreenspaceReflectionsRenderer();

   /**
    * Renders the post process effect.
    *
    * @param renderer
    * @param gBuffer
    * @param outRenderTarget
    */
   void render( Renderer& renderer, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );
};

///////////////////////////////////////////////////////////////////////////////
