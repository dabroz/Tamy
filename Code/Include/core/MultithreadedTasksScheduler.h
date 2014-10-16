/// @file   core/MultithreadedTasksScheduler.h
/// @brief  Scheduler of methods executed on separate threads ( so called tasks )
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Array.h"
#include "core\Dequeue.h"
#include "core\Runnable.h"


///////////////////////////////////////////////////////////////////////////////

class MultithreadedTask;
class CriticalSection;
class Thread;
struct SingletonConstruct;

///////////////////////////////////////////////////////////////////////////////

/**
 * Scheduler of methods executed on separate threads ( so called tasks ).
 */
class MultithreadedTasksScheduler : public Runnable
{
   REGULAR_SINGLETON();
   DECLARE_ALLOCATOR( MultithreadedTasksScheduler, AM_DEFAULT );

private:
   class ThreadContext : public Runnable
   {
      DECLARE_ALLOCATOR( ThreadContext, AM_DEFAULT );

   private:
      Thread*                          m_thread;
      MultithreadedTasksScheduler*     m_scheduler;
      uint                             m_threadIdx;

      CriticalSection*                 m_taskLock;
      MultithreadedTask*               m_task;
      volatile bool                    m_newTaskAvailable;
      volatile bool                    m_forceClose;

   public:
      /**
       * Constructor.
       */
      ThreadContext( MultithreadedTasksScheduler* scheduler, int threadIdx );
      ~ThreadContext();

      /**
       * Checks if the context is processing a task at the moment.
       */
      bool isProcessing() const;

      /**
       * Assigns a task to be executed by this context.
       *
       * @param task
       */
      void assignTask( MultithreadedTask* task );

      /**
       * Stops the running task.
       */
      void stopTask();

      // ----------------------------------------------------------------------
      // Runnable implementation
      // ----------------------------------------------------------------------
      void run();
   };

private:
   uint                                m_fixedThreadsCount;

   volatile bool                       m_closeRequest;
   Thread*                             m_schedulerThread;

   typedef List< MultithreadedTask* >  TasksQueue;
   TasksQueue                          m_tasksQueue;

   CriticalSection*                    m_runningTasksLock;

   Array< ThreadContext* >             m_threadContexts;
   Dequeue< uint >*                    m_freeThreadIds;

public:
   /**
    * Constructor.
    *
    * @param numThreads       how many threads can the scheduler distribute tasks amongst
    */
   MultithreadedTasksScheduler( uint numThreads );

   /**
    * Singleton compatible constructor.
    */
   MultithreadedTasksScheduler( const SingletonConstruct& );
   ~MultithreadedTasksScheduler();

   /**
    * Schedules a task to be executed on a worker thread.
    *
    * If 'immediateExecution' is set to 'false', the task will be scheduled to one of
    * the worker threads and will be executed as soon as it frees up.
    * Otherwise, if there are no free worker threads left, a new one
    * will be created specifically for this task.
    *
    * @param task
    * @param immediateExecution
    */
   void run( MultithreadedTask& task, bool immediateExecution = false );

   /**
    * Returns number of scheduled and running tasks.
    */
   uint getAllTasksCount() const;

   /**
    * Returns number of scheduled tasks.
    */
   uint getScheduledTasksCount() const;

   /**
    * Returns number of tasks currently in progress.
    */
   uint getRunningTasksCount() const;

   /**
    * Removes all currently running and scheduled tasks
    */
   void clear();

   // -------------------------------------------------------------------------
   // Channel of communication with ThreadContext
   // -------------------------------------------------------------------------
   /**
    * Called by a task to report that it's completed.
    *
    * @param threadIdx
    */
   void reportTaskCompleted( uint threadIdx );

   // -------------------------------------------------------------------------
   // Runnable implementation
   // -------------------------------------------------------------------------
   void run();

private:
   void init( uint numThreads );
   void scheduleTasks();
   void scheduleImmediateTask( MultithreadedTask& task );
};

///////////////////////////////////////////////////////////////////////////////
