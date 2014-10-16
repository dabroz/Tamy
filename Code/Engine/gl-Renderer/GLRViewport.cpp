#include "core-Renderer\Viewport.h"
#include "core-Renderer\Renderer.h"

// OpenGL related headers
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"



///////////////////////////////////////////////////////////////////////////////

void RCSetViewport::execute( Renderer& renderer )
{
   glViewport( m_viewport.m_offsetX, m_viewport.m_offsetY, m_viewport.m_width, m_viewport.m_height );
}

///////////////////////////////////////////////////////////////////////////////

void RCResetViewport::execute( Renderer& renderer )
{
   Viewport defaultViewport;
   renderer.getViewport( defaultViewport );
   glViewport( defaultViewport.m_offsetX, defaultViewport.m_offsetY, defaultViewport.m_width, defaultViewport.m_height );
}

///////////////////////////////////////////////////////////////////////////////
