#ifndef _COLLECTION_COMPARATORS_H
#error "This file can only be included in CollectionComparators.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Comparator< T >::isLesser( const T& rhs, const T& lhs )
{
   return rhs < lhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Comparator< T >::isGreater( const T& rhs, const T& lhs )
{
   return rhs > lhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Comparator< T >::isEqual( const T& rhs, const T& lhs )
{
   return rhs == lhs;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _COLLECTION_COMPARATORS_H
