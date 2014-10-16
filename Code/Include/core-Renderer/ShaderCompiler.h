/// @file   core-Renderer/ShaderCompiler.h
/// @brief  a tool for compiling shader code for verification purposes
#pragma once

#include <string>
#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core-Renderer\ShaderConstantDesc.h"
#include "core\types.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Shader;

///////////////////////////////////////////////////////////////////////////////

class ShaderCompiler
{
   DECLARE_ALLOCATOR( ShaderCompiler, AM_DEFAULT );

private:
   std::string    m_errorMsg;

public:
   /**
    * Parses shader uniform constants.
    *
    * @param shader
    * @param outConstants
    * @return              'true' if parsing was successful, 'false' otherwise
    */
   bool parseConstants( const Shader& shader, Array< ShaderConstantDesc >& outConstants );

   /**
    * Compiles the specified shader, returning the compilation results and information about
    * the errors that were encountered in the process.
    *
    * The method does not actually affect the shader nor its implementation.
    * It only serves the purpose of compiling the shader to learn if it compiles ok.
    *
    * @param shader
    * @return              'true' if compilation was successful, 'false' otherwise
    */
   bool compileShader( const Shader& shader, HWND hWnd );

   /**
    * Returns the last error message.
    */
   inline const std::string& getLastError() const { return m_errorMsg; }

private:
   bool parseConstants_HLSL( const Shader& shader, Array< ShaderConstantDesc >& outConstants );
   bool parseConstants_GLSL( const Shader& shader, Array< ShaderConstantDesc >& outConstants );
};

///////////////////////////////////////////////////////////////////////////////
