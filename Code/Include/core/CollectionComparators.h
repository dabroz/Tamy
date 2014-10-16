/// @file   core/CollectionComparators.h
/// @brief  comparator functors used by collection utilities
#ifndef _COLLECTION_COMPARATORS_H
#define _COLLECTION_COMPARATORS_H

///////////////////////////////////////////////////////////////////////////////

/**
 * Default comparator functor - assumes that compared types implement the < operator
 */
template< typename T >
class Comparator
{
public:
   /**
    * rhs < lhs
    */
   static bool isLesser( const T& rhs, const T& lhs );

   /**
    * rhs > lhs
    */
   static bool isGreater( const T& rhs, const T& lhs );

   /**
    * rhs == lhs
    */
   static bool isEqual( const T& rhs, const T& lhs );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\CollectionComparators.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _COLLECTION_COMPARATORS_H
