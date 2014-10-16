#include "core\Thread.h"
#include "core\Runnable.h"
#include "core\TSFragmentedMemoryPool.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

Thread::Thread( uint stackSize )
   : m_sharedMemoryPool( new TSFragmentedMemoryPool( 2048, 128 ) )
   , m_stackSize( stackSize )
   , m_threadHandle( NULL )
   , m_threadId( 0 )
   , m_serializationInProgress( false )
{
}

///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{
   join();

   delete m_sharedMemoryPool;
   m_sharedMemoryPool = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool Thread::start( Runnable& runnable )
{
   runnable.execute( this, m_stackSize, m_threadId, m_threadHandle );
   return m_threadHandle != NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Thread::join()
{
   if ( m_threadHandle )
   {
      // wait until the thread completes
      WaitForSingleObject( m_threadHandle, INFINITE );

      // release it
      CloseHandle( m_threadHandle );

      m_threadHandle = NULL;
      m_threadId = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Thread::yield()
{
   Sleep( 1 );
}

///////////////////////////////////////////////////////////////////////////////

void Thread::sleep( uint miliseconds )
{
   Sleep( miliseconds );
}

///////////////////////////////////////////////////////////////////////////////

ulong Thread::getCurrentThreadId()
{
   return GetCurrentThreadId();
}

///////////////////////////////////////////////////////////////////////////////

bool Thread::isCurrentThread() const
{
   return m_threadId == GetCurrentThreadId();
}

///////////////////////////////////////////////////////////////////////////////
