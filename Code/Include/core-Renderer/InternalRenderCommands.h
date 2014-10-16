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
 * Opens a rendering batch.
 */
class RCRenderingBegin  : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRenderingBegin, AM_DEFAULT );

private:
   uint     m_viewportWidth;
   uint     m_viewportHeight;

public:
   /**
    * Constructor.
    *
    * @param viewportWidth
    * @param viewportHeight
    */
   RCRenderingBegin( uint viewportWidth, uint viewportHeight );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Closes a rendering batch, flushing all contents to the screen.
 */
class RCRenderingEnd : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRenderingEnd, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

private:
   void endFrame( Renderer& renderer );
};

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
