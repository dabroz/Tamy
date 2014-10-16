#include "core\ContinuousMemoryPool.h"
#include "core\MemoryUtils.h"
#include "core\Assert.h"
#include "core\CriticalSection.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

int ContinuousMemoryPool::POOL_INFO_CHUNK_SIZE = sizeof( size_t ) + sizeof( void* );

///////////////////////////////////////////////////////////////////////////////

ContinuousMemoryPool::ContinuousMemoryPool( size_t size )
{
   m_bufSize = size;
   m_memory = new char[ m_bufSize ];
   m_allocStartOffset = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

ContinuousMemoryPool::~ContinuousMemoryPool()
{
   delete [] m_memory;
   m_memory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ulong ContinuousMemoryPool::getMemoryUsed() const 
{ 
   size_t val = m_allocStartOffset;

   return val; 
}

///////////////////////////////////////////////////////////////////////////////

void ContinuousMemoryPool::reset()
{
   ASSERT_MSG( m_allocationsCount == 0, "There are still some objects left in the pool" );

   m_allocStartOffset = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

void* ContinuousMemoryPool::alloc( size_t size )
{
   size_t additionalAllocationSize = POOL_INFO_CHUNK_SIZE;
   long sizeAtEnd = m_bufSize - m_allocStartOffset - size - additionalAllocationSize;

   if ( sizeAtEnd >= 0 )
   {
      // get the pointer to the allocated memory chunk
      char* ptr = m_memory + m_allocStartOffset;

      // memorize pointer to the buffer and the chunk size
      size += additionalAllocationSize;
      *(void**)ptr = this;
      ptr += sizeof( void* );
      *(size_t*)ptr = size;
      ptr += sizeof( size );

      // move the pointer of the next free memory area
      m_allocStartOffset += size;
      ++m_allocationsCount;

      // return a pointer to the allocated memory
      return ptr;
   }
   else
   {
      // there's not enough memory
      ASSERT_MSG( false, "Not enough memory in the memory pool" );
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ContinuousMemoryPool::dealloc( void* ptr )
{
   --m_allocationsCount;

   if ( m_allocationsCount == 0 )
   {
      // reset the allocation start offset
      m_allocStartOffset = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
