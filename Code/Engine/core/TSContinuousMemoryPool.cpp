#include "core\TSContinuousMemoryPool.h"
#include "core\MemoryUtils.h"
#include "core\Assert.h"
#include "core\CriticalSection.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

int TSContinuousMemoryPool::POOL_INFO_CHUNK_SIZE = sizeof( size_t ) +sizeof( void* );

///////////////////////////////////////////////////////////////////////////////

TSContinuousMemoryPool::TSContinuousMemoryPool( size_t size )
: m_allocationLock( new CriticalSection() )
{
   m_bufSize = size;
   m_memory = new char[m_bufSize];
   m_allocStartOffset = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

TSContinuousMemoryPool::~TSContinuousMemoryPool()
{
   delete[] m_memory;
   m_memory = NULL;

   delete m_allocationLock;
   m_allocationLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ulong TSContinuousMemoryPool::getMemoryUsed() const
{
   m_allocationLock->enter();
   size_t val = m_allocStartOffset;
   m_allocationLock->leave();

   return val;
}

///////////////////////////////////////////////////////////////////////////////

void TSContinuousMemoryPool::reset()
{
   m_allocationLock->enter();
   ASSERT_MSG( m_allocationsCount == 0, "There are still some objects left in the pool" );

   m_allocStartOffset = 0;
   m_allocationsCount = 0;
   m_allocationLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void* TSContinuousMemoryPool::alloc( size_t size )
{
   m_allocationLock->enter();

   size_t additionalAllocationSize = POOL_INFO_CHUNK_SIZE;
   long sizeAtEnd = m_bufSize - m_allocStartOffset - size - additionalAllocationSize;

   if ( sizeAtEnd >= 0 )
   {
      // get the pointer to the allocated memory chunk
      char* ptr = m_memory + m_allocStartOffset;

      // memorize pointer to the buffer and the chunk size
      size += additionalAllocationSize;
      *( void** ) ptr = this;
      ptr += sizeof( void* );
      *( size_t* ) ptr = size;
      ptr += sizeof( size );

      // move the pointer of the next free memory area
      m_allocStartOffset += size;
      ++m_allocationsCount;

      // return a pointer to the allocated memory
      m_allocationLock->leave();
      return ptr;
   }
   else
   {
      m_allocationLock->leave();

      // there's not enough memory
      ASSERT_MSG( false, "Not enough memory in the memory pool" );
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void TSContinuousMemoryPool::dealloc( void* ptr )
{
   m_allocationLock->enter();

   --m_allocationsCount;

   if ( m_allocationsCount == 0 )
   {
      // reset the allocation start offset
      m_allocStartOffset = 0;
   }

   m_allocationLock->leave();
}

///////////////////////////////////////////////////////////////////////////////
