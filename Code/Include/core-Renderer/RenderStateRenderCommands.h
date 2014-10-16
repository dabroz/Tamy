/// @file   core-Renderer\RenderStateRenderCommands.h
/// @brief  render commands that apply render state changes to the rendering device
#pragma once

#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\RenderingParams.h"


///////////////////////////////////////////////////////////////////////////////

/*
 * CAUTION: These commands SHOULD NOT be issued directly to the rendering device.
 *          Use a corresponding RenderStateCommand and a RenderStateChangeTracker
 *          in order to correctly apply a state change, so that it gets reverted
 *          as soon as that part of rendering is over.
 */

///////////////////////////////////////////////////////////////////////////////

/**
 * Sets the depth buffer test state.
 */
class RCSetDepthTest : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetDepthTest, AM_ALIGNED_16 );

public:
   bool                          m_useZBuffer;
   bool                          m_writeToZBuffer;
   ComparisonFunc                m_depthTestFunc;

public:
   /**
   * Constructor.
   */
   RCSetDepthTest( bool writeToZBuffer = true, bool useZBuffer = true, ComparisonFunc depthTestFunc = CMP_LESS );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Toggles the alpha test.
 */
class RCSetAlphaTest : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetAlphaTest, AM_ALIGNED_16 );

public:
   bool                          m_enable;

public:
   /**
    * Constructor.
    */
   RCSetAlphaTest( bool enable = false );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

};

///////////////////////////////////////////////////////////////////////////////

/**
* Sets the alpha/color blending state.
*/
class RCSetBlending : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetBlending, AM_ALIGNED_16 );

public:
   bool                          m_enabled;
   BlendFunc                     m_blendSourceFunc;
   BlendFunc                     m_blendDestFunc;

public:
   /**
   * Constructor.
   */
   RCSetBlending( bool enabled = false, BlendFunc sourceFunc = BLEND_ONE, BlendFunc destFunc = BLEND_ZERO );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

};

///////////////////////////////////////////////////////////////////////////////

/**
 * Changes the face culling mode.
 */
class RCSetFaceCulling : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetFaceCulling, AM_ALIGNED_16 );

public:
   CullingMode                   m_mode;

public:
   /**
    * Constructor.
    *
    * @param mode
    */
   RCSetFaceCulling( CullingMode mode = CULL_CCW );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

};

///////////////////////////////////////////////////////////////////////////////
