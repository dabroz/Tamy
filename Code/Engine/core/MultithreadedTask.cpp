#include "core\MultithreadedTask.h"
#include "core\CriticalSection.h"
#include "core\Thread.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

MultithreadedTask::MultithreadedTask()
   : m_forceClose( false )
   , m_forceCloseLock( new CriticalSection() )
   , m_statusLock( new CriticalSection() )
   , m_status( MTS_Inactive )
{
}

///////////////////////////////////////////////////////////////////////////////

MultithreadedTask::~MultithreadedTask()
{
   delete m_forceCloseLock;
   m_forceCloseLock = NULL;

   delete m_statusLock;
   m_statusLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTask::forceClose()
{
   m_forceCloseLock->enter();
   m_forceClose = true;
   m_forceCloseLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTask::setStatus( Status status )
{
   m_statusLock->enter();
   m_status = status;
   m_statusLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

MultithreadedTask::Status MultithreadedTask::getStatus() const
{
   m_statusLock->enter();
   MultithreadedTask::Status status = m_status;
   m_statusLock->leave();

   return status;
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTask::join()
{
   while( true )
   {
      m_statusLock->enter();
      MultithreadedTask::Status status = m_status;
      m_statusLock->leave();

      if ( status == MTS_Completed )
      {
         return;
      }

      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////

bool MultithreadedTask::shouldClose() const
{
   m_forceCloseLock->enter();
   bool val = m_forceClose;
   m_forceCloseLock->leave();

   return val;
}

///////////////////////////////////////////////////////////////////////////////
