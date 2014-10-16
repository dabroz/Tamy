#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"



///////////////////////////////////////////////////////////////////////////////

const GLenum glComparisonFuncs[] = { GL_NEVER, GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };

const uint glFillMode[] = { GL_POINT, GL_POINT, GL_POINT, GL_LINE, GL_FILL };

const uint glBlendingFunc[] =
{
   GL_ZERO,
   GL_ZERO,
   GL_ONE,
   GL_SRC_COLOR,
   GL_ONE_MINUS_SRC_COLOR,
   GL_SRC_ALPHA,
   GL_ONE_MINUS_SRC_ALPHA,
   GL_DST_ALPHA,
   GL_ONE_MINUS_DST_ALPHA,
   GL_DST_COLOR,
   GL_ONE_MINUS_DST_COLOR,
   GL_SRC_ALPHA_SATURATE,
};

///////////////////////////////////////////////////////////////////////////////

const char* shaderCompilationHeaders[] =
{
   "Vertex shader",
   "Tessellation control shader",
   "Tessellation evaluation shader",
   "Geometry shader",
   "Fragment shader",
};

///////////////////////////////////////////////////////////////////////////////

const GLenum glShaderType[] = {
   GL_VERTEX_SHADER,
   GL_TESS_CONTROL_SHADER,
   GL_TESS_EVALUATION_SHADER,
   GL_GEOMETRY_SHADER,
   GL_FRAGMENT_SHADER };



///////////////////////////////////////////////////////////////////////////////

GLRScopedContextActivator::GLRScopedContextActivator( GLRenderer& renderer )
   : m_renderer( renderer )
{
   m_prevContext = wglGetCurrentContext();
   if ( m_prevContext != m_renderer.m_hrc )
   {
      wglMakeCurrent( m_renderer.m_hdc, m_renderer.m_hrc );
   }
}

///////////////////////////////////////////////////////////////////////////////

GLRScopedContextActivator::~GLRScopedContextActivator()
{
   if ( m_prevContext != m_renderer.m_hrc )
   {
      wglMakeCurrent( m_renderer.m_hdc, m_prevContext );
   }
}

///////////////////////////////////////////////////////////////////////////////
