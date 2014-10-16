/// @file   ext-RenderingPipeline\LightsRenderer.h
/// @brief  deferred lights renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class DirectionalLightRenderer;
class PointLightRenderer;
class SpotLightRenderer;
class Renderer;
class RenderTarget2D;
class GeometryStorage;
class Shader;
class Light;

///////////////////////////////////////////////////////////////////////////////

class LightsRenderer
{
   DECLARE_ALLOCATOR( LightsRenderer, AM_DEFAULT );

private:
   Array< Light* >*                                m_lights;
   DirectionalLightRenderer*                       m_directionalLightRenderer;
   PointLightRenderer*                             m_pointLightRenderer;
   SpotLightRenderer*                              m_spotLightRenderer;

public:
   /**
    * Constructor.
    *
    * @param lights
    * @param sceneGeometryStorage
    */
   LightsRenderer( Array< Light* >* lights, GeometryStorage* sceneGeometryStorage );
   ~LightsRenderer();

   /**
    * Renders lights.
    *
    * @param renderer
    * @param gBuffer
    * @param outRenderTarget
    */
   void render( Renderer& renderer, RenderTarget2D* gBuffer, RenderTarget2D* outRenderTarget );

};

///////////////////////////////////////////////////////////////////////////////
