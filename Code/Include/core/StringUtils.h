/// @file      core\StringUtils.h
/// @brief     various tools operating on strings
#pragma once

#include <string>
#include "core\List.h"
#include "core\Array.h"


namespace StringUtils
{

///////////////////////////////////////////////////////////////////////////////

/**
 * Tokenizes a string by splitting it across the boundaries of the specified
 * characters (tokens).
 *
 * @param str     string to be tokenized
 * @param tokens  characters used as the splitting markers
 * @param output  collection which will be filled with the results of string slicing
 */
void tokenize( const std::string& str, const std::string& tokens, List<std::string>& output );

///////////////////////////////////////////////////////////////////////////////

/**
* Tokenizes a string by splitting it across the boundaries of the specified
* characters (tokens).
*
* @param str     string to be tokenized
* @param tokens  characters used as the splitting markers
* @param output  collection which will be filled with the results of string slicing
*/
void tokenize( const std::string& str, const std::string& tokens, Array<std::string>& output );

///////////////////////////////////////////////////////////////////////////////

/**
 * Replaces all occurrences of 'strToReplace' with 'replacementStr'.
 *
 * @param str
 * @param strToReplace
 * @param replacementStr
 */
std::string replace( const std::string& str, const char* strToReplace, const char* replacementStr );

///////////////////////////////////////////////////////////////////////////////

/**
 * Calculates a has value based on the string contents
 *
 * @param str
 * @return     hash value
 */
unsigned int calculateHash( const std::string& str );

///////////////////////////////////////////////////////////////////////////////

/**
 * Converts all characters in the string to upper case.
 *
 * @param inOutStr
 */
void toUpper( std::string& inOutStr );

///////////////////////////////////////////////////////////////////////////////

/**
 * Converts a unicode string to a regular string.
 *
 * CAUTION: allocates the memory for the ASCII string, so be sure to release it afterwards
 * with a call to 'free'
 *
 * @param unicodeStr
 */
char* unicodeToAnsii( const wchar_t* unicodeStr );

///////////////////////////////////////////////////////////////////////////////

} // StringUtils
