#include "core\StringParser.h"


///////////////////////////////////////////////////////////////////////////////

StringParser::StringParser( const std::string* parsedString )
   : m_parsedString( parsedString )
   , m_cursorPos( -1 )
{
   m_delimiters = "\r\n\t ";
}

///////////////////////////////////////////////////////////////////////////////

void StringParser::addDelimiter( const char* delimiter )
{
   m_delimiters += delimiter;
}

///////////////////////////////////////////////////////////////////////////////

void StringParser::restart()
{
   m_cursorPos = -1;
}

///////////////////////////////////////////////////////////////////////////////

void StringParser::getStringBetween( const char* startTag, const char* endTag, std::string& outStr )
{
   int startPos = m_parsedString->find( startTag, m_cursorPos + 1 );
   if ( startPos == std::string::npos )
   {
      // tag not found
      return;
   }
   startPos += strlen( startTag );

   const int endPos = m_parsedString->find( endTag, startPos );
   if ( endPos == std::string::npos )
   {
      // tag not found
      return;
   }

   m_cursorPos = endPos + strlen( endTag );
   outStr = m_parsedString->substr( startPos, endPos - startPos );
}

///////////////////////////////////////////////////////////////////////////////

bool StringParser::findNext( const char* tokenWord )
{
   std::size_t pos;
   bool result = findNext( tokenWord, pos );
   return result;
}

///////////////////////////////////////////////////////////////////////////////

bool StringParser::findNext( const char* tokenWord, std::size_t& outPos )
{
   if ( tokenWord == NULL )
   {
      return false;
   }
   const uint wordLen = strlen( tokenWord );
   bool isWord = false;

   int nextCursorPos = m_cursorPos;
   while ( !isWord )
   {
      nextCursorPos = m_parsedString->find( tokenWord, nextCursorPos + 1 );
      if ( nextCursorPos == std::string::npos )
      {
         // we've reached the end of the string
         return false;
      }

      // verify that this is a word ( it should be surrounded by delimiters from both ends
      isWord = true;
      if ( nextCursorPos > 0 )
      {
         isWord &= isDelimiter( ( *m_parsedString )[nextCursorPos - 1] );
      }

      if ( isWord && nextCursorPos + wordLen < m_parsedString->length() )
      {
         isWord &= isDelimiter( ( *m_parsedString )[nextCursorPos + wordLen] );
      }
   }

   // return the position at which the word was found
   outPos = nextCursorPos;

   // found it - move the cursor behind it
   m_cursorPos = nextCursorPos + wordLen;
   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool StringParser::getNextWord( std::string& outWord )
{
   std::size_t pos;
   bool result = getNextWord( outWord, pos );
   return result;
}

///////////////////////////////////////////////////////////////////////////////

bool StringParser::getNextWord( std::string& outWord, std::size_t& outPos )
{
   // find where the next word starts
   int wordStart = m_parsedString->find_first_not_of( m_delimiters, m_cursorPos );
   if ( wordStart == std::string::npos )
   {
      // there are no more words
      return false;
   }

   // find where the next word ends
   int wordEnd = m_parsedString->find_first_of( m_delimiters, wordStart );
   if ( wordEnd == std::string::npos )
   {
      // this is the last word in the string
      wordEnd = m_parsedString->length();
   }

   const uint len = wordEnd - wordStart;
   if ( len == 0 )
   {
      return false;
   }

   // return the position at which the word was found
   outPos = wordStart;

   outWord = m_parsedString->substr( wordStart, wordEnd - wordStart );
   m_cursorPos = wordEnd;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool StringParser::isDelimiter( char character ) const
{
   const uint delimsCount = m_delimiters.length();
   for ( uint i = 0; i < delimsCount; ++i )
   {
      if ( character == m_delimiters[i] )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

/*

while ( ( startPos = shaderCode.find( "uniform", startPos ) ) != std::string::npos )
{
// found the first marker, so now look for the end marker
// and once you find it, we'll get the length of the string the markers contain - the length
// of our technique name

startPos += 7; // 11 is the length of "uniform" string

// the next line should contain the function declaration - extract the function name and store it
// as the technique's entry function
int functionDeclStart = shaderCode.find_first_of( "\n", startPos );
if ( functionDeclStart == std::string::npos )
{
m_errorMsg += "Function declaration not found\n";
return false;
}

functionDeclStart += 1;
int functionDeclEnd = shaderCode.find_first_of( "{", functionDeclStart );
if ( functionDeclEnd == std::string::npos )
{
m_errorMsg += "Invalid function declaration encountered\n";
return false;
}

std::string functionDecl = shaderCode.substr( functionDeclStart, functionDeclEnd - functionDeclStart );

functionDeclElems.clear();
StringUtils::tokenize( functionDecl, " \t()", functionDeclElems );

if ( functionDeclElems.size() < 2 )
{
m_errorMsg += "Invalid function declaration\n";
return false;
}

// store the technique name
outEntryFunctions.push_back( functionDeclElems[1] );
outTechniqueNames.push_back( functionDeclElems[0] );

// jump to the next position in the script string
startPos = functionDeclEnd + 1;
}

if ( outEntryFunctions.empty() )
{
// if no technique markers were found or there was a parsing error, assume there's just one common technique
// that uses the 'main' method as its entry function
outTechniqueNames.push_back( "<<default>>" );
outEntryFunctions.push_back( "main" );
}

return true;
*/