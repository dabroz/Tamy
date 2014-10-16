#ifndef _ARRAY_UTILS
#error "This file can only be included in ArrayUtils.h
#else

#include "core\CollectionComparators.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void ArrayUtils::quickSort( typename Array< T >& inOutArr )
{
   recursiveQuickSort< T, Comparator< T > >( inOutArr, 0, (int)( inOutArr.size() - 1 ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T, typename TComparator >
void ArrayUtils::quickSort( typename Array< T >& inOutArr )
{
   recursiveQuickSort< T, TComparator >( inOutArr, 0, (int)( inOutArr.size() - 1 ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T, typename TComparator >
void ArrayUtils::recursiveQuickSort( typename Array< T >& inOutArr, int left, int right )
{
   int i = left, j = right;
   T pivot = inOutArr[( left + right ) / 2];

   // partition
   while( i <= j )
   {
      while( TComparator::isLesser( inOutArr[i], pivot ) )
      {
         i++;
      }

      while( TComparator::isGreater( inOutArr[j], pivot ) )
      {
         j--;
      }

      if ( i <= j ) 
      {
         T tmp = inOutArr[i];
         inOutArr[i] = inOutArr[j];
         inOutArr[j] = tmp;
         i++;
         j--;
      }
   }

   // recursion
   if ( left < j )
   {
      recursiveQuickSort< T, TComparator >( inOutArr, left, j );
   }
   if ( i < right )
   {
      recursiveQuickSort< T, TComparator >( inOutArr, i, right );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void ArrayUtils::convertStringToArray( const std::string& str, Array< T >& outArr )
{
   const uint count = str.length();
   outArr.resize( count );

   for ( uint i = 0; i < count; ++i )
   {
      outArr[i] = (T)str[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void ArrayUtils::createFromRawArray( const T* rawArray, uint size, Array< T >& outArr )
{
   outArr.resize( size );
   memcpy( ( T* ) outArr, rawArray, sizeof( T ) * size );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
int ArrayUtils::findStr( const Array< T >& str, const Array< T >& pattern, int startIdx )
{
   const uint strLen = str.size();
   const uint patternLen = pattern.size();

   uint matchingCharactersCount;
   uint firstCharacterIdx = -1;
   for ( uint i = startIdx; i < strLen; ++i )
   {
      matchingCharactersCount = 0;
      firstCharacterIdx = i;
      for ( uint j = 0; i < strLen && j < patternLen; ++j, ++i, ++matchingCharactersCount )
      {
         if ( str[i] != pattern[j] )
         {
            break;
         }
      }

      if ( matchingCharactersCount >= patternLen )
      {
         return firstCharacterIdx;
      }
   }

   // no match was found
   return -1;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _ARRAY_UTILS
