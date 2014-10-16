/// @file   core-Renderer\RenderTargetCube.h
/// @brief  a cubical render target
#pragma once

#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\RenderCommand.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderResource;

///////////////////////////////////////////////////////////////////////////////

/**
* A special kind of texture which a renderer uses as an output.
*/
class RenderTargetCube : public ShaderTexture
{
   DECLARE_ALLOCATOR( RenderTargetCube, AM_DEFAULT );
   DECLARE_CLASS();

public:
   RenderResource*               m_renderResource;

private:
   uint                          m_edgeLength;
   TextureUsage                  m_usage;

public:
   /**
    * Constructor.
    *
    * @param edgeLength             length of the cube texture edge
    * @param usage                  render target usage
    */
   RenderTargetCube( uint edgeLength = 1024, TextureUsage usage = TU_COLOR );
   ~RenderTargetCube();

   /**
    * Sets the new length of the texture's edge.
    */
   void resize( uint edgeLength );

   /**
   * Returns the render target usage.
   */
   inline TextureUsage getUsage() const {
      return m_usage;
   }

   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   inline unsigned int getWidth() const override {
      return m_edgeLength;
   }
   inline unsigned int getHeight() const override {
      return m_edgeLength;
   }
   inline ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override {
      return new ( allocator ) ShaderParamRenderTargetCube( paramName.getId(), this, samplerSettings );
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that sets a render target for rendering.
 */
class RCActivateRenderTargetCube : public RenderCommand
{
   DECLARE_ALLOCATOR( RCActivateRenderTargetCube, AM_DEFAULT );

private:
   const RenderTargetCube*          m_renderTarget;
   byte                             m_sideIdx;

public:
   /**
    * Constructor.
    *
    * @param renderTarget
    * @param sideIdx
    */
   RCActivateRenderTargetCube( const RenderTargetCube* renderTarget, byte sideIdx );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that unbinds a render target, disabling any further rendering to it.
 */
class RCDeactivateRenderTargetCube : public RenderCommand
{
   DECLARE_ALLOCATOR( RCDeactivateRenderTargetCube, AM_ALIGNED_16 );

public:
   /**
    * Constructor.
    */
   RCDeactivateRenderTargetCube();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Command that clears the specified render target.
*/
class RCClearRenderTargetCube : public RenderCommand
{
   DECLARE_ALLOCATOR( RCClearRenderTargetCube, AM_ALIGNED_16 );

private:
   const RenderTargetCube*             m_renderTarget;
   Color                               m_bgColor;

public:
   /**
    * Constructor.
    *
    * @param renderTarget        cleaned render target
    * @param bgColor
    */
   RCClearRenderTargetCube( const RenderTargetCube* renderTarget, const Color& bgColor = Color( 0, 0, 0, 0 ) );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
