/// @file   core/LocklessCommandsQueue.h
/// @brief  a commands queue for issuing commands from one thread to another
#ifndef _LOCKLESS_COMMANDS_QUEUE_H
#define _LOCKLESS_COMMANDS_QUEUE_H

#include "core\MemoryRouter.h"
#include "core\MemoryAllocator.h"
#include "core\LocklessCommandFence.h"


///////////////////////////////////////////////////////////////////////////////

class ClientServerRoundBuffer;
class CriticalSection;
class CommandWaitForFrame;

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
class LocklessCommandsQueue : public MemoryAllocator
{
   DECLARE_ALLOCATOR( LocklessCommandsQueue, AM_DEFAULT );

private:
   ClientServerRoundBuffer*         m_commandsQueue;
   CriticalSection*                 m_fenceLock;
   LocklessCommandFence< Owner >*   m_fenceCommand;

public:
   /**
    * Constructor.
    *
    * @param queueSize
    */
   LocklessCommandsQueue( size_t queueSize );
   ~LocklessCommandsQueue();

   /**
    * Starts a command processing loop that's gonna run until the queue gets flushed.
    *
    * @param owner
    */
   void processingLoop( Owner& owner );

   /**
    * Flushes the queue. 
    * Call it if you intend to destroy the queue in order to get all of its commands processed.
    */
   void flush();

   /**
    * Commits submitted commands for processing.
    */
   void commit();

   /**
    * Returns number of commands in the queue.
    */
   uint getCommandsCount() const;

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
};

///////////////////////////////////////////////////////////////////////////////

#include "core\LocklessCommandsQueue.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCKLESS_COMMANDS_QUEUE_H
