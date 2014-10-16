/// @file   core/ContinuousMemoryPool.h
/// @brief  simple memory pool
#pragma once

#include "core\types.h"
#include "core\MemoryAllocator.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A simple memory pool.
 */
class ContinuousMemoryPool : public MemoryAllocator
{
private:
   static int           POOL_INFO_CHUNK_SIZE;

   size_t               m_bufSize;
   char*                m_memory;
   size_t               m_allocStartOffset;

   unsigned int         m_allocationsCount;

public:
   /**
    * Constructor.
    *
    * @param size          buffer size
    */
   ContinuousMemoryPool( size_t size );
   ~ContinuousMemoryPool();

   /**
    * Returns the number of allocations made.
    */
   inline unsigned int getAllocationsCount() const { return m_allocationsCount; }

   /**
    * Resets the pool allocation pointer, making sure the objects
    * that will be allocated here from now on will be allocated
    * from the pool's memory block start.
    */
   void reset();

   // -------------------------------------------------------------------------
   // MemoryAllocator implementation
   // -------------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;

private:
   // -------------------------------------------------------------------------
   // We don't allow for copying of memory pools
   // -------------------------------------------------------------------------
   ContinuousMemoryPool( const ContinuousMemoryPool& rhs ) {}
   void operator=( const ContinuousMemoryPool& rhs ) {}
};

///////////////////////////////////////////////////////////////////////////////
