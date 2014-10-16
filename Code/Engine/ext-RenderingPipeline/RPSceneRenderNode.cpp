#include "core-Renderer\RPSceneRenderNode.h"
#include "core-Renderer\RenderingPipelineMechanism.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderTarget.h"
#include "core-Renderer\RenderingPipelineSockets.h"
#include "core-Renderer\DeferredLightingRenderData.h"
#include "core-Renderer\DeferredLightingPipelineUtility.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPSceneRenderNode );
   PARENT( RenderingPipelineNode );
   PROPERTY_EDIT( "Clear depth buffer", bool, m_clearDepthBuffer );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::RPSceneRenderNode()
   : m_clearDepthBuffer( true )
   , m_normalsRT( NULL )
   , m_depthRT( NULL )
   , m_albedoRT( NULL )
   , m_resultRT( NULL )
{   
   if ( !IS_BEING_SERIALIZED() )
   {
      defineInput( new RPVoidInput( "Input" ) );

      m_normalsRT = new RPTextureOutput( "Normals" );
      m_depthRT = new RPTextureOutput( "Depth" );
      m_albedoRT = new RPTextureOutput( "Albedo" );
      m_resultRT = new RPTextureOutput( "Result" );

      std::vector< GBNodeOutput< RenderingPipelineNode >* > outputs;
      outputs.push_back( m_normalsRT );
      outputs.push_back( m_depthRT );
      outputs.push_back( m_albedoRT );
      outputs.push_back( m_resultRT );
      defineOutputs( outputs );
   }
}
///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::~RPSceneRenderNode()
{
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onObjectLoaded()
{
   RenderingPipelineNode::onObjectLoaded();

   m_normalsRT = static_cast< RPTextureOutput* >( findOutput( "Normals" ) );
   m_depthRT = static_cast< RPTextureOutput* >( findOutput( "Depth" ) );
   m_albedoRT = static_cast< RPTextureOutput* >( findOutput( "Albedo" ) );
   m_resultRT = static_cast< RPTextureOutput* >( findOutput( "Result" ) );
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onCreateLayout( RenderingPipelineMechanism& host ) const
{
   RuntimeDataBuffer& data = host.data();

   // register runtime members
   data.registerVar( m_renderingData );

   Renderer& renderer = host.getRenderer();
   DeferredLightingRenderData* renderData = new DeferredLightingRenderData( renderer );
   const RenderingView* view = host.getSceneRenderingView();
   renderData->m_renderingView = view;

   data[m_renderingData] = renderData;
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onDestroyLayout( RenderingPipelineMechanism& host ) const
{
   RuntimeDataBuffer& data = host.data();

   DeferredLightingRenderData* renderingData = data[m_renderingData];
   data[ m_renderingData ] = NULL;
   delete renderingData;

   data.unregisterVar( m_renderingData );
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onUpdate( RenderingPipelineMechanism& host ) const
{
   RuntimeDataBuffer& data = host.data();

   // render the scene
   DeferredLightingRenderData* renderingData = data[m_renderingData];
   Renderer& renderer = host.getRenderer();
   DeferredLightingPipelineUtility::render( renderer, *renderingData );

   // set the results on the outputs
   m_normalsRT->setValue( data, renderingData->m_sceneRenderData->m_normalsRT );
   m_depthRT->setValue( data, renderingData->m_sceneRenderData->m_depthRT );
   m_albedoRT->setValue( data, renderingData->m_sceneRenderData->m_albedoRT );
   m_resultRT->setValue( data, renderingData->m_finalLightColorTarget );
}

///////////////////////////////////////////////////////////////////////////////
