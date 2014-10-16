#include "gl-Renderer\GLRShaderCompilerTool.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "core-Renderer\Shader.h"
#include "gl-Renderer\GLRShader.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

HGLRC GLRShaderCompilerTool::createCompilationContext( HWND hWnd, HDC hdc, std::string& outErrorMsg )
{
   // Create a new PIXELFORMATDESCRIPTOR (PFD)
   PIXELFORMATDESCRIPTOR pfd;
   {
      memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
      pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR ); // Set the size of the PFD to the size of the class  

      pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
      pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
      pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
      pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
      pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD  
   }

   int nPixelFormat = ChoosePixelFormat( hdc, &pfd );
   if ( nPixelFormat == 0 )
   {
      outErrorMsg = "Failed to select that pixel format";
      return NULL;
   }

   BOOL bResult = SetPixelFormat( hdc, nPixelFormat, &pfd );
   if ( !bResult )
   {
      outErrorMsg = "Failed to set the specified pixel format";
      return NULL;
   }

   // We need to start by create an OpenGL 2.1 context for our device context, and later on upgrading it to the 3.x standard
   HGLRC tempOpenGLContext = wglCreateContext( hdc );

   // Make the OpenGL 2.1 context current and active  
   wglMakeCurrent( hdc, tempOpenGLContext );

   // enable the extensions manager
   GLenum error = glewInit();
   if ( error != GLEW_OK )
   {
      outErrorMsg = "Failed to initialize the extensions manager";
      return NULL;
   }

   // now we can finally switch to OpenGL 3.x
   int attributes[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3  
      WGL_CONTEXT_MINOR_VERSION_ARB, 3, // Set the MINOR version of OpenGL to 3 
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible  
      0
   };

   // If the OpenGL 3.x context creation extension is available  
   HGLRC hrc = NULL;
   if ( wglewIsSupported( "WGL_ARB_create_context" ) == 1 )
   {
      // Create and OpenGL 3.x context based on the given attributes  
      hrc = wglCreateContextAttribsARB( hdc, NULL, attributes );

      // Remove the temporary context from being active  
      wglMakeCurrent( NULL, NULL );

      // Delete the temporary OpenGL 2.1 context  
      wglDeleteContext( tempOpenGLContext );

      // Make our OpenGL 3.0 context current  
      wglMakeCurrent( hdc, hrc );

      LOG( "OpenGL 3.x context initialized" );
   }
   else
   {
      // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context 
      LOG( "OpenGL 3.x initialization failed, we're sticking to OpenGL 2.1" );
      hrc = tempOpenGLContext;
   }

   // let's check which version did we manage to initialize
   {
      int glVersion[2] = { -1, -1 }; // Set some default values for the version  
      glGetIntegerv( GL_MAJOR_VERSION, &glVersion[0] ); // Get back the OpenGL MAJOR version we are using  
      glGetIntegerv( GL_MINOR_VERSION, &glVersion[1] ); // Get back the OpenGL MAJOR version we are using  

      LOG( "Using OpenGL %d.%d", glVersion[0], glVersion[1] );
   }

   return hrc;
}

///////////////////////////////////////////////////////////////////////////////

bool GLRShaderCompilerTool::compileShader( const Shader& shader, uint& outProgramID, std::string& outErrorStr )
{
   outProgramID = (uint)-1;
   uint arrShaderIds[LAST_SHADER_ROUTINE];
   memset( arrShaderIds, ( uint ) -1, sizeof( uint ) * LAST_SHADER_ROUTINE );

   // load the code of particular shaders
   std::string shaderCode, errStr;
   bool compilationResult = true;
   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      ShaderRoutine type = ( ShaderRoutine ) i;
      shader.getScriptPreprocessed( type, shaderCode );
      if ( shaderCode.empty() )
      {
         continue;
      }

      arrShaderIds[type] = glCreateShader( glShaderType[type] );

      const char* shaderCodeStr = shaderCode.c_str();
      glShaderSource( arrShaderIds[type], 1, &shaderCodeStr, 0 );
      GLR_LOG_ERRORS();
      glCompileShader( arrShaderIds[type] );
      GLR_LOG_ERRORS();

      if ( validateShader( arrShaderIds[type], errStr ) == false )
      {
         char tmpStr[4096];
         sprintf_s( tmpStr, "--- %s %s compilation errors ---\n\n%s\n\n", shaderCompilationHeaders[type], shader.getScriptPath().c_str(), errStr.c_str() );
         outErrorStr += tmpStr;

         arrShaderIds[type] = ( uint ) -1;
         compilationResult = false;
         break;
      }
   }

   if ( !compilationResult )
   {
      // detach sub-shaders from the shader program and delete them
      for ( int i = 0; i < LAST_SHADER_ROUTINE; ++i )
      {
         if ( arrShaderIds[i] != ( uint ) -1 )
         {
            glDeleteShader( arrShaderIds[i] );
            arrShaderIds[i] = ( uint ) -1;
         }
      }
      return false;
   }


   
   // compile them into a single shader program
   outProgramID = glCreateProgram();
   for ( int i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      if ( arrShaderIds[i] != ( uint ) -1 )
      {
         glAttachShader( outProgramID, arrShaderIds[i] );
         GLR_LOG_ERRORS();
      }
   }

   // link the shader program
   glLinkProgram( outProgramID );
   GLR_LOG_ERRORS();

   // check if the program compiled and linked ok
   if ( validateProgram( outProgramID, errStr ) == false )
   {
      outProgramID = ( uint ) -1;

      char tmpStr[1024];
      sprintf_s( tmpStr, "--- Shader linking errors ---\n\n%s\n\n", errStr.c_str() );
      outErrorStr += tmpStr;

      // delete the program
      glDeleteProgram( outProgramID );
      outProgramID = ( uint ) -1;
   }

   // detach sub-shaders from the shader program and delete them
   for ( int i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      if ( arrShaderIds[i] != ( uint ) -1 )
      {
         glDeleteShader( arrShaderIds[i] );
         arrShaderIds[i] = ( uint ) -1;
      }
   }

   return outProgramID != (uint)-1;
}

///////////////////////////////////////////////////////////////////////////////

bool GLRShaderCompilerTool::validateShader( uint shaderId, std::string& outErrors )
{
   const unsigned int BUFFER_SIZE = 1024;
   char buffer[BUFFER_SIZE];
   memset( buffer, 0, BUFFER_SIZE );
   GLsizei length = 0;

   // Ask OpenGL to give us the log associated with the shader
   glGetShaderInfoLog( shaderId, BUFFER_SIZE, &length, buffer );
   if ( length > 0 )
   {
      outErrors = buffer;
      return false;
   }
   else
   {
      return true;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool GLRShaderCompilerTool::validateProgram( uint programId, std::string& outErrors )
{
   const unsigned int BUFFER_SIZE = 1024;
   char buffer[BUFFER_SIZE];
   memset( buffer, 0, BUFFER_SIZE );
   GLsizei length = 0;

   // Ask OpenGL to give us the log associated with the program
   glGetProgramInfoLog( programId, BUFFER_SIZE, &length, buffer );

   if ( length > 0 )
   {
      outErrors = buffer;
      return false;
   }

   // Get OpenGL to try validating the program
   glValidateProgram( programId );

   // Find out if the shader program validated correctly
   GLint status;
   glGetProgramiv( programId, GL_VALIDATE_STATUS, &status );
   if ( status == GL_FALSE )
   {
      outErrors = "Error validating the shader program";
      return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
