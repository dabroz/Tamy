/// @file   core/RoundBuffer.h
/// @brief  round buffer data structure
#ifndef _ROUND_BUFFER_H
#define _ROUND_BUFFER_H

#include "core\MemoryRouter.h"
#include "core\MemoryAllocator.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This is a structure similar to a queue, but it operates
 * on a constant area of memory, and thus is much much faster.
 *
 * NOTE: This allocator is not thread safe - keep that in mind when you use it.
 */
class RoundBuffer : public MemoryAllocator
{
   DECLARE_ALLOCATOR( RoundBuffer, AM_DEFAULT );

private:
   static size_t        BUFFER_INFO_CHUNK_SIZE;
   size_t               m_bufSize;
   char*                m_memory;

   volatile size_t      m_tail;
   volatile size_t      m_head;

   volatile uint        m_allocationsCount;

public:
   /**
    * Constructor.
    *
    * @param size          buffer size
    */
   RoundBuffer( size_t size );
   ~RoundBuffer();

   /**
    * Returns the number of allocations made.
    */
   inline uint getAllocationsCount() const;

   /**
    * Returns the number of elements in it ( equal to the number of allocations that have been made ).
    */
   inline uint size() const;

   /**
    * Checks if the buffer has any elements in it.
    */
   inline bool empty() const;

   /**
    * Returns the front element.
    */
   template< typename T >
   T* front();

   /**
    * Calculates amount of memory required to allocate an object of the specified type.
    *
    * @param T
    */
   template< typename T >
   static uint sizeOf();

   // -------------------------------------------------------------------------
   // MemoryAllocator implementation
   // -------------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;

private:
   void getTail( size_t& outOffset, size_t& outSize );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\RoundBuffer.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ROUND_BUFFER_H
