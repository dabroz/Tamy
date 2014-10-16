/// @file   core/CollectionUtils.h
/// @brief  a set of utilities that operate on collections of data
#ifndef _COLLECTION_UTILS_H
#define _COLLECTION_UTILS_H

#include <vector>
#include "core/Array.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

class CollectionUtils
{
public:
   /**
    * Checks if the tested collection contains all entries specified in the pattern collection.
    *
    * @param testedCollection
    * @param pattern
    */
   template< typename T >
   static bool containsAll( const std::vector< T >& testedCollection, const Array< T >& pattern );

   template< typename T >
   static bool containsAll( const Array< T >& testedCollection, const std::vector< T >& pattern );

   template< typename T >
   static bool containsAll( const std::vector< T >& testedCollection, const std::vector< T >& pattern );

   template< typename T >
   static bool containsAll( const Array< T >& testedCollection, const Array< T >& pattern );

   template< typename T >
   static bool containsAll( const T* arrTestedCollection, uint testedCollectionSize, const T* arrPattern, uint patternSize );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/CollectionUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _COLLECTION_UTILS_H
