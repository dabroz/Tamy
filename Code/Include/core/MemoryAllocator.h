/// @file   core/MemoryAllocator.h
/// @brief  memory allocator interface
#pragma once

#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class MemoryAllocator
{
public:
   virtual ~MemoryAllocator() {}

   /**
    * Allocates a chunk of memory of the specified size.
    *
    * @param size
    */
   virtual void* alloc( size_t size ) = 0;

   /**
    * Deallocates the memory from the specified address.
    */
   virtual void dealloc( void* ptr ) = 0;

   /**
    * Returns the amount of currently allocated memory.
    */
   virtual ulong getMemoryUsed() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
