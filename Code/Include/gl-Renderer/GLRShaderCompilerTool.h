/// @file   gl-Renderer\GLRShaderCompilerTool.h
/// @brief  a utility for compiling shaders
#pragma once

#include "core\types.h"
#include <string>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Shader;

///////////////////////////////////////////////////////////////////////////////

/**
 * A utility for compiling shaders
 */
class GLRShaderCompilerTool
{
public:
   /**
    * Creates a temporary shader compilation context.
    *
    * @param hWnd    a handle to a temporary render window
    * @param hdc     a handle to the window's render context
    * @param outErrorMsg
    */
   static HGLRC createCompilationContext( HWND hWnd, HDC hdc, std::string& outErrorMsg );

   /**
    * Compiles a shader.
    *
    * @param shader
    * @param outProgramID
    * @param outErrorStr
    */
   static bool compileShader( const Shader& shader, uint& outProgramID, std::string& outErrorStr );

private:
   /**
   * This method allows to validate shader's code.
   *
   * @param shaderId
   * @param outErrors
   * @return 'true' if the shader compiles OK,, false otherwise
   */
   static bool validateShader( uint shaderId, std::string& outErrors );

   /**
   * This method allows to validate the shader program's code.
   *
   * @param programId
   * @param outErrors
   * @return 'true' if the shader compiles OK,, false otherwise
   */
   static bool validateProgram( uint programId, std::string& outErrors );
};

///////////////////////////////////////////////////////////////////////////////
