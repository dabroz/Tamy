#include "core.h"
#include "core/InStream.h"
#include "core/Array.h"


///////////////////////////////////////////////////////////////////////////////

InStream& InStream::operator>>( std::string& val )
{
   // load the length of the string
   uint len;
   load( (void*)&len, sizeof( uint ) );

   // load its contents
   char* tmpStr = new char[len + 1];
   load( (void*)tmpStr, len );
   tmpStr[len] = 0;
   val = std::string( tmpStr );
   delete tmpStr;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void InStream::skip( uint sizeInBytes )
{
   Array< byte > skipBuf( sizeInBytes ); 
   skipBuf.resize( sizeInBytes );
   load( (byte*)skipBuf, sizeInBytes );
}

///////////////////////////////////////////////////////////////////////////////
