#include "gl-Renderer\GLRErrorCheck.h"
#include "core\Log.h"
#include "core\Assert.h"

#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"


///////////////////////////////////////////////////////////////////////////////

static const char* getErrorDescription()
{
   int errCode = glGetError();
   switch ( errCode )
   {
   case GL_NO_ERROR: return NULL;
   case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
   case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
   case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
   case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
   case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
   case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
   case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void __GLR_LOG_ERRORS( const char* fileName, int lineNo )
{
   const char* errorDesc = getErrorDescription();
   if ( errorDesc != NULL )
   {
      LOG( "GLRError: %s : %d - %s", fileName, lineNo, errorDesc );
   }
}

///////////////////////////////////////////////////////////////////////////////

void __GLR_HALT_ERRORS()
{
   const char* errorDesc = getErrorDescription();
   if ( errorDesc != NULL )
   {
      ASSERT_MSG( false, errorDesc );
   }
}

///////////////////////////////////////////////////////////////////////////////
