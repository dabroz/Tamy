#include "core.h"
#include "core/Log.h"
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////

Log Log::s_theInstance;

///////////////////////////////////////////////////////////////////////////////

void LOG( const char* msg, ... )
{
   static char formattedStr[2048];

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

   static char formattedStr[2048];

   va_list argptr;
   va_start( argptr, msg );
   vsprintf_s( formattedStr, msg, argptr );
   va_end( argptr );

   Log::s_theInstance.m_outputStream << formattedStr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

void WARNING( const char* msg, ... )
{
   static char formattedStr[2048];

   va_list argptr;
   va_start( argptr, msg );
   vsprintf_s( formattedStr, msg, argptr );
   va_end( argptr );

   Log::s_theInstance.m_outputStream << formattedStr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

