/// @file   ext-Renderer\DeferredRenderingMechanism.h
/// @brief  a rendering mechanism using the forward rendering method
#pragma once 

#include "core\MemoryRouter.h"
#include "core-Renderer\RenderingMechanism.h"
#include "core\Array.h"
#include "core\Color.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"


///////////////////////////////////////////////////////////////////////////////

enum ForwardRenderingSceneId
{
   FRS_Main,
   FRS_Debug,
   FRS_Gizmo,

   FRS_ScenesCount
};

enum ForwardRenderTargetId
{
   FRT_Final,

   FRT_Scene,
   FRT_Selection,
   FRT_Gizmo,
   FRT_Intermediate,
   FRT_SelectionQuery,
   
   FRT_TargetsCount
};

enum RenderPassSelection
{
   RPass_All,
   RPass_SelectionQuery, 
   RPass_AmbientOcclusion,
   RPass_ScreenspaceReflections,
};

///////////////////////////////////////////////////////////////////////////////

class EntitiesGroup;
class Model;
class Light;
class Shader;

///////////////////////////////////////////////////////////////////////////////

class DeferredRenderingMechanism : public RenderingMechanism
{
   DECLARE_ALLOCATOR( DeferredRenderingMechanism, AM_DEFAULT );

private:
   RenderPassSelection                       m_selectedRenderPass;

   // utils
   bool                                      m_isInitialized;

   // scenes & groups
   Model*                                    m_scenes[FRS_ScenesCount];
   EntitiesGroup*                            m_selectedEntities;

   // settings
   Color                                     m_selectionColor;

   // render targets
   class RenderTarget2D*                     m_gBuffer;
   class RenderTarget2D*                     m_renderTargets[FRT_TargetsCount];

   // collectors
   class CustomComponentsView*               m_customComponentsView;
   class LightsView*                         m_lightsView;
   class GeometryView*                       m_mainGeometryView;
   class GeometryView*                       m_selectedGeometryView;
   class GeometryView*                       m_gizmoGeometryView;
   class RPVolFrustum*                       m_cameraVolume;
   class RPVolAxisAlignedBox*                m_spaceVolume;

   // fixed lights for the debug draw
   Array< Light* >                           m_fixedDebugLights;

   // renderers
   class SceneRenderer*                      m_mainSceneRenderer;
   class SceneRenderer*                      m_gizmoSceneRenderer;
   class SceneRenderer*                      m_selectionRenderer;
   class SkyboxRenderer*                     m_skyboxRenderer;
   class AmbientOcclusionPass*               m_ambientOcclusionPass;
   class TexturesBlender*                    m_selectionResultsBlender;
   class TexturesBlender*                    m_gizmoBlender;
   class LightsRenderer*                     m_lightsRenderer;
   class ScreenspaceReflectionsRenderer*     m_screenSpaceReflectionsRenderer;

public:
   /**
    * Constructor.
    */
   DeferredRenderingMechanism();
   ~DeferredRenderingMechanism();

   /**
    * Selects which render pass to render.
    *
    * @pass
    */
   void selectRenderPass( RenderPassSelection pass ) {
      m_selectedRenderPass = pass;
   }

   /**
    * A helper method that defines the output layout of a material fragment shader
    * that is expected by this rendering mechanism.
    */
   static void defineFragmentShaderOutput( class MNOutputStruct* node );

   // -------------------------------------------------------------------------
   // Scenes management
   // -------------------------------------------------------------------------

   /**
    * Assigns a scene.
    *
    * @param sceneContentsId
    * @param scene
    */
   void assignScene( ForwardRenderingSceneId sceneContentsId, Model* scene );

   /**
    * Assigns an entities group that will be rendered as 'selected'.
    *
    * @param group
    */
   void assignSelectionGroup( EntitiesGroup* group );

   // -------------------------------------------------------------------------
   // RenderingMechanism implementation
   // -------------------------------------------------------------------------
   void initialize( Renderer& renderer ) override;
   void deinitialize( Renderer& renderer ) override;
   void render( Renderer& renderer ) override;

private:
   // -------------------------------------------------------------------------
   // Render passes
   // -------------------------------------------------------------------------
   void debugPass( Renderer& renderer, RenderTarget2D* sceneTarget, RenderTarget2D* outRenderTarget );
   void selectionQueryPass( Renderer& renderer, RenderTarget2D* outRenderTarget );
};

///////////////////////////////////////////////////////////////////////////////
