/// @file   core/SteppedLocklessCommandsQueue.h
/// @brief  a commands queue for issuing commands from one thread to another
#ifndef _STEPPED_LOCKLESS_COMMANDS_QUEUE_H
#define _STEPPED_LOCKLESS_COMMANDS_QUEUE_H

#include "core\MemoryRouter.h"
#include "core\MemoryAllocator.h"
#include "core\SteppedLocklessCommandFence.h"


///////////////////////////////////////////////////////////////////////////////

class ClientServerRoundBuffer;
class CriticalSection;
class CommandWaitForFrame;

///////////////////////////////////////////////////////////////////////////////

/**
 * 
 */
template< typename Owner >
class SteppedLocklessCommandsQueue : public MemoryAllocator
{
   DECLARE_ALLOCATOR( SteppedLocklessCommandsQueue, AM_DEFAULT );

private:
   ClientServerRoundBuffer*                  m_commandsQueue;
   CriticalSection*                          m_fenceLock;
   bool                                      m_fenceReachedFlag;
   SteppedLocklessCommandFence< Owner >*     m_fenceCommand;

public:
   /**
    * Constructor.
    *
    * @param queueSize
    */
   SteppedLocklessCommandsQueue( size_t queueSize );
   ~SteppedLocklessCommandsQueue();

   /**
    * Processes a batch of commands aggregated in the buffer.
    *
    * @param owner
    */
   void process( Owner& owner );

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

#include "core\SteppedLocklessCommandsQueue.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _STEPPED_LOCKLESS_COMMANDS_QUEUE_H
