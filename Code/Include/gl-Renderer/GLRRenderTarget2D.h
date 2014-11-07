/// @file   gl-Renderer\GLRRenderTarget.h
/// @brief  OpenGL implementation of a render target
#pragma once

#include "core-Renderer\RenderTarget2D.h"
#include "core\Color.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
struct Point;
class SurfaceQuery;

///////////////////////////////////////////////////////////////////////////////

class GLRRenderTarget2D
{
   DECLARE_ALLOCATOR( GLRRenderTarget2D, AM_DEFAULT );

private:
   Renderer&               m_renderer;
   const RenderTarget2D*   m_renderTarget;

   uint                    m_frameBufferObject;
   Array< uint >           m_renderTexture;
   uint                    m_depthTexture;

   Array< uint >           m_attachments;

public:
   GLRRenderTarget2D( Renderer& renderer, const RenderTarget2D* renderTarget );
   ~GLRRenderTarget2D();

   /**
    * Refreshes the resource to reflect the changes in the corresponding engine resource.
    */
   void refresh();

   /**
    * Binds this render target to the active rendering device for drawing.
    */
   void bindForWrite();

   /**
   * Binds the buffer to the active rendering device for reading.
   */
   void bindForRead();

   /**
    * Returns the ID of the texture the render target renders to.
    *
    * @param idx
    */
   inline uint getTexture( uint idx = 0 ) const {
      return m_renderTexture[idx];
   }

   /**
    * Returns the ID of the depth buffer used with this render target.
    */
   inline uint getDepthBuffer() const {
      return m_depthTexture;
   }

   /**
    * Called when the viewport gets resized.
    */
   void onViewportResized( uint width, uint height );

   // -------------------------------------------------------------------------
   // IRenderResourceStorage interface
   // -------------------------------------------------------------------------
   const RenderTarget2D* getEngineResource() {
      return m_renderTarget;
   }

private:
   void initialize();
   void deinitialize();
};

///////////////////////////////////////////////////////////////////////////////
