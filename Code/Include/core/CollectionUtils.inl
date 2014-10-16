#ifndef _COLLECTION_UTILS_H
#error "This file can only be included from CollectionUtils.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool CollectionUtils::containsAll( const std::vector< T >& testedCollection, const Array< T >& pattern )
{
   if ( testedCollection.empty() || pattern.empty() )
   {
      // test collection or the pattern is empty, so there's no way of telling the result, so let's just
      // assume that the tested collection DOESN'T contain all the elems
      return false;
   }

   return containsAll( &testedCollection[0], testedCollection.size(), (const T*)pattern, pattern.size() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool CollectionUtils::containsAll( const Array< T >& testedCollection, const std::vector< T >& pattern )
{
   if ( testedCollection.empty() || pattern.empty() )
   {
      // test collection or the pattern is empty, so there's no way of telling the result, so let's just
      // assume that the tested collection DOESN'T contain all the elems
      return false;
   }

   return containsAll( (const T*)testedCollection, testedCollection.size(), &pattern[0], pattern.size() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool CollectionUtils::containsAll( const std::vector< T >& testedCollection, const std::vector< T >& pattern )
{
   if ( testedCollection.empty() || pattern.empty() )
   {
      // test collection or the pattern is empty, so there's no way of telling the result, so let's just
      // assume that the tested collection DOESN'T contain all the elems
      return false;
   }

   return containsAll( &testedCollection[0], testedCollection.size(), &pattern[0], pattern.size() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool CollectionUtils::containsAll( const Array< T >& testedCollection, const Array< T >& pattern )
{
   if ( testedCollection.empty() || pattern.empty() )
   {
      // test collection or the pattern is empty, so there's no way of telling the result, so let's just
      // assume that the tested collection DOESN'T contain all the elems
      return false;
   }

   return containsAll( (const T*)testedCollection, testedCollection.size(), (const T*)pattern, pattern.size() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool CollectionUtils::containsAll( const T* arrTestedCollection, uint testedCollectionSize, const T* arrPattern, uint patternSize )
{
   if ( testedCollectionSize == 0 || patternSize == 0 )
   {
      // test collection or the pattern is empty, so there's no way of telling the result, so let's just
      // assume that the tested collection DOESN'T contain all the elems
      return false;
   }

   bool patternElemFound = false;
   for ( uint patternElemIdx = 0; patternElemIdx < patternSize; ++patternElemIdx )
   {
      const T& patternElem = arrPattern[patternElemIdx];

      patternElemFound = false;
      for ( uint testedElemIdx = 0; testedElemIdx < testedCollectionSize; ++testedElemIdx )
      {
         if ( arrTestedCollection[testedElemIdx] == patternElem )
         {
            patternElemFound = true;
            break;
         }
      }

      if ( !patternElemFound )
      {
         // pattern element wasn't found in the collection
         return false;
      }
   }
 
   // all pattern elements were located
   return true;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _COLLECTION_UTILS_H
