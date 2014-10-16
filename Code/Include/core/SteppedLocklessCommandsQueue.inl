#ifndef _STEPPED_LOCKLESS_COMMANDS_QUEUE_H
#error "This file can only be included in SteppedLocklessCommandsQueue.h"
#else

#include "core\ClientServerRoundBuffer.h"
#include "core\LocklessCommandFence.h"
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
SteppedLocklessCommandsQueue< Owner >::SteppedLocklessCommandsQueue( size_t queueSize )
   : m_commandsQueue( new ClientServerRoundBuffer( queueSize ) )
   , m_fenceLock( new CriticalSection() )
   , m_fenceCommand( NULL )
   , m_fenceReachedFlag( false )
{
   m_fenceCommand = new ( m_commandsQueue ) SteppedLocklessCommandFence< Owner >( m_fenceLock, m_fenceCommand, m_fenceReachedFlag );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
SteppedLocklessCommandsQueue< Owner >::~SteppedLocklessCommandsQueue()
{
   m_fenceCommand = NULL;

   delete m_commandsQueue;
   m_commandsQueue = NULL;

   delete m_fenceLock;
   m_fenceLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::process( Owner& owner )
{
   m_fenceReachedFlag = false;

   LocklessCommand< Owner >* c = NULL;
   while ( c = m_commandsQueue->front< LocklessCommand< Owner > >() )
   {
      c->execute( owner );

      if ( m_fenceReachedFlag )
      {
         break;
      }

      delete c;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::flush()
{
   m_commandsQueue->enable( false );
   m_fenceCommand->proceed();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::commit()
{
   m_fenceCommand = new ( m_commandsQueue ) SteppedLocklessCommandFence< Owner >( m_fenceLock, m_fenceCommand, m_fenceReachedFlag );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void* SteppedLocklessCommandsQueue< Owner >::alloc( size_t size )
{
   return m_commandsQueue->alloc( size );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::dealloc( void* ptr )
{
   m_commandsQueue->dealloc( ptr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
ulong SteppedLocklessCommandsQueue< Owner >::getMemoryUsed() const
{
   return m_commandsQueue->getMemoryUsed();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
uint SteppedLocklessCommandsQueue< Owner >::getCommandsCount() const
{
   return m_commandsQueue->getCommandsCount();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::setWriterThreadId( ulong threadId )
{
   m_commandsQueue->setWriterThreadId( threadId );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandsQueue< Owner >::setReaderThreadId( ulong threadId )
{
   m_commandsQueue->setReaderThreadId( threadId );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _STEPPED_LOCKLESS_COMMANDS_QUEUE_H
