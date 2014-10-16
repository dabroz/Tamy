/// @file   core-Renderer/RPSceneRenderNode.h
/// @brief  a rendering pipeline node responsible for rendering a scene
#pragma once

#include "core-Renderer/RenderingPipelineMechanism.h"
#include "core-Renderer/RenderingPipelineNode.h"
#include "core/RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

struct DeferredLightingRenderData;
class RPTextureOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * A rendering pipeline node responsible for rendering a scene.
 */
class RPSceneRenderNode : public RenderingPipelineNode
{
   DECLARE_ALLOCATOR( RPSceneRenderNode, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   bool                                         m_clearDepthBuffer;

   // outputs ( runtime )
   RPTextureOutput*                             m_normalsRT;
   RPTextureOutput*                             m_depthRT;
   RPTextureOutput*                             m_albedoRT;
   RPTextureOutput*                             m_resultRT;

   // runtime data
   TRuntimeVar< DeferredLightingRenderData* >   m_renderingData;

public:
   /**
    * Constructor.
    */
   RPSceneRenderNode();
   ~RPSceneRenderNode();

   // -------------------------------------------------------------------------
   // RenderingPipelineNode implementation
   // -------------------------------------------------------------------------
   void onCreateLayout( RenderingPipelineMechanism& host ) const;
   void onDestroyLayout( RenderingPipelineMechanism& host ) const;
   void onUpdate( RenderingPipelineMechanism& host ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
