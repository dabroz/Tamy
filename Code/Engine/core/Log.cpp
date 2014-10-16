#include "core.h"
#include "core/Log.h"
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////

Log Log::s_theInstance;

///////////////////////////////////////////////////////////////////////////////

void LOG( const char* msg, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, msg );
   vsprintf_s( formattedStr, msg, argptr );
   va_end( argptr );

   Log::s_theInstance.m_outputStream << formattedStr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

void CONDITIONAL_LOG( bool condition, const char* msg, ... )
{
   if ( !condition )
   {
      return;
   }

   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, msg );
   vsprintf_s( formattedStr, msg, argptr );
   va_end( argptr );

   Log::s_theInstance.m_outputStream << formattedStr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

void WARNING( const char* msg, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, msg );
   vsprintf_s( formattedStr, msg, argptr );
   va_end( argptr );

   Log::s_theInstance.m_outputStream << formattedStr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

