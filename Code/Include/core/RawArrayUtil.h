/// @file   core/RawArrayUtil.h
/// @brief  utilities for managing raw arrays
#ifndef _RAW_ARRAY_UTIL_H
#define _RAW_ARRAY_UTIL_H

#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

class MemoryAllocator;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utilities for managing raw arrays.
 */
class RawArrayUtil
{
public:
   /**
    * Appends an element to an array
    *
    * @param allocator        memory allocator
    * @param inOutArr         modified array
    * @param inOutArrSize     current array size. Upon return this will be modified to the new array size
    * @param item             appended item
    */
   template< typename T >
   static void push_back( MemoryAllocator* allocator, T*& inOutArr, uint& inOutArrSize, const T& item );

   /**
    * Removes an element from an array
    *
    * @param allocator        memory allocator
    * @param inOutArr         modified array
    * @param inOutArrSize     current array size. Upon return this will be modified to the new array size
    * @param itemIdx          index of the removed item
    */
   template< typename T >
   static void remove( MemoryAllocator* allocator, T*& inOutArr, uint& inOutArrSize, uint itemIdx );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/RawArrayUtil.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RAW_ARRAY_UTIL_H
