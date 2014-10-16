/// @file   core/MultithreadedTask.h
/// @brief  a routine running on a dedicated thread
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class MultithreadedTasksScheduler;
class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * A routine running on a dedicated thread.
 */
class MultithreadedTask
{
   DECLARE_ALLOCATOR( MultithreadedTask, AM_DEFAULT );

public:
   enum Status
   {
      MTS_Inactive,
      MTS_Running,
      MTS_Completed
   };

private:
   volatile bool                 m_forceClose;
   CriticalSection*              m_forceCloseLock;

   CriticalSection*              m_statusLock;
   Status                        m_status;

public:
   virtual ~MultithreadedTask();

   /**
    * Place your multithreaded routine code here.
    */
   virtual void run() = 0;

   /**
    * Blocks the caller thread and waits for the specified 
    */
   void join();

   /**
    * Sets a flag that will inform the task that it should close down immediately.
    * It's up to task's implementation however if it will obey the request.
    */
   void forceClose();

   /**
    * Returns task's status.
    */
   Status getStatus() const;

protected:
   /**
    * Constructor.
    */
   MultithreadedTask();

   /**
    * Tells if the application is expected for the task to close as quickly as possible.
    */
   bool shouldClose() const;

private:
   // ---------------------------------------------------------------------------------
   // MultithreadedTasksScheduler API
   // ---------------------------------------------------------------------------------
   friend class MultithreadedTasksScheduler;

   /**
    * Sets a new task status
    */
   void setStatus( Status status );
};

///////////////////////////////////////////////////////////////////////////////
