#include "core\RoundBuffer.h"
#include "core\Assert.h"
#include "core\MemoryRouter.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

size_t RoundBuffer::BUFFER_INFO_CHUNK_SIZE = sizeof( size_t );

///////////////////////////////////////////////////////////////////////////////

RoundBuffer::RoundBuffer( size_t size )
{
   m_bufSize = size;

   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   m_memory = (char*)memRouter.alloc( m_bufSize * sizeof( char ), AM_DEFAULT, &memRouter.m_defaultAllocator );

   m_tail = 0;
   m_head = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

RoundBuffer::~RoundBuffer()
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   memRouter.dealloc( m_memory, AM_DEFAULT );
   m_memory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void* RoundBuffer::alloc( size_t size )
{
   size_t actualAllocationSize = size + BUFFER_INFO_CHUNK_SIZE;
   size_t spaceTillEnd = m_bufSize - m_head;

   size_t allocationStartOffset = m_head;
   if ( spaceTillEnd < actualAllocationSize )
   {
      // if possible, store the size of the allocation at the end.
      // It will be used by data access methods to tell if we definitely need to wrap
      // around the end
      if ( spaceTillEnd >= BUFFER_INFO_CHUNK_SIZE )
      {
         *(size_t*)( m_memory + m_head ) = actualAllocationSize;
      }

      // we need to wrap around the end for this allocation to fit in
      allocationStartOffset = 0;
      spaceTillEnd = 0;
   }

   // check if there's enough free space to accommodate this piece
   size_t freeSpace = m_tail + spaceTillEnd;
   if ( actualAllocationSize > freeSpace )
   {
      // yes - this allocation will cause an overflow
      ASSERT_MSG( false, "Not enough memory in the round buffer - this allocation would cause a buffer overflow" );
      return NULL;
   }

   // if we got to this point, it means we can go through with the allocation
   char* ptr = m_memory + allocationStartOffset;

   // memorize pointer to the buffer and the chunk size
   *(size_t*)ptr = actualAllocationSize;
   ptr += sizeof( actualAllocationSize );

   // move the pointer to the free memory start
   m_head = allocationStartOffset + actualAllocationSize;

   ++m_allocationsCount;
   
   // return the pointer
   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void RoundBuffer::dealloc( void* ptr )
{
   // we'll always be deallocating the chunk the tail points to - pointer specified as the method param
   // only serves the purpose of verifying that the deallocation is correct

   size_t tailOffset = 0;
   size_t tailSize = 0;
   getTail( tailOffset, tailSize ); 

   // verify that the released address is correct
   ASSERT_MSG( getMemoryUsed() > 0, "The buffer is empty" );
   ASSERT_MSG( ptr == m_memory + tailOffset + BUFFER_INFO_CHUNK_SIZE, "Trying to deallocate an invalid address" );

   m_tail = tailOffset + tailSize;

   --m_allocationsCount;
}

///////////////////////////////////////////////////////////////////////////////

void RoundBuffer::getTail( size_t& outOffset, size_t& outSize )
{
   size_t spaceTillEnd = m_bufSize - m_tail;
   if ( spaceTillEnd <= BUFFER_INFO_CHUNK_SIZE )
   {
      outOffset = 0;
      outSize = *(size_t*)m_memory;
   }
   else
   {
      outSize = *(size_t*)( m_memory + m_tail );
      if ( outSize > spaceTillEnd )
      {
         outOffset = 0;
      }
      else
      {
         outOffset = m_tail;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

ulong RoundBuffer::getMemoryUsed() const
{
   ulong occupiedSpace;
   if ( m_head > m_tail )
   {
      occupiedSpace = m_head - m_tail;
   }
   else if ( m_head == m_tail )
   {
      if ( m_allocationsCount == 0 )
      {
         occupiedSpace = 0;
      }
      else
      {
         occupiedSpace = m_bufSize;
      }
   }
   else
   {
      occupiedSpace = m_bufSize - ( m_tail - m_head );
   }

   return occupiedSpace;
}

///////////////////////////////////////////////////////////////////////////////
