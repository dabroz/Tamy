/// @file   core/DefaultAllocator.h
/// @brief  default memory allocator
#pragma once

#include "core\types.h"
#include "core\MemoryAllocator.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * Default memory allocator.
 */
class DefaultAllocator : public MemoryAllocator
{
private:
   CriticalSection*     m_allocationLock;
   ulong                m_allocatedMemorySize;

public:
   /**
    * Constructor.
    */
   DefaultAllocator();
   ~DefaultAllocator();


   // -------------------------------------------------------------------------
   // MemoryAllocator implementation
   // -------------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;

private:
   // -------------------------------------------------------------------------
   // We don't support making copies of allocators
   // -------------------------------------------------------------------------
   DefaultAllocator( const DefaultAllocator& rhs ) {}
   void operator=( const DefaultAllocator& rhs ) {}
};

///////////////////////////////////////////////////////////////////////////////
