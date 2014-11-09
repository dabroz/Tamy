#include "core-Renderer\InternalRenderCommands.h"
#include "core-Renderer\Renderer.h"
#include "gl-Renderer\GLRenderer.h"

// OpenGL related headers
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"


///////////////////////////////////////////////////////////////////////////////

void RCInitializeRenderer::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   glRenderer->createDisplay();
}

///////////////////////////////////////////////////////////////////////////////
