#ifndef _LIST_UTILS_H
#error "This file can only be included in ListUtils.h"
#else

#include "core\CollectionComparators.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T, typename TComparator >
void ListUtils::insertSorted( typename List< T >& list, const T& element )
{
   for ( List< T >::iterator it = list.begin(); !it.isEnd(); ++it )
   {
      if ( TComparator::isLesser( element, *it ) )
      {
         // this is the spot
         list.insert( it, element );
         return;
      }
   }

   // append at the end if no suitable insertion slot was found
   list.pushBack( element );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator ListUtils::find( typename const List< T >& list, const T& element )
{
   List< T >::const_iterator it = list.begin();
   for ( ; !it.isEnd(); ++it )
   {
      if ( Comparator< T >::isEqual( element, *it ) )
      {
         break;
      }
   }

   return it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator ListUtils::find( typename List< T >& list, const T& element )
{
   List< T >::iterator it = list.begin();
   for ( ; !it.isEnd(); ++it )
   {
      if ( Comparator< T >::isEqual( element, *it ) )
      {
         break;
      }
   }

   return it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T1, typename T2, typename TComparator >
typename List< T1 >::const_iterator ListUtils::find( typename const List< T1 >& list, const T2& element )
{
   List< T1 >::const_iterator it = list.begin();
   for ( ; !it.isEnd(); ++it )
   {
      if ( TComparator::isEqual( *it, element ) )
      {
         break;
      }
   }

   return it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T1, typename T2, typename TComparator >
typename List< T1 >::iterator ListUtils::find( typename List< T1 >& list, const T2& element )
{
   List< T1 >::iterator it = list.begin();
   for ( ; !it.isEnd(); ++it )
   {
      if ( TComparator::isEqual( *it, element ) )
      {
         break;
      }
   }

   return it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool ListUtils::compareWithPrevious( typename const List< T >& oldList, typename const List< T >& newList, typename List< T >& outAddedElements, typename List< T >& outRemovedElements )
{
   bool hasAnythingChanged = oldList.size() != newList.size();

   // look for elems that are in the new list, but are not present in the old one ( the added elems )
   for ( List< T >::const_iterator newIt = newList.begin(); !newIt.isEnd(); ++newIt )
   {
      const T& newElem = *newIt;
      bool elemFound = false;

      for ( List< T >::const_iterator oldIt = oldList.begin(); !oldIt.isEnd(); ++oldIt )
      {
         const T& oldElem = *oldIt;
         if ( Comparator< T >::isEqual( oldElem, newElem ) )
         {
            elemFound = true;
            break;
         }
      }

      if ( !elemFound )
      {
         // we've got one
         hasAnythingChanged |= true;
         outAddedElements.pushBack( newElem );
      }
   }

   // now do the opposite - look for the elements that disappeared from the newList
   for ( List< T >::const_iterator oldIt = oldList.begin(); !oldIt.isEnd(); ++oldIt )
   {
      const T& oldElem = *oldIt;
      bool elemFound = false;

      for ( List< T >::const_iterator newIt = newList.begin(); !newIt.isEnd(); ++newIt )
      {
         const T& newElem = *newIt;
         if ( Comparator< T >::isEqual( oldElem, newElem ) )
         {
            elemFound = true;
            break;
         }
      }

      if ( !elemFound )
      {
         // we've got one
         hasAnythingChanged |= true;
         outRemovedElements.pushBack( oldElem );
      }
   }

   return hasAnythingChanged;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool ListUtils::pushBackUnique( typename List< T >& list, const T& element )
{
   List< T >::iterator it = find( list, element );
   if ( it.isEnd() )
   {
      list.pushBack( element );
      return true;
   }
   else
   {
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LIST_UTILS_H
