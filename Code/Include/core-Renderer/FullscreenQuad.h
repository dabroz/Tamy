/// @file   core-Renderer/FullscreenQuad.h
/// @brief  full screen quad geometry
#pragma once

#include "core-Renderer/RenderCommand.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Graphics library dependent implementation of a full-screen quad.
 */
class RCFullscreenQuad : public RenderCommand
{
   DECLARE_ALLOCATOR( RCFullscreenQuad, AM_DEFAULT );

private:
   uint           m_width;
   uint           m_height;

public:
   RCFullscreenQuad( uint width, uint height );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
