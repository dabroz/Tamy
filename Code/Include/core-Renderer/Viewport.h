/// @file   core-Renderer/Viewport.h
/// @brief  rendering viewport description
#pragma once

#include "core\types.h"
#include "core-Renderer\RenderCommand.h"


///////////////////////////////////////////////////////////////////////////////

struct Viewport
{
   ulong  m_offsetX;
   ulong  m_offsetY;
   ulong  m_width;
   ulong  m_height;
   float  m_minZ;
   float  m_maxZ;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Sets the rendering viewport.
 *
 * As opposed to the Renderer API, this commands will change the viewport settings temporarily
 * ( next device reset or a Renderer intrusion will change it back to the original settings ),
 * but it's guaranteed to work during a single rendering frame.
 * For that reason it should be used i.e. when you want to render multiple passes to a single texture.
 *
 * But it's obligatory that you reset the viewport back to its proper state once you're done rendering
 * ( using the RCResetViewport command ).
 */
class RCSetViewport : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetViewport, AM_DEFAULT );

private:
   Viewport    m_viewport;

public:
   /**
    * Constructor.
    *
    * @param viewport
    */
   RCSetViewport( const Viewport& viewport )
      : m_viewport( viewport )
   {}

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Resets rendering viewport back to its former state ( one used by the renderer ).
 */
class RCResetViewport : public RenderCommand
{
   DECLARE_ALLOCATOR( RCResetViewport, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
