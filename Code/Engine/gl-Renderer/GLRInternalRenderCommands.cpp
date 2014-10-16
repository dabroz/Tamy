#include "core-Renderer\InternalRenderCommands.h"
#include "core-Renderer\Renderer.h"
#include "gl-Renderer\GLRenderer.h"

// OpenGL related headers
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"


///////////////////////////////////////////////////////////////////////////////

void RCRenderingBegin::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );

   // Bind the render context
   wglMakeCurrent( glRenderer->m_hdc, glRenderer->m_hrc );

   // resize the viewport if needed
   glRenderer->resizeViewport( m_viewportWidth, m_viewportHeight );

   // Set the viewport size to fill the window  
   glViewport( 0, 0, m_viewportWidth, m_viewportHeight );

   // Clear required buffers 
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////
#include "core\Timer.h"

void RCRenderingEnd::endFrame( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );

#ifndef _DEBUG
   // Double buffering is disabled in the debug mode, because it was causing stalls.
   // The explanation and an associated TODO is described in GLRenderer.cpp : 94
   SwapBuffers( glRenderer->m_hdc );
#endif

   // unbind the rendering context
   wglMakeCurrent( glRenderer->m_hdc, NULL );
}

///////////////////////////////////////////////////////////////////////////////

void RCInitializeRenderer::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   glRenderer->createDisplay();
}

///////////////////////////////////////////////////////////////////////////////
