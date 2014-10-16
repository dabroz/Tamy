#ifndef _CLIENT_SERVER_ROUND_BUFFER_H
#error "This file can only be included in ClientServerRoundBuffer.h"
#else

///////////////////////////////////////////////////////////////////////////////

// optimized version of MemoryRouter::calcAlignedAddr
#define ALIGN_ADDRESS( addr ) (char*)( ( (uint)(addr) + 23 ) & 0xfffffff0 );

///////////////////////////////////////////////////////////////////////////////

void ClientServerRoundBuffer::getTail( size_t& outOffset, size_t& outSize )
{
#ifdef _DEBUG_ROUNDBUFFER
   ThreadSystem& ts = TSingleton< ThreadSystem >::getInstance();
   if ( m_writerThreadId > 0 && ts.getCurrentThread().getThreadId() != m_readerThreadId )
   {
      ASSERT_MSG( false, "Only a thread designated as the reader thread can read data from this buffer" );
   }
#endif

   size_t spaceTillEnd = m_bufSize - m_tail;
   if ( spaceTillEnd <= BUFFER_INFO_CHUNK_SIZE )
   {
      outOffset = 0;

      outSize = *( size_t* ) m_memory;
   }
   else
   {
      outSize = *( size_t* ) ( m_memory + m_tail );
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

template< typename T >
T* ClientServerRoundBuffer::front()
{
   bool isEnabled = m_enabled;
   size_t tailOffset = 0;
   size_t tailSize = 0;

   if ( !isEnabled && m_head == m_tail )
   {
      // this is the end of the buffer
      return NULL;
   }
   else
   {
      getTail( tailOffset, tailSize );
   }

   // calculate and align the address
   char* startAddr = ALIGN_ADDRESS( m_memory + tailOffset + BUFFER_INFO_CHUNK_SIZE );

   return reinterpret_cast< T* >( startAddr );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _CLIENT_SERVER_ROUND_BUFFER_H
