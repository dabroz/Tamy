/// @file   core/ClientServerRoundBuffer.h
/// @brief  a circular buffer for storing render commands
#ifndef _CLIENT_SERVER_ROUND_BUFFER_H
#define _CLIENT_SERVER_ROUND_BUFFER_H

#include "core\MemoryAllocator.h"
#include "core\MemoryRouter.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A circular buffer that can safely be used to pass data between two threads.
 *
 * The general idea is that the server thread keeps pushing new data to one
 * end of the buffer, and the client ( consumer ) thread reads them sequentially
 * from the other end.
 *
 * Because this allocator is designed to work with two threads referencing it,
 * we need to clearly separate the thread that can access one end of the buffer
 * from another - so that we know the two won't accidentally overwrite themselves.
 * Because of that, there are two requirements the user of this buffer needs to fulfill:
 *
 *   1. The whole concept works well provided that there's at least one piece of data
 *      separating the two ends - that's up to the user to provide.
 *
 *   2. Also - there's no checking if we have enough room for a new allocation - it's the caller's
 *      responsibility to ensure that the tail never catches up with the tail.
 */
class ClientServerRoundBuffer : public MemoryAllocator
{
   DECLARE_ALLOCATOR( ClientServerRoundBuffer, AM_DEFAULT );

private:
   static size_t        BUFFER_INFO_CHUNK_SIZE;
   static uint          MAX_TIMESTAMP;

   size_t               m_bufSize;
   char*                m_memory;

   ulong                m_readerThreadId;
   ulong                m_writerThreadId;

   volatile bool        m_enabled;

   volatile size_t      m_head;
   volatile size_t      m_tail;
   volatile uint        m_commandsCount;

public:
   /**
    * Constructor.
    *
    * @param size          buffer size
    */
   ClientServerRoundBuffer( size_t size );
   ~ClientServerRoundBuffer();

   /**
    * Returns the front element.
    */
   template< typename T >
   T* front();

   /**
    * Enables or disables the buffer.
    * 
    * When ENABLING the queue, you need to make sure that only one thread operates on it at the moment.
    * This operation will reset the head and tail - thus the single-threadedness requirement.
    *
    * DISABLING the queue is designed as a multithreaded operation.
    */
   void enable( bool flag );

   /**
    * Returns the number of commands. Not very reliable - use it only for testing.
    */
   inline uint getCommandsCount() const { return m_commandsCount; }

   // -------------------------------------------------------------------------
   // Debug
   // -------------------------------------------------------------------------

   /**
    * Sets the id of a thread that will write to this buffer.
    *
    * @param threadId
    */
   void setWriterThreadId( ulong threadId );

   /**
    * Sets the id of a thread that will read from this buffer.
    *
    * @param threadId
    */
   void setReaderThreadId( ulong threadId );

   // -------------------------------------------------------------------------
   // MemoryAllocator implementation
   // -------------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const;

private:
   inline void getTail( size_t& outOffset, size_t& outSize );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ClientServerRoundBuffer.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _CLIENT_SERVER_ROUND_BUFFER_H
