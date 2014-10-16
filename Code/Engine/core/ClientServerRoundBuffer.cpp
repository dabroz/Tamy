#include "core\ClientServerRoundBuffer.h"
#include "core\Assert.h"
#include "core\CriticalSection.h"

#ifdef _DEBUG
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#endif


///////////////////////////////////////////////////////////////////////////////

size_t ClientServerRoundBuffer::BUFFER_INFO_CHUNK_SIZE = sizeof( size_t );
uint ClientServerRoundBuffer::MAX_TIMESTAMP = 65535;

///////////////////////////////////////////////////////////////////////////////

ClientServerRoundBuffer::ClientServerRoundBuffer( size_t size )
   : m_writerThreadId( 0 )
   , m_readerThreadId( 0 )
   , m_enabled( true )
{
   m_bufSize = size;

   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   m_memory = (char*)memRouter.alloc( m_bufSize * sizeof( char ), AM_DEFAULT, &memRouter.m_defaultAllocator );

   m_tail = 0;
   m_head = 0;
   m_commandsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

ClientServerRoundBuffer::~ClientServerRoundBuffer()
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   memRouter.dealloc( m_memory, AM_DEFAULT );
   m_memory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ClientServerRoundBuffer::setWriterThreadId( ulong threadId )
{
   m_writerThreadId = threadId;
}

///////////////////////////////////////////////////////////////////////////////

void ClientServerRoundBuffer::setReaderThreadId( ulong threadId )
{
   m_readerThreadId = threadId;
}

///////////////////////////////////////////////////////////////////////////////

void* ClientServerRoundBuffer::alloc( size_t size )
{
#if _DEBUG_ROUNDBUFFER
   ThreadSystem& ts = TSingleton< ThreadSystem >::getInstance();
   Thread& currThread = ts.getCurrentThread();
   if ( m_writerThreadId > 0 && currThread.getThreadId() != m_writerThreadId )
   {
      ASSERT_MSG( false, "Only a thread designated as the writer thread can allocate memory in this buffer" );
   }
#endif

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

   // if we got to this point, it means we can go through with the allocation
   char* ptr = m_memory + allocationStartOffset;

   // memorize pointer to the buffer and the chunk size
   *(size_t*)ptr = actualAllocationSize;
   ptr += BUFFER_INFO_CHUNK_SIZE;

   // move the pointer to the free memory start
   m_head = allocationStartOffset + actualAllocationSize;

   ++m_commandsCount;

   // return the pointer
   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void ClientServerRoundBuffer::dealloc( void* ptr )
{
   // we'll always be deallocating the chunk the tail points to - pointer specified as the method param
   // only serves the purpose of verifying that the deallocation is correct

   size_t tailOffset = 0;
   size_t tailSize = 0;
   getTail( tailOffset, tailSize ); 

   // verify that the released address is correct
   ASSERT_MSG( ptr == m_memory + tailOffset + BUFFER_INFO_CHUNK_SIZE, "Trying to deallocate an invalid address" );

   m_tail = tailOffset + tailSize;

   --m_commandsCount;
}

///////////////////////////////////////////////////////////////////////////////

ulong ClientServerRoundBuffer::getMemoryUsed() const
{
   ulong headOffset = m_head;
   ulong tailOffset = m_tail;

   ulong occupiedSpace;
   if ( headOffset >= tailOffset )
   {
      occupiedSpace = headOffset - tailOffset;
   }
   else
   {
      occupiedSpace = m_bufSize - ( tailOffset - headOffset );
   }

   return occupiedSpace;
}

///////////////////////////////////////////////////////////////////////////////

void ClientServerRoundBuffer::enable( bool flag )
{
   m_enabled = flag;
   
   if ( flag )
   {
      m_head = m_tail = 0;
      m_commandsCount = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
