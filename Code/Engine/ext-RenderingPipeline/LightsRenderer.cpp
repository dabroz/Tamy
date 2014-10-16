#include "ext-RenderingPipeline\LightsRenderer.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderStateChangeTracker.h"

// light renderers
#include "ext-RenderingPipeline\DirectionalLightRenderer.h"
#include "ext-RenderingPipeline\PointLightRenderer.h"
#include "ext-RenderingPipeline\SpotLightRenderer.h"

// lights
#include "core-Renderer\Light.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\PointLight.h"
#include "core-Renderer\SpotLight.h"


///////////////////////////////////////////////////////////////////////////////

LightsRenderer::LightsRenderer( Array< class Light* >* lights, class GeometryStorage* sceneGeometryStorage )
   : m_lights( lights )
{
   m_directionalLightRenderer = new DirectionalLightRenderer( sceneGeometryStorage );
   m_pointLightRenderer = new PointLightRenderer( sceneGeometryStorage );
   m_spotLightRenderer = new SpotLightRenderer( sceneGeometryStorage );
}

///////////////////////////////////////////////////////////////////////////////

LightsRenderer::~LightsRenderer()
{
   delete m_directionalLightRenderer;
   m_directionalLightRenderer = NULL;

   delete m_pointLightRenderer;
   m_pointLightRenderer = NULL;

   delete m_spotLightRenderer;
   m_spotLightRenderer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void LightsRenderer::render( class Renderer& renderer, class RenderTarget2D* gBuffer, class RenderTarget2D* outRenderTarget )
{
   const uint lightsCount = m_lights->size();

   uint bufferIdx = 0;
   for ( uint lightIdx = 0; lightIdx < lightsCount; ++lightIdx )
   {
      // set the context data
      Light* light = (*m_lights)[lightIdx];
      if ( !light->isEnabled() )
      {
         // the light is disabled - move on to the next one
         continue;
      }
      
      switch ( light->getType() )
      {
         case Light::LT_Directional:
         {
            DirectionalLight* dirLight = static_cast< DirectionalLight* >( light );
            m_directionalLightRenderer->render( renderer, dirLight, gBuffer, outRenderTarget );
            break;
         }

         case Light::LT_Point:
         {
            PointLight* pointLight = static_cast< PointLight* >( light );
            m_pointLightRenderer->render( renderer, pointLight, gBuffer, outRenderTarget );
            break;
         }

         case Light::LT_Spot:
         {
            SpotLight* spotLight = static_cast< SpotLight* >( light );
            m_spotLightRenderer->render( renderer, spotLight, gBuffer, outRenderTarget );
            break;
         }
      }
   }

}

///////////////////////////////////////////////////////////////////////////////
