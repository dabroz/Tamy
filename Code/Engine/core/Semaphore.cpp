#include "core\Semaphore.h"
#include "core\Thread.h"
#include "core\Assert.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

Semaphore::Semaphore( uint maxThreadsCount )
{
   m_semaphoreHandle = CreateSemaphore( 
      NULL,             // default security attributes
      maxThreadsCount,  // initial count
      maxThreadsCount,  // maximum count
      NULL);            // unnamed semaphore

   ASSERT_MSG( m_semaphoreHandle != NULL, "Semaphore could not be created" );
}

///////////////////////////////////////////////////////////////////////////////

Semaphore::~Semaphore()
{
   if ( m_semaphoreHandle )
   {
      CloseHandle( m_semaphoreHandle );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Semaphore::tryAcquiring()
{
   ASSERT_MSG( m_semaphoreHandle != NULL, "Semaphore doesn't exist" );

   DWORD waitResult = WaitForSingleObject( m_semaphoreHandle, 0L );
   return ( waitResult == WAIT_OBJECT_0 );
}

///////////////////////////////////////////////////////////////////////////////

void Semaphore::acquire()
{
   ASSERT_MSG( m_semaphoreHandle != NULL, "Semaphore doesn't exist" );

   DWORD waitResult = WaitForSingleObject( m_semaphoreHandle, INFINITE );
   ASSERT_MSG( waitResult == WAIT_OBJECT_0, "Waiting for semaphore has finished incorrectly" );
}

///////////////////////////////////////////////////////////////////////////////

void Semaphore::release()
{
   ASSERT_MSG( m_semaphoreHandle != NULL, "Semaphore doesn't exist" );

   if ( !ReleaseSemaphore( m_semaphoreHandle, 1, NULL ) )
   {
      ASSERT_MSG( false, "Semaphore flag count wasn't decreased" );
   }

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SemaphoredSection::SemaphoredSection( Semaphore& semaphore, bool yieldAfterUnlock )
   : m_semaphore( semaphore )
   , m_yieldAfterUnlock( yieldAfterUnlock )
{
   m_semaphore.acquire();
}

///////////////////////////////////////////////////////////////////////////////

SemaphoredSection::~SemaphoredSection()
{
   m_semaphore.release();

   if ( m_yieldAfterUnlock )
   {
      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////
