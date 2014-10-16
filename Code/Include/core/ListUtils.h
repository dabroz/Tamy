/// @file   core/ListUtils.h
/// @brief  a set of utility method that operate on lists
#ifndef _LIST_UTILS_H
#define _LIST_UTILS_H

#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A set of utility method that operate on lists.
 */
class ListUtils
{
public:
   /**
    * Inserts an element into the list, preserving its sorted order.
    *
    * If you use only this method to introduce new elements into the list,
    * you are guaranteed to end up with a sorted list.
    * 
    * The algorithm runs with O(n) average complexity.
    *
    * @param list
    * @param element
    */
   template< typename T, typename TComparator = Comparator< T > >
   static void insertSorted( typename List< T >& list, const T& element );

   /**
    * Looks for the specified element in the specified list. Returns an iterator that points at it ( const version )
    *
    * The algorithm runs with O(n) average complexity.
    *
    * @param list
    * @param element
    */
   template< typename T >
   static typename List< T >::const_iterator find( typename const List< T >& list, const T& element );

   /**
    * Looks for the specified element in the specified list. Returns an iterator that points at it.
    *
    * The algorithm runs with O(n) average complexity.
    *
    * @param list
    * @param element
    */
   template< typename T >
   static typename List< T >::iterator find( typename List< T >& list, const T& element );

   /**
    * Looks for the specified element in the specified list using a custom comparator. Returns an iterator that points at it ( const version )
    *
    * The algorithm runs with O(n) average complexity.
    *
    * @param TComparator
    * @param list
    * @param element
    */
   template< typename T1, typename T2, typename TComparator >
   static typename List< T1 >::const_iterator find( typename const List< T1 >& list, const T2& element );

   /**
    * Looks for the specified element in the specified list using a custom comparator. Returns an iterator that points at it.
    *
    * The algorithm runs with O(n) average complexity.
    *
    * @param TComparator
    * @param list
    * @param element
    */
   template< typename T1, typename T2, typename TComparator >
   static typename List< T1 >::iterator find( typename List< T1 >& list, const T2& element );

   /**
    * Compares the contents of one list with the other in search for differences - elements that have been added and removed.
    * It then outputs them to the respective output lists.
    *
    * @param oldList
    * @param newList
    * @param outAddedElements
    * @param outRemovedElements
    */
   template< typename T >
   static bool compareWithPrevious( typename const List< T >& oldList, typename const List< T >& newList, typename List< T >& outAddedElements, typename List< T >& outRemovedElements );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ListUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LIST_UTILS_H
