/// @file   core/ArrayUtils.h
/// @brief  Array utilities
#ifndef _ARRAY_UTILS
#define _ARRAY_UTILS

#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class ArrayUtils
{
public:
   /**
    * Sorts the specified array using a quick-sort algorithm.
    *
    * @param inOutArr
    */
   template< typename T >
   static void quickSort( typename Array< T >& inOutArr );

   /**
    * Sorts the specified array using a quick-sort algorithm ( using a custom comparator ).
    *
    * @param inOutArr
    */
   template< typename T, typename TComparator >
   static void quickSort( typename Array< T >& inOutArr );

   /**
    * Converts a string to an array with elements of the specified type.
    *
    * @param str
    * @param outArr
    */
   template< typename T >
   static void convertStringToArray( const std::string& str, Array< T >& outArr );

   /**
    * Creates an array from a raw array.
    *
    * @param rawArray
    * @param size
    * @param outArr
    */
   template< typename T >
   static void createFromRawArray( const T* rawArray, uint size, Array< T >& outArr );

   /**
    * Searches for the specified pattern in the specified string of data.
    *
    * @param str
    * @param pattern
    * @param startIdx
    * @return index of the first character of the pattern, or -1 if the pattern wasn't found
    */
   template< typename T >
   static int findStr( const Array< T >& str, const Array< T >& pattern, int startIdx = 0 );

private:
   template< typename T, typename TComparator >
   static void recursiveQuickSort( typename Array< T >& inOutArr, int left, int right );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ArrayUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ARRAY_UTILS
