#include "core.h"
#include "core\StringUtils.h"
#include "core/types.h"
#include "core/Assert.h"
#include <wchar.h>



namespace StringUtils
{

///////////////////////////////////////////////////////////////////////////////

void tokenize( const std::string& str, const std::string& tokens, List<std::string>& output )
{
   std::size_t start, end;
   std::string tmp;
   start = 0;
   end = 0;

   while ( end != std::string::npos )
   {
      end = str.find_first_of(tokens, start);
      tmp = str.substr(start, end - start);
      if (tmp.length() > 0)
      {
         output.pushBack(tmp);
      }
      start = end + 1;
   }
}

///////////////////////////////////////////////////////////////////////////////

void tokenize( const std::string& str, const std::string& tokens, Array<std::string>& output )
{
   std::size_t start, end;
   std::string tmp;
   start = 0;
   end = 0;

   while ( end != std::string::npos )
   {
      end = str.find_first_of( tokens, start );
      tmp = str.substr( start, end - start );
      if ( tmp.length() > 0 )
      {
         output.push_back( tmp );
      }
      start = end + 1;
   }
}


///////////////////////////////////////////////////////////////////////////////

std::string replace( const std::string& str, const char* strToReplace, const char* replacementStr )
{
   uint strToReplacementLen = strlen( strToReplace );

   std::size_t start, end;
   std::string tmp;
   start = 0;
   end = 0;

   while ( end != std::string::npos )
   {
      end = str.find( strToReplace, start );
      tmp += str.substr(start, end - start);

      if ( end != std::string::npos )
      {
         tmp += replacementStr;
      }
      start = end + strToReplacementLen;
   }

   return tmp;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int calculateHash( const std::string& str )
{
   unsigned int hash = INT_MAX;

   unsigned int count = str.length() / sizeof( unsigned int );
   if ( str.length() % sizeof( unsigned int ) > 0 )
   {
      count++;
   }
   unsigned int tmpStrSize = count * sizeof( unsigned int );
   static char tempStr[ 1024 ];
   memset( tempStr, 0, tmpStrSize );
   memcpy( tempStr, str.c_str(), str.length() );
   const unsigned int* valsArr = ( const unsigned int* )( tempStr );
   for ( unsigned int i = 0; i < count; ++i )
   {
      hash ^= valsArr[ i ];
   }

   return hash;
}

///////////////////////////////////////////////////////////////////////////////

void toUpper( std::string& inOutStr )
{
   uint count = inOutStr.length();
   char offset = 'A' - 'a';

   ASSERT_MSG( count < 512, "String is too long" );
   char tmpStr[512];
   const char* analyzedStr = inOutStr.c_str();
   for ( uint i = 0; i < count; ++i )
   {
      char c = analyzedStr[i];

      if ( c >= 'a' && c <= 'z' )
      {
         tmpStr[i] = (char)(c + offset);
      }
      else
      {
         tmpStr[i] = (char)c;
      }
   }
   tmpStr[count] = 0;

   inOutStr = tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

char* unicodeToAnsii( const wchar_t* unicodeStr )
{
   size_t l = wcslen( unicodeStr );
   char* asciiStr = (char*)malloc( l + 1 ); 
   ASSERT( asciiStr != NULL );

   for( size_t i = 0; i < l; ++i ) 
   {
      if ( unicodeStr[i] > 127 ) 
      {
         asciiStr[i] = '?'; // no ascii
      }
      else
      {
         asciiStr[i] = unicodeStr[i] & 0xff;
      }
   }

   asciiStr[l] = 0;

   return asciiStr;
}

///////////////////////////////////////////////////////////////////////////////

} // StringUtils
