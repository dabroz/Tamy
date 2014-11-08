#include "ext-RenderingPipeline\DeferredRenderingMechanism.h"

// renderer
#include "core-Renderer\Renderer.h"
#include "core-Renderer\BasicRenderCommands.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\RendererQuery.h"
#include "core-Renderer\RenderableCubeTexture.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\MNOutputStruct.h"

// render surfaces
#include "core-Renderer\RenderTarget2D.h"

// custom components
#include "ext-RenderingPipeline\LocationRenderSettings.h"

// collectors
#include "ext-RenderingPipeline\LightsView.h"
#include "ext-RenderingPipeline\GeometryView.h"
#include "ext-RenderingPipeline\RPDataProxies.h"
#include "ext-RenderingPipeline\CustomComponentsView.h"

// logging
#include "core\Log.h"
#include "core\Assert.h"

// tools
#include "core-Renderer\RendererUtils.h"

// math
#include "core\AxisAlignedBox.h"

// scenes
#include "core-MVC\Model.h"
#include "core-MVC\EntitiesGroup.h"

// passes
#include "ext-RenderingPipeline\SceneRenderer.h"
#include "ext-RenderingPipeline\SkyboxRenderer.h"
#include "ext-RenderingPipeline\AmbientOcclusionPass.h"
#include "ext-RenderingPipeline\TexturesBlender.h"
#include "ext-RenderingPipeline\LightsRenderer.h"
#include "ext-RenderingPipeline\ScreenspaceReflectionsRenderer.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( RenderPassSelection );
   ENUM_VAL( RPass_All );
   ENUM_VAL( RPass_SelectionQuery );
   ENUM_VAL( RPass_AmbientOcclusion );
   ENUM_VAL( RPass_ScreenspaceReflections );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

DeferredRenderingMechanism::DeferredRenderingMechanism()
   : m_selectedRenderPass( RPass_All )
   , m_isInitialized( false )
   , m_gBuffer( NULL )
   , m_selectedEntities( NULL )
   , m_selectionResultsBlender( NULL )
   , m_gizmoBlender( NULL )
   , m_mainSceneRenderer( NULL )
   , m_gizmoSceneRenderer( NULL )
   , m_skyboxRenderer( NULL )
   , m_mainGeometryView( NULL )
   , m_selectedGeometryView( NULL )
   , m_gizmoGeometryView( NULL )
   , m_ambientOcclusionPass( NULL )
   , m_lightsRenderer( NULL )
   , m_screenSpaceReflectionsRenderer( NULL )
   , m_selectionColor( 178, 107, 218 )
{
   memset( m_scenes, NULL, sizeof( Model* ) * FRS_ScenesCount );
   memset( m_renderTargets, NULL, sizeof( RenderTarget* ) * FRT_TargetsCount );

   m_cameraVolume = new RPVolFrustum();
   m_spaceVolume = new RPVolAxisAlignedBox(); m_spaceVolume->m_volume.set( Vector( -FLT_MAX, -FLT_MAX, -FLT_MAX ), Vector( FLT_MAX, FLT_MAX, FLT_MAX ) );

   // create the fixed lights for the debug data - the light can shine in any direction, as long as its there ( the shaders ignore it either way )
   m_fixedDebugLights.push_back( new DirectionalLight() );
}

///////////////////////////////////////////////////////////////////////////////

DeferredRenderingMechanism::~DeferredRenderingMechanism()
{
   const uint count = m_fixedDebugLights.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_fixedDebugLights[i];
   }

   delete m_spaceVolume;
   m_spaceVolume = NULL;

   delete m_cameraVolume;
   m_cameraVolume = NULL;

   // release the scenes
   for ( uint i = 0; i < FRS_ScenesCount; ++i )
   {
      if ( m_scenes[i] )
      {
         m_scenes[i]->removeReference();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::initialize( Renderer& renderer )
{
   // create the scene g-buffer
   m_gBuffer = new RenderTarget2DDynamic( &renderer, 1.0f, 1.0f, TU_COLOR, false, GBuf_Size );

   // create the render targets
   for ( int i = ( int ) FRT_Scene; i <= ( int ) FRT_TargetsCount; ++i )
   {
      m_renderTargets[i] = new RenderTarget2DDynamic( &renderer, 1.0f, 1.0f, TU_COLOR, false );
   }

   // calculate scene bounds
   AxisAlignedBox sceneBounds( Vector( -10000.0f, -10000.0f, -10000.0f ), Vector( 10000.0f, 10000.0f, 10000.0f ) );
   {
      if ( m_scenes[FRS_Main] )
      {
         RendererUtils::calculateSceneBounds( *m_scenes[FRS_Main], sceneBounds );
      }
      
      if ( m_scenes[FRS_Debug] )
      {
         AxisAlignedBox bounds;
         RendererUtils::calculateSceneBounds( *m_scenes[FRS_Debug], bounds );
         sceneBounds.add( bounds, sceneBounds );
      }
   }

   // setup the collectors of renderables
   {
      m_customComponentsView = new CustomComponentsView();
      m_lightsView = new LightsView( sceneBounds );
      m_mainGeometryView = new GeometryView( sceneBounds );
      m_selectedGeometryView = new GeometryView( sceneBounds );
      m_gizmoGeometryView = new GeometryView( AxisAlignedBox( Vector( -2000.0f, -2000.0f, -2000.0f ), Vector( 2000.0f, 2000.0f, 2000.0f ) ) );

      // The main geometry view takes its data from the main scene and the debug scene.
      // We have a separate view for gizmo, since we're rendering it in a different pass
      if ( m_scenes[FRS_Main] )
      {
         m_scenes[FRS_Main]->attachListener( m_lightsView );
         m_scenes[FRS_Main]->attachListener( m_mainGeometryView );
         m_scenes[FRS_Main]->attachListener( m_customComponentsView );
      }

      if ( m_scenes[FRS_Debug] )
      {
         m_scenes[FRS_Debug]->attachListener( m_mainGeometryView );
      }

      if ( m_scenes[FRS_Gizmo] )
      {
         m_scenes[FRS_Gizmo]->attachListener( m_gizmoGeometryView );
      }

      if ( m_selectedEntities )
      {
         m_selectedEntities->attachListener( m_selectedGeometryView );
      }
   }

   // setup renderers
   {
      m_mainSceneRenderer = new SceneRenderer( &m_mainGeometryView->m_visibleGeometry->m_collection );

      m_gizmoSceneRenderer = new SceneRenderer( &m_gizmoGeometryView->m_visibleGeometry->m_collection );

      m_selectionRenderer = new SceneRenderer( &m_selectedGeometryView->m_visibleGeometry->m_collection );

      m_selectionResultsBlender = new TexturesBlender();

      m_gizmoBlender = new TexturesBlender();

      m_skyboxRenderer = new SkyboxRenderer();

      m_ambientOcclusionPass = new AmbientOcclusionPass( m_renderTargets[FRT_Intermediate] );

      m_lightsRenderer = new LightsRenderer( &m_lightsView->m_visibleLights->m_collection, m_mainGeometryView->m_geometryStorage );

      m_screenSpaceReflectionsRenderer = new ScreenspaceReflectionsRenderer();
   }

   // the pipeline is initialized
   m_isInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::deinitialize( Renderer& renderer )
{
   // the pipeline is deinitialized
   m_isInitialized = false;

   // destroy the collectors of renderables
   {
      if ( m_scenes[FRS_Main] )
      {
         m_scenes[FRS_Main]->detachListener( m_lightsView );
         m_scenes[FRS_Main]->detachListener( m_mainGeometryView );
         m_scenes[FRS_Main]->detachListener( m_customComponentsView );
      }

      if ( m_scenes[FRS_Debug] )
      {
         m_scenes[FRS_Debug]->detachListener( m_mainGeometryView );
      }

      if ( m_scenes[FRS_Gizmo] )
      {
         m_scenes[FRS_Gizmo]->detachListener( m_gizmoGeometryView );
      }

      if ( m_selectedEntities )
      {
         m_selectedEntities->detachListener( m_selectedGeometryView );
      }

      delete m_customComponentsView;
      m_customComponentsView = NULL;

      delete m_lightsView;
      m_lightsView = NULL;

      delete m_mainGeometryView;
      m_mainGeometryView = NULL;

      delete m_gizmoGeometryView;
      m_gizmoGeometryView = NULL;

      delete m_selectedGeometryView;
      m_selectedGeometryView = NULL;
   }

   // destroy render targets
   for ( uint i = 0; i < FRT_TargetsCount; ++i )
   {
      if ( m_renderTargets[i] )
      {
         m_renderTargets[i]->removeReference();
         m_renderTargets[i] = NULL;
      }
   }

   if ( m_gBuffer )
   {
      m_gBuffer->removeReference();
      m_gBuffer = NULL;
   }

   // destroy renderers
   {
      delete m_mainSceneRenderer;
      m_mainSceneRenderer = NULL;

      delete m_gizmoSceneRenderer;
      m_gizmoSceneRenderer = NULL;

      delete m_selectionRenderer;
      m_selectionRenderer = NULL;

      delete m_skyboxRenderer;
      m_skyboxRenderer = NULL;

      delete m_ambientOcclusionPass;
      m_ambientOcclusionPass = NULL;
   
      delete m_selectionResultsBlender;
      m_selectionResultsBlender = NULL;

      delete m_gizmoBlender;
      m_gizmoBlender = NULL;

      delete m_lightsRenderer;
      m_lightsRenderer = NULL;

      delete m_screenSpaceReflectionsRenderer;
      m_screenSpaceReflectionsRenderer = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::assignScene( ForwardRenderingSceneId sceneContentsId, Model* newScene )
{
   Model* oldScene = m_scenes[sceneContentsId];
   if ( oldScene )
   {
      // detach listeners if the pipeline's already initialized
      if ( m_isInitialized )
      {
         switch ( sceneContentsId )
         {
         case FRS_Main:
         {
            oldScene->detachListener( m_lightsView );
            oldScene->detachListener( m_mainGeometryView );
            oldScene->detachListener( m_customComponentsView );
            break;
         }

         case FRS_Debug:
         {
            oldScene->detachListener( m_mainGeometryView );
            break;
         }

         case FRS_Gizmo:
         {
            oldScene->detachListener( m_gizmoGeometryView );
            break;
         }
         }
      }

      oldScene->removeReference();
   }

   m_scenes[( uint ) sceneContentsId] = newScene;

   if ( newScene )
   {
      newScene->addReference();

      // attach listeners if the pipeline's already initialized
      if ( m_isInitialized )
      {
         if ( m_isInitialized )
         {
            switch ( sceneContentsId )
            {
            case FRS_Main:
            {
               newScene->attachListener( m_lightsView );
               newScene->attachListener( m_mainGeometryView );
               newScene->attachListener( m_customComponentsView );
               break;
            }

            case FRS_Debug:
            {
               newScene->attachListener( m_mainGeometryView );
               break;
            }

            case FRS_Gizmo:
            {
               newScene->attachListener( m_gizmoGeometryView );
               break;
            }
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::assignSelectionGroup( EntitiesGroup* group )
{
   if ( m_isInitialized && m_selectedEntities )
   {
      m_selectedEntities->detachListener( m_selectedGeometryView );
   }

   m_selectedEntities = group;

   if ( m_isInitialized && m_selectedEntities )
   {
      m_selectedEntities->attachListener( m_selectedGeometryView );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::render( Renderer& renderer )
{
   // collect the renderables
   {
      Camera& camera = renderer.getActiveCamera();
      camera.calculateFrustum( m_cameraVolume->m_volume );

      m_lightsView->queryLights( *m_cameraVolume );
      m_mainGeometryView->queryGeometry( *m_cameraVolume );
      m_selectedGeometryView->queryGeometry( *m_cameraVolume );
      m_customComponentsView->query( *m_cameraVolume );
      
      // gizmo should always be visible
      m_gizmoGeometryView->queryGeometry( *m_spaceVolume );
   }

   // refresh renderer settings
   Color globalAmbientLight( 0, 0, 0 );
   {
      LocationRenderSettings* locationSettings = m_customComponentsView->m_locationSettings;
      if ( locationSettings )
      {
         // location settings available
         m_skyboxRenderer->setTexture( locationSettings->getSkyboxTexture() );

         AmbientOcclusionSettings& aoSettings = m_ambientOcclusionPass->accessSettings();
         aoSettings.m_power = locationSettings->m_aoPower;
         aoSettings.m_radius = locationSettings->m_aoRadius;

         globalAmbientLight = locationSettings->getBaseAmbientLight();
      }
      else
      {
         // use the default location settings
         m_skyboxRenderer->setTexture(NULL);

         AmbientOcclusionSettings& aoSettings = m_ambientOcclusionPass->accessSettings();
         aoSettings.m_power = 1.0f;
         aoSettings.m_radius = 0.5f;;
      }
   }

   MemoryAllocator* rcComm = renderer.rtComm();

   // render the pipeline
   new ( rcComm ) RCBeginScene();

   // clear the render targets
   new ( rcComm ) RCClearRenderTarget( m_gBuffer );
   for ( uint i = 0; i < FRT_TargetsCount; ++i )
   {
      new ( rcComm ) RCClearRenderTarget( m_renderTargets[i] );
   }

   // g-buffer pass
   m_mainSceneRenderer->renderColor( renderer, m_gBuffer );
   
   switch ( m_selectedRenderPass )
   {
      case RPass_All:
      {
         m_ambientOcclusionPass->render( renderer, m_gBuffer, globalAmbientLight, m_renderTargets[FRT_Scene] );
         m_lightsRenderer->render( renderer, m_gBuffer, m_renderTargets[FRT_Scene] );
         m_skyboxRenderer->render( renderer, m_gBuffer, m_renderTargets[FRT_Scene] );
         debugPass( renderer, m_renderTargets[FRT_Scene], m_renderTargets[FRT_Final] );
         selectionQueryPass( renderer, m_renderTargets[FRT_SelectionQuery] );
         break;
      }

      case RPass_AmbientOcclusion:
      {
         m_ambientOcclusionPass->render( renderer, m_gBuffer, Color( 255, 255, 255 ), m_renderTargets[FRT_Final] );

         break;
      }

      case RPass_SelectionQuery:
      {
         selectionQueryPass( renderer, m_renderTargets[FRT_Final] );
         break;
      }

      case RPass_ScreenspaceReflections:
      {
         m_screenSpaceReflectionsRenderer->render( renderer, m_gBuffer, m_renderTargets[FRT_Final] );
         break;
      }
   }
   
   new ( rcComm ) RCEndScene();
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::debugPass( Renderer& renderer, RenderTarget2D* sceneTarget, RenderTarget2D* outRenderTarget )
{
   // gizmo pass
   m_gizmoSceneRenderer->renderColor( renderer, m_renderTargets[FRT_Gizmo] );

   // selection
   m_selectionRenderer->renderColorOverride( renderer, m_selectionColor, m_renderTargets[FRT_Selection] );
   
   m_selectionResultsBlender->blendByColor( renderer, sceneTarget, m_renderTargets[FRT_Selection], m_renderTargets[FRT_Intermediate] );
   m_gizmoBlender->blendByAlpha( renderer, m_renderTargets[FRT_Intermediate], m_renderTargets[FRT_Gizmo], outRenderTarget );
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::selectionQueryPass( Renderer& renderer, RenderTarget2D* outRenderTarget )
{
   // base scene pass
   m_mainSceneRenderer->renderObjectPointers( renderer, outRenderTarget );

   // gizmo pass
   SAVE_RENDER_STATE( renderer );
   CHANGE_RENDER_STATE( renderer, RSSetDepthTest( false, false ) );
   m_gizmoSceneRenderer->renderObjectPointers( renderer, outRenderTarget );
   RESTORE_RENDER_STATE( renderer );

   // process rendering queries
   RoundBuffer* queriesQueue = renderer.accessQueriesQueue();
   while ( !queriesQueue->empty() )
   {
      RendererQuery* query = queriesQueue->front< RendererQuery >();
      query->execute( renderer, outRenderTarget );
      delete query;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DeferredRenderingMechanism::defineFragmentShaderOutput( MNOutputStruct* node )
{
   node->removeAllInputs();
   node->removeAllOutputs();

   std::vector< GBNodeInput< MaterialNode >* > inputs;
   inputs.push_back( new MSColorInput( "gBuf_Albedo" ) );
   inputs.push_back( new MSColorInput( "gBuf_Specular" ) );
   inputs.push_back( new MSColorInput( "gBuf_BRDF" ) );
   inputs.push_back( new MSColorInput( "gBuf_Normals" ) );

   node->defineInputs( inputs );
}

///////////////////////////////////////////////////////////////////////////////
