#ifndef _LOCKLESS_COMMANDS_QUEUE_H
#error "This file can only be included in LocklessCommandsQueue.h"
#else

#include "core\ClientServerRoundBuffer.h"
#include "core\LocklessCommandFence.h"
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
LocklessCommandsQueue< Owner >::LocklessCommandsQueue( size_t queueSize )
   : m_commandsQueue( new ClientServerRoundBuffer( queueSize ) )
   , m_fenceLock( new CriticalSection() )
   , m_fenceCommand( NULL )
{
   m_fenceCommand = new ( m_commandsQueue ) LocklessCommandFence< Owner >( m_fenceLock, m_fenceCommand );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
LocklessCommandsQueue< Owner >::~LocklessCommandsQueue()
{
   m_fenceCommand = NULL;

   delete m_commandsQueue;
   m_commandsQueue = NULL;

   delete m_fenceLock;
   m_fenceLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::processingLoop( Owner& owner )
{
   LocklessCommand< Owner >* c = NULL;
   while ( c = m_commandsQueue->front< LocklessCommand< Owner > >() )
   {
      c->execute( owner );
      delete c;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::flush()
{
   m_commandsQueue->enable( false );
   m_fenceCommand->proceed();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::commit()
{
   m_fenceCommand = new ( m_commandsQueue ) LocklessCommandFence< Owner >( m_fenceLock, m_fenceCommand );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void* LocklessCommandsQueue< Owner >::alloc( size_t size )
{
   return m_commandsQueue->alloc( size );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::dealloc( void* ptr )
{
   m_commandsQueue->dealloc( ptr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
ulong LocklessCommandsQueue< Owner >::getMemoryUsed() const
{
   return m_commandsQueue->getMemoryUsed();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
uint LocklessCommandsQueue< Owner >::getCommandsCount() const
{
   return m_commandsQueue->getCommandsCount();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::setWriterThreadId( ulong threadId )
{
   m_commandsQueue->setWriterThreadId( threadId );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandsQueue< Owner >::setReaderThreadId( ulong threadId )
{
   m_commandsQueue->setReaderThreadId( threadId );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCKLESS_COMMANDS_QUEUE_H
