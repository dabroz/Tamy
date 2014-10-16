/// @file   core/FragmentedMemoryPool.h
/// @brief  a memory pool divided into chunks, which can allocate up to 'chunkSize' memory at a time
#pragma once

#include "core\MemoryRouter.h"
#include "core\MemoryAllocator.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A memory pool divided into chunks, which can allocate up to 'chunkSize' memory at a time.
 */
class FragmentedMemoryPool : public MemoryAllocator
{
   DECLARE_ALLOCATOR( FragmentedMemoryPool, AM_DEFAULT );

private:
   struct FreeChunk
   {
      uint        m_nextIdx;
   };

private:
   const uint           m_chunkSize;
   uint                 m_chunksCount;
   uint                 m_head;
   char*                m_memBuf;

   ulong                m_allocatedChunksCount;

public:
   FragmentedMemoryPool( uint chunksCount = 1, uint chunkSize = 32 );
   ~FragmentedMemoryPool();

   /**
    * Checks if the specified address has been allocated in this pool and if it's correct.
    *
    * @param addr
    */
   bool isAddressCorrect( void* addr ) const;

   /**
    * Returns the number of currently allocated chunks.
    */
   inline ulong getAllocatedChunksCount() const { return m_allocatedChunksCount; }

   // ----------------------------------------------------------------------
   // MemoryAllocator implementation
   // ----------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;
};

///////////////////////////////////////////////////////////////////////////////
