#include "core-Renderer\ShaderCompiler.h"
#include "core-Renderer\ShaderConstantDesc.h"
#include "core-Renderer\Shader.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRShaderCompilerTool.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

bool ShaderCompiler::compileShader( const Shader& shader, HWND hWnd )
{
   // create a compilation context
   HDC hdc = GetDC( hWnd );
   HGLRC context = GLRShaderCompilerTool::createCompilationContext( hWnd, hdc, m_errorMsg );
   if ( context == NULL )
   {
      return false;
   }

   wglMakeCurrent( hdc, context );

   // compile shaders
   GLuint programId = 0;
   bool result = GLRShaderCompilerTool::compileShader( shader, programId, m_errorMsg );

   if ( result )
   {
      glDeleteProgram( programId );
   }

   // delete the context
   wglMakeCurrent( hdc, 0 );
   wglDeleteContext( context );
   ReleaseDC( hWnd, hdc );

   return result;
}

///////////////////////////////////////////////////////////////////////////////
