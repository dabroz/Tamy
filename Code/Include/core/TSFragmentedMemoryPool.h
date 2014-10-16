/// @file   core/TSFragmentedMemoryPool.h
/// @brief  a thread safe version of FragmentedMemoryPool
#pragma once

#include "core\MemoryRouter.h"
#include "core\MemoryAllocator.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
* A memory pool divided into chunks, which can allocate up to 'chunkSize' memory at a time.
*/
class TSFragmentedMemoryPool : public MemoryAllocator
{
   DECLARE_ALLOCATOR( TSFragmentedMemoryPool, AM_DEFAULT );

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

   CriticalSection*     m_lock;

public:
   TSFragmentedMemoryPool( uint chunksCount = 1, uint chunkSize = 32 );
   ~TSFragmentedMemoryPool();

   /**
   * Checks if the specified address has been allocated in this pool and if it's correct.
   *
   * @param addr
   */
   bool isAddressCorrect( void* addr ) const;

   /**
   * Returns the number of currently allocated chunks.
   */
   inline ulong getAllocatedChunksCount() const {
      return m_allocatedChunksCount;
   }

   // ----------------------------------------------------------------------
   // MemoryAllocator implementation
   // ----------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;
};

///////////////////////////////////////////////////////////////////////////////
