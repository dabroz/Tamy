/// @file   gl-Renderer\GLRDefinitions.h
/// @brief  this file contains definitions of various data structures 
///         useful for mapping between the Tamy and the OpenGL domains 
#pragma once

#include "core\types.h"

// OpenGL related headers
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"


///////////////////////////////////////////////////////////////////////////////

// #define ENABLE_ERROR_CHECKS         // uncomment this define in order to enable the error checking

///////////////////////////////////////////////////////////////////////////////

// entries in this array correspond to those in the ComparisonFunc enum
extern const GLenum glComparisonFuncs [];

// entries in this array correspond to those in the CullingMode enum
extern const uint glFillMode[];

// entries in this array correspond to those in the BlendFunc enum
extern const uint glBlendingFunc[];

// names of the individual sub-shader stages
extern const char* shaderCompilationHeaders [];

// mapping of the shader stage id enums
extern const GLenum glShaderType [];

///////////////////////////////////////////////////////////////////////////////

/**
 * Structure used to pass Matrix instances as OpenGL parameters
 */
struct GLRMatrix
{
   float    m[16];
};

///////////////////////////////////////////////////////////////////////////////

/**
* Structure used to pass Vector instances as OpenGL parameters
*/
struct GLRVector4
{
   float    m[4];
};

///////////////////////////////////////////////////////////////////////////////

class GLRenderer;

/**
 * This helper structure temporarily activates the specified renderer's OpenGL context.
 * It's useful for rebuilding resources - commands that may be executed outside the beginRendering-endRendering
 * commands chain.
 */
struct GLRScopedContextActivator
{
   GLRenderer& m_renderer;
   HGLRC       m_prevContext;

   GLRScopedContextActivator( GLRenderer& renderer );
   ~GLRScopedContextActivator();
};

///////////////////////////////////////////////////////////////////////////////
