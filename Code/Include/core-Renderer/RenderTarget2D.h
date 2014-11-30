/// @file   core-Renderer\RenderTarget2D.h
/// @brief  a 2D render target
#pragma once

#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\RenderCommand.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class RenderResource;
class PixelColorQuery;

///////////////////////////////////////////////////////////////////////////////

class RenderTarget2D : public ShaderTexture
{
   DECLARE_ALLOCATOR( RenderTarget2D, AM_DEFAULT );
   DECLARE_CLASS();

public:
   RenderResource*               m_renderResource;

protected:
   TextureUsage                  m_usage;
   bool                          m_isReadable;
   byte                          m_renderTexturesCount;

public:
   /**
    * Constructor.
    *
    * @param usage                  render target usage
    * @param isReadable             tells if we can perform reading operations on the texture
    * @param renderTexturesCount    how many render textures should be created
    */
   RenderTarget2D( TextureUsage usage = TU_COLOR, bool isReadable = false, byte renderTexturesCount = 1 );
   virtual ~RenderTarget2D();

   /**
    * Returns the render target usage.
    */
   inline TextureUsage getUsage() const {
      return m_usage;
   }

   /**
    * Tells if the texture supports reading operation.
    */
   inline bool isReadable() const {
      return m_isReadable;
   }

   /**
    * Returns a number of render textures this g-buffer comprises.
    */
   inline byte getRenderTexturesCount() const {
      return m_renderTexturesCount;
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
* A special kind of texture which a renderer uses as an output.
*/
class RenderTarget2DStatic : public RenderTarget2D
{
   DECLARE_ALLOCATOR( RenderTarget2DStatic, AM_DEFAULT );
   DECLARE_CLASS();

private:
   uint                          m_width;
   uint                          m_height;

public:
   /**
    * Constructor.
    *
    * @param width
    * @param height
    * @param usage                  render target usage
    * @param isReadable             tells if we can perform reading operations on the texture
    * @param renderTexturesCount    how many render textures should be created
    */
   RenderTarget2DStatic( uint width, uint height, TextureUsage usage = TU_COLOR, bool isReadable = false, byte renderTexturesCount = 1 );
   ~RenderTarget2DStatic();

   /**
    * Sets the new size of the render target.
    */
   void resize( uint width, uint height );
 
   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   unsigned int getWidth() const override {
      return m_width;
   }
   unsigned int getHeight() const override {
      return m_height;
   }
   ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override {
      return new ( allocator ) ShaderParamRenderTarget2D( paramName.getId(), this, samplerSettings, renderTextureIdx );
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
* A special kind of texture which a renderer uses as an output.
*/
class RenderTarget2DDynamic : public RenderTarget2D
{
   DECLARE_ALLOCATOR( RenderTarget2DDynamic, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                         m_widthScale;
   float                         m_heightScale;

   Renderer*                     m_renderer;

public:
   /**
    * Constructor.
    *
    * @param renderer
    * @param widthScale
    * @param heightScale
    * @param usage                  render target usage
    * @param isReadable             tells if we can perform reading operations on the texture
    * @param renderTexturesCount    how many render textures should be created
    */
   RenderTarget2DDynamic( Renderer* renderer, float widthScale = 1.0f, float heightScale = 1.0, TextureUsage usage = TU_COLOR, bool isReadable = false, byte renderTexturesCount = 1 );
   ~RenderTarget2DDynamic();

   /**
    * Sets the new size of the render target.
    */
   void resize( float widthScale, float heightScale );

   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   unsigned int getWidth() const override;
   unsigned int getHeight() const override;
   ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override {
      return new ( allocator ) ShaderParamRenderTarget2D( paramName.getId(), this, samplerSettings, renderTextureIdx );
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that sets a render target for rendering.
 */
class RCActivateRenderTarget : public RenderCommand
{
   DECLARE_ALLOCATOR( RCActivateRenderTarget, AM_DEFAULT );

private:
   const RenderTarget2D*           m_renderTarget;

public:
   /**
    * Constructor.
    *
    * @param renderTarget     ptr to the render target ( NULL value also accepted )
    */
   RCActivateRenderTarget( const RenderTarget2D* renderTarget );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that unbinds a render target, disabling any further rendering to it.
 */
class RCDeactivateRenderTarget : public RenderCommand
{
   DECLARE_ALLOCATOR( RCDeactivateRenderTarget, AM_ALIGNED_16 );

public:
   /**
    * Constructor.
    */
   RCDeactivateRenderTarget();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that clears the specified render target.
 */
class RCClearRenderTarget : public RenderCommand
{
   DECLARE_ALLOCATOR( RCClearRenderTarget, AM_ALIGNED_16 );

public:
   const RenderTarget2D*            m_renderTarget;
   Color                            m_bgColor;
   float                            m_depth;

public:
   /**
    * Constructor.
    *
    * @param renderTarget        cleaned render target
    */
   RCClearRenderTarget( const RenderTarget2D* renderTarget );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that allows you to query for the specified pixel color.
 */
class RCGetPixel : public RenderCommand
{
   DECLARE_ALLOCATOR( RCGetPixel, AM_DEFAULT );

private:
   const RenderTarget2D*            m_renderTarget;
   PixelColorQuery*                 m_query;

public:
   /**
    * Constructor.
    *
    * @param renderTarget        queried render target
    * @param query
    */
   RCGetPixel( const RenderTarget2D* renderTarget, PixelColorQuery* query );
   ~RCGetPixel();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
