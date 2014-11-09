/// @file   core-Renderer/InternalRenderCommands.h
/// @brief  render commands used internally by the renderer
#pragma once

#include "core\types.h"
#include "core-Renderer\RenderCommand.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;
class RendererInitializer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the rendering device.
 */
class RCInitializeRenderer : public RenderCommand
{
   DECLARE_ALLOCATOR( RCInitializeRenderer, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
