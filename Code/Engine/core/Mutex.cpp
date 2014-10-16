#include "core\Mutex.h"
#include "core\Assert.h"
#include "core\Thread.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
   m_mutexHandle = CreateMutex( 
      NULL,              // default security attributes
      FALSE,             // initially not owned
      NULL ); 

   ASSERT_MSG( m_mutexHandle != NULL, "Mutex could not be created" );
}

///////////////////////////////////////////////////////////////////////////////

Mutex::~Mutex()
{
   if ( m_mutexHandle )
   {
      CloseHandle( m_mutexHandle );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Mutex::lock()
{
   ASSERT_MSG( m_mutexHandle != NULL, "Mutex doesn't exist" );

   DWORD waitResult = WaitForSingleObject( m_mutexHandle, INFINITE );
   ASSERT_MSG( waitResult == WAIT_OBJECT_0, "Waiting for mutex has finished incorrectly" );
}

///////////////////////////////////////////////////////////////////////////////

void Mutex::unlock()
{
   ASSERT_MSG( m_mutexHandle != NULL, "Mutex doesn't exist" );

   if ( !ReleaseMutex( m_mutexHandle ) )
   {
      ASSERT_MSG( false, "Ownership of the mutex wasn't released" );
   }

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MutexedSection::MutexedSection( Mutex& mutex, bool yieldAfterUnlock )
   : m_mutex( mutex )
   , m_yieldAfterUnlock( yieldAfterUnlock )
{
   m_mutex.lock();
}

///////////////////////////////////////////////////////////////////////////////

MutexedSection::~MutexedSection()
{
   m_mutex.unlock();

   if ( m_yieldAfterUnlock )
   {
      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////
