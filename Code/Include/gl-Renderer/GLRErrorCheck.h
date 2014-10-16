/// @file   gl-Renderer\GLRErrorCheck.h
/// @brief  a utility method for checking OpenGL errors
#pragma once

#include "gl-Renderer\GLRDefinitions.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A utility method for checking OpenGL errors.
 *
 * Upon an error, a log entry will be made
 */
void __GLR_LOG_ERRORS( const char* fileName, int lineNo );

#ifdef ENABLE_ERROR_CHECKS
   #define GLR_LOG_ERRORS() __GLR_LOG_ERRORS( __FILE__, __LINE__ );
#else
   #define GLR_LOG_ERRORS()
#endif 

///////////////////////////////////////////////////////////////////////////////

/**
* A utility method for checking OpenGL errors.
*
* Upon an error, an assertion will be thrown
*/
void __GLR_HALT_ERRORS();

#ifdef ENABLE_ERROR_CHECKS
   #define GLR_HALT_ERRORS() __GLR_HALT_ERRORS();
#else
   #define GLR_HALT_ERRORS()
#endif 

///////////////////////////////////////////////////////////////////////////////
