/// @file   core\StringParser.h
/// @brief  a tool that facilitates parsing strings
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * A tool that facilitates parsing strings.
 * It allows you to extract individual words from a string in a linear order.
 *
 * For instance, if you were searching for sets of words - every set
 * features some sort of key word followed by a bunch of words we want to discover:
 *
 *    key_word something1 something2;
 *    key_word something3 something4;
 *    ....
 *
 * Then this is the tool to do it. Simply define the delimiters ( whitespaces are considered
 * a delimiter by default ) - in this case that would be the ";" char, and start iterating over,
 * looking for key words and extracting the following words.
 */
class StringParser
{
   DECLARE_ALLOCATOR( StringParser, AM_DEFAULT );

private:
   const std::string*         m_parsedString;

   int                        m_cursorPos;
   std::string                m_delimiters;

public:
   /**
    * Constructor.
    *
    * @param parsedString        this string needs to exist until the parser finishes working
    */
   StringParser( const std::string* parsedString );

   /**
    * Adds a new delimiter.
    *
    * @param delimiter
    */
   void addDelimiter( const char* delimiter );

   /**
    * Restarts the parsing process.
    */
   void restart();

   /**
    * Looks for the next occurrence of the specified word.
    * The method ignores partial hits at the moment.
    *
    * @param tokenWord
    * @return           'true' if it was found, 'false' otherwise
    */
   bool findNext( const char* tokenWord );

   /**
    * Looks for the next occurrence of the specified word.
    * The method ignores partial hits at the moment.
    *
    * This version returns the position in the parsed string at which the token word
    * was found
    *
    * @param tokenWord
    * @param outPos
    * @return           'true' if it was found, 'false' otherwise
    */
   bool findNext( const char* tokenWord, std::size_t& outPos );

   /**
    * Returns the next word located after our current parsing position.
    *
    * @param outWord
    * @return        'true' if the word was found, 'false' otherwise
    */
   bool getNextWord( std::string& outWord );

   /**
    * Returns the next word located after our current parsing position.
    *
    * This version returns the position in the parsed string at which the token word
    * was found
    *
    * @param outWord
    * @param outPos
    * @return        'true' if the word was found, 'false' otherwise
    */
   bool getNextWord( std::string& outWord, std::size_t& outPos );

   /**
    * Returns the text between the specified tags.
    *
    * @param startTag
    * @param endTag
    * @param outStr
    */
   void getStringBetween( const char* startTag, const char* endTag, std::string& outStr );

private:
   /**
    * Checks if the specified character is a delimiter.
    *
    * @param character
    */
   bool isDelimiter( char character ) const;
};

///////////////////////////////////////////////////////////////////////////////
