#ifndef _RAW_ARRAY_UTIL_H
#error "This file can only be included in RawArrayUtil.h"
#else

#include <memory>


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void RawArrayUtil::push_back( MemoryAllocator* allocator, T*& inOutArr, uint& inOutArrSize, const T& item )
{
   T* newArray = (T*)allocator->alloc( sizeof( T ) * ( inOutArrSize + 1 ) );
   if ( inOutArr != NULL )
   {
      memcpy( newArray, inOutArr, sizeof( T ) * inOutArrSize );
   }

   newArray[inOutArrSize] = item;
   if ( inOutArr != NULL )
   {
      allocator->dealloc( inOutArr );
   }
   inOutArr = newArray;
   ++inOutArrSize;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void RawArrayUtil::remove( MemoryAllocator* allocator, T*& inOutArr, uint& inOutArrSize, uint itemIdx )
{
   if ( inOutArrSize == 0 )
   {
      return;
   }
   else if ( inOutArrSize == 1 )
   {
      allocator->dealloc( inOutArr );
      inOutArr = NULL;
      inOutArrSize = 0;
   }
   else
   {
      T* newArray = (T*)allocator->alloc( sizeof( T ) * ( inOutArrSize - 1 ) );
      memcpy( newArray, inOutArr, sizeof( T ) * itemIdx );
      memcpy( newArray + itemIdx, inOutArr + itemIdx + 1, sizeof( T ) * ( inOutArrSize - itemIdx - 1 ) );

      allocator->dealloc( inOutArr );
      inOutArr = newArray;
      --inOutArrSize;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RAW_ARRAY_UTIL_H
