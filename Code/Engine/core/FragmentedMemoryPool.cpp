#include "core\FragmentedMemoryPool.h"
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

FragmentedMemoryPool::FragmentedMemoryPool( uint chunksCount, uint chunkSize )
   : m_chunksCount( chunksCount )
   , m_chunkSize( chunkSize )
   , m_allocatedChunksCount( 0 )
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();

   uint memToAllocSize = ( m_chunksCount + 1 ) * m_chunkSize;
   m_memBuf = (char*)memRouter.alloc( memToAllocSize * sizeof( char ), AM_DEFAULT, &memRouter.m_defaultAllocator );

   // initialize the head
   m_head = 0;
   char* chunkPtr = m_memBuf;
   for ( uint i = 0; i < m_chunksCount; ++i, chunkPtr += m_chunkSize )
   {
      FreeChunk* chunk = (FreeChunk*)chunkPtr;
      chunk->m_nextIdx = i + 1;
   }
   FreeChunk* chunk = (FreeChunk*)( m_memBuf + ( m_chunksCount - 1 ) * m_chunkSize );
   chunk->m_nextIdx = 0;
}

///////////////////////////////////////////////////////////////////////////////

FragmentedMemoryPool::~FragmentedMemoryPool()
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();

   memRouter.dealloc( m_memBuf, AM_DEFAULT );
   m_memBuf = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void* FragmentedMemoryPool::alloc( size_t size )
{
   if ( size > m_chunkSize )
   {
      ASSERT_MSG( size <= m_chunkSize, "This memory pool can allocate up to `m_chunkSize` bytes per allocation" );
      return NULL;
   }
   if ( m_allocatedChunksCount > m_chunksCount )
   {
      ASSERT_MSG ( m_allocatedChunksCount <= m_chunksCount, "Ran out of memory" );
      return NULL;
   }

   // get the pointer to the allocated chunk
   FreeChunk* allocatedChunk = (FreeChunk*)( m_memBuf + m_head * m_chunkSize );

   // move the head to the next chunk
   m_head = allocatedChunk->m_nextIdx;

   // update the score of how many chunks we have allocated
   ++m_allocatedChunksCount;

   return allocatedChunk;
}

///////////////////////////////////////////////////////////////////////////////

void FragmentedMemoryPool::dealloc( void* ptr )
{
   uint deallocatedChunkIdx = ( (ulong)ptr - (ulong)m_memBuf ) / m_chunkSize;

   // the deallocated chunk becomes the head, which points to the previous head chunk
   FreeChunk* deallocatedChunk = (FreeChunk*)( m_memBuf + deallocatedChunkIdx * m_chunkSize );
   deallocatedChunk->m_nextIdx = m_head;
   m_head = deallocatedChunkIdx;

   // update the score of how many chunks we have allocated
   --m_allocatedChunksCount;
}

///////////////////////////////////////////////////////////////////////////////

ulong FragmentedMemoryPool::getMemoryUsed() const
{
   ulong val = m_allocatedChunksCount * m_chunkSize;
   return val;
}

///////////////////////////////////////////////////////////////////////////////

bool FragmentedMemoryPool::isAddressCorrect( void* addr ) const
{
   long addrOffset = (long)addr - (long)m_memBuf;
   long bufSize = m_chunkSize * m_chunksCount;
   
   if ( addrOffset < 0 || addrOffset >= bufSize )
   {
      // the address isn't located in the buffer this allocator operates on
      return false;
   }

   // verify that the address is aligned to a chunk boundary
   return addrOffset % m_chunkSize == 0;
}

///////////////////////////////////////////////////////////////////////////////
