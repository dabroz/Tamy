/// @file   core-Renderer/IndexedDirectionalLightRenderer.h
/// @brief  utility class that renders a directional light using the indexed rendering method
#pragma once

#include "core\Array.h"
#include "core-Renderer\VertexShaderConfigurator.h"
#include "core-Renderer\CascadedShadowsUtils.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class DirectionalLight;
struct IndexedLightingRenderData;
class PixelShader;
class Geometry;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility class that renders a directional light using the indexed rendering method.
 */
class IndexedDirectionalLightRenderer
{
private:
   struct VSSetter : public VertexShaderConfigurator
   {
      Matrix               m_lightViewProjMtx;

      /**
       * Constructor.
       *
       * @param lightCamera
       */
      VSSetter( Camera& lightCamera );

      // ----------------------------------------------------------------------
      // VertexShaderConfigurator implementation
      // ----------------------------------------------------------------------
      void configure( const Geometry& geometry, RCBindVertexShader* command );
   };

   TResourceHandle< PixelShader >         m_lightingShader;
   TResourceHandle< PixelShader >         m_shadowDepthMapShader;
   TResourceHandle< PixelShader >         m_shadowProjectionPS;

   Array< Geometry* >                     m_visibleGeometry;
   CascadesConfig                         m_cascadeCalculationConfig;
   CascadeStage*                          m_calculatedCascadeStages;

public:
   /**
    * Constructor.
    * 
    * @param cascadesCount
    * @param cascadesPerRow
    */
   IndexedDirectionalLightRenderer( uint cascadesCount, uint cascadesPerRow );
   ~IndexedDirectionalLightRenderer();

   /**
    * Renders the light
    *
    * @param renderer
    * @param light
    * @param data
    * @param lightIdx
    */
   void render( Renderer& renderer, const DirectionalLight* light, const IndexedLightingRenderData& data, int lightIdx );

private:
   // -------------------------------------------------------------------------
   // Lighting computation related methods
   // -------------------------------------------------------------------------
   void renderLighting( Renderer& renderer, const DirectionalLight* light, const IndexedLightingRenderData& data, bool drawShadows, int lightIdx );

   // -------------------------------------------------------------------------
   // Shadow map computation related methods
   // -------------------------------------------------------------------------
   void renderShadowMap( Renderer& renderer, const DirectionalLight* light, const IndexedLightingRenderData& data );

   void renderCascades( Renderer& renderer, Camera& activeCamera, Camera& lightCamera, const IndexedLightingRenderData& data );

   void combineCascades( Renderer& renderer, const IndexedLightingRenderData& data, Camera& activeCamera, Camera& lightCamera, float cascadeDimensions );

};

///////////////////////////////////////////////////////////////////////////////
