#include "core\ThreadSystem.h"
#include "core\Singleton.h"
#include "core\CriticalSection.h"
#include "core\Thread.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( ThreadSystem );

///////////////////////////////////////////////////////////////////////////////

ThreadSystem::ThreadSystem( const SingletonConstruct& )
   : m_threadsLock( new CriticalSection() )
   , m_mainThread( new Thread() )
{
   m_mainThreadId = GetCurrentThreadId();
   m_mainThread->m_threadId = m_mainThreadId;

   m_threads.push_back( new ThreadEntry( m_mainThreadId, m_mainThread ) );
}

///////////////////////////////////////////////////////////////////////////////

ThreadSystem::~ThreadSystem()
{
   m_threadsLock->enter();
   ASSERT_MSG( m_threads.size() == 1 && m_threads[0]->m_thread == m_mainThread, "Some threads are still running" );
   m_threadsLock->leave();

   delete m_mainThread;
   m_mainThread = NULL;

   delete m_threadsLock;
   m_threadsLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool ThreadSystem::isMainThread() const
{
   DWORD threadId = GetCurrentThreadId();
   return threadId == m_mainThreadId;
}

///////////////////////////////////////////////////////////////////////////////

uint ThreadSystem::getCoresCount() const
{
   return 8;
}


///////////////////////////////////////////////////////////////////////////////

Thread& ThreadSystem::getCurrentThread()
{
   Thread* thread = NULL;
   DWORD threadId = GetCurrentThreadId();


   m_threadsLock->enter();

   uint count = m_threads.size();
   for ( uint i = 0; i < count; ++i )
   {
      ThreadEntry* entry = m_threads[i];
      if ( entry->m_threadId == threadId )
      {
         thread = entry->m_thread;
         break;
      }
   }

   m_threadsLock->leave();

   ASSERT_MSG( thread != NULL, "No dedicated Thread instance is registered" );
   return *thread;
}

///////////////////////////////////////////////////////////////////////////////

// @DedicatedThreadOfExecution
void ThreadSystem::registerThread( Thread& thread )
{
   DWORD threadId = GetCurrentThreadId();

   m_threadsLock->enter();
   m_threads.push_back( new ThreadEntry( threadId, &thread ) );
   m_threadsLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

// @DedicatedThreadOfExecution
void ThreadSystem::unregisterThread()
{
   DWORD threadId = GetCurrentThreadId();

   m_threadsLock->enter();

   uint count = m_threads.size();
   for ( uint i = 0; i < count; ++i )
   {
      ThreadEntry* entry = m_threads[i];
      if ( entry->m_threadId == threadId )
      {
         // found it
         delete entry;
         m_threads.remove( i );

         m_threadsLock->leave();
         return;
      }
   }

   ASSERT_MSG( false, "The specified thread wasn't registered in the first place" );
   m_threadsLock->leave();
}

///////////////////////////////////////////////////////////////////////////////
