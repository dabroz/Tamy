/// @file   core-Renderer\RenderStateCommand.h
/// @brief  a command that changes the rendering state
#pragma once

#include "core\MemoryRouter.h"
#include "core-Renderer\RenderingParams.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;

///////////////////////////////////////////////////////////////////////////////

class RenderStateCommand
{
   DECLARE_ALLOCATOR( RenderStateCommand, AM_DEFAULT );

public:
   RenderStateCommand*           m_prev;

public:
   /**
    * Constructor.
    */
   RenderStateCommand() : m_prev( NULL ) {}
   virtual ~RenderStateCommand() {}

   /**
    * Commits the state change, creating a rollback point at the same time.
    */
   virtual void commit( Renderer& renderer ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
* Sets the depth buffer test state.
*/
class RSSetDepthTest : public RenderStateCommand
{
   DECLARE_ALLOCATOR( RSSetDepthTest, AM_ALIGNED_16 );

public:
   bool                          m_useZBuffer;
   bool                          m_writeToZBuffer;
   ComparisonFunc                m_depthTestFunc;

public:
   /**
    * Constructor.
    *
    * @param writeToZBuffer
    * @param useZBuffer
    * @param depthTestFunc
    */
   RSSetDepthTest( bool writeToZBuffer = true, bool useZBuffer = true, ComparisonFunc depthTestFunc = CMP_LESS );

   // -------------------------------------------------------------------------
   // RenderStateCommand implementation
   // -------------------------------------------------------------------------
   void commit( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

class RSSetAlphaTest : public RenderStateCommand
{
   DECLARE_ALLOCATOR( RSSetAlphaTest, AM_DEFAULT );

private:
   bool                       m_enable;

public:
   /**
    * Constructor.
    *
    * @param enable
    */
   RSSetAlphaTest( bool enable );

   // -------------------------------------------------------------------------
   // RenderStateCommand implementation
   // -------------------------------------------------------------------------
   void commit( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Sets the alpha/color blending state.
*/
class RSSetBlending : public RenderStateCommand
{
   DECLARE_ALLOCATOR( RSSetBlending, AM_ALIGNED_16 );

public:
   bool                          m_enabled;
   BlendFunc                     m_blendSourceFunc;
   BlendFunc                     m_blendDestFunc;

public:
   /**
   * Constructor.
   */
   RSSetBlending( bool enabled = false, BlendFunc sourceFunc = BLEND_ONE, BlendFunc destFunc = BLEND_ZERO );

   // -------------------------------------------------------------------------
   // RenderStateCommand implementation
   // -------------------------------------------------------------------------
   void commit( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

class RSSetFaceCulling : public RenderStateCommand
{
   DECLARE_ALLOCATOR( RSSetFaceCulling, AM_DEFAULT );

private:
   CullingMode                m_mode;

public:
   /**
   * Constructor.
   *
   * @param mode
   */
   RSSetFaceCulling( CullingMode mode );

   // -------------------------------------------------------------------------
   // RenderStateCommand implementation
   // -------------------------------------------------------------------------
   void commit( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
