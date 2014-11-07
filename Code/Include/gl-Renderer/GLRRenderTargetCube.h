/// @file   gl-Renderer\GLRRenderTargetCube.h
/// @brief  an OpenGL implementation of a cube render target
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderTargetCube;

///////////////////////////////////////////////////////////////////////////////

class GLRRenderTargetCube
{
   DECLARE_ALLOCATOR( GLRRenderTargetCube, AM_DEFAULT );

private:
   Renderer&                     m_renderer;
   const RenderTargetCube*       m_renderTarget;

   uint                          m_frameBufferObject;
   uint                          m_renderTexture;
   uint                          m_depthTexture;

public:
   /**
    * Constructor.
    */
   GLRRenderTargetCube( Renderer& renderer, const RenderTargetCube* renderTarget );
   ~GLRRenderTargetCube();

   /**
    * Refreshes the resource to reflect the changes in the corresponding engine resource.
    */
   void refresh();

   /**
    * Binds this render target for clearing the color.
    */
   void bindForClear( byte sideIdx );

   /**
    * Binds this render target to the active rendering device for drawing.
    */
   void bindForWrite( byte sideIdx );

   /**
    * Returns the ID of the texture the render target renders to.
    *
    * @param idx
    */
   inline uint getTexture() const {
      return m_renderTexture;
   }

   /**
    * Returns the ID of the depth buffer used with this render target.
    */
   inline uint getDepthBuffer() const {
      return m_depthTexture;
   }

     // -------------------------------------------------------------------------
   // IRenderResourceStorage interface
   // -------------------------------------------------------------------------
   const RenderTargetCube* getEngineResource() {
      return m_renderTarget;
   }

private:
   void initialize();
   void deinitialize();
};

///////////////////////////////////////////////////////////////////////////////
