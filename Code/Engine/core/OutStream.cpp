#include "core.h"
#include "core/OutStream.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

OutStream& OutStream::operator<<( const std::string& val )
{
   // save the string length
   uint len = val.length();
   save( (void*)&len, sizeof( uint ) );

   // save the string's contents
   save( (void*)val.c_str(), len );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////
