#include "core\MultithreadedTasksScheduler.h"
#include "core\MultithreadedTask.h"
#include "core\Thread.h"
#include "core\CriticalSection.h"
#include "core\Singleton.h"
#include "core\ThreadSystem.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( MultithreadedTasksScheduler );

///////////////////////////////////////////////////////////////////////////////

MultithreadedTasksScheduler::MultithreadedTasksScheduler( uint numThreads )
{
   init( numThreads );
}

///////////////////////////////////////////////////////////////////////////////

MultithreadedTasksScheduler::MultithreadedTasksScheduler( const SingletonConstruct& )
{
   uint numThreads = TSingleton< ThreadSystem >::getInstance().getCoresCount();
   init( numThreads );
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::init( uint numThreads )
{
   m_runningTasksLock = new CriticalSection();
   m_closeRequest = false;
   m_schedulerThread = new Thread();
   
   m_fixedThreadsCount = numThreads;
   m_freeThreadIds = new Dequeue< uint >( numThreads );
   m_threadContexts.resize( numThreads );
   for ( uint i = 0; i < numThreads; ++i )
   {
      ThreadContext* context = new ThreadContext( this, i );
      m_threadContexts[i] = context;
      m_freeThreadIds->push( i );
   }

   // start the scheduler thread
   m_schedulerThread->start( *this );
}

///////////////////////////////////////////////////////////////////////////////

MultithreadedTasksScheduler::~MultithreadedTasksScheduler()
{
   // stop the scheduler thread
   m_closeRequest = true;
   m_schedulerThread->join();
   delete m_schedulerThread;
   m_schedulerThread = NULL;

   // join and delete threads
   uint count = m_threadContexts.size();
   for ( uint i = 0; i < count; ++i )
   {
      ThreadContext* context = m_threadContexts[i];      
      delete context;
   }

   delete m_freeThreadIds;
   m_freeThreadIds = NULL;

   delete m_runningTasksLock;
   m_runningTasksLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::clear()
{
   // clear tasks queue
   m_tasksQueue.clear();

   // close all running tasks
   uint numThreads = m_threadContexts.size();
   for ( uint i = 0; i < numThreads; ++i )
   {
      ThreadContext* context = m_threadContexts[i]; 
      context->stopTask();
   }
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::run( MultithreadedTask& task, bool immediateExecution )
{
   if ( immediateExecution )
   {
      scheduleImmediateTask( task );
   }
   else
   {
      m_tasksQueue.pushBack( &task );
   }
}

///////////////////////////////////////////////////////////////////////////////

uint MultithreadedTasksScheduler::getAllTasksCount() const
{
   CriticalSectionedSection lock( *m_runningTasksLock, false );
   uint activeTasksCount = m_threadContexts.size() - m_freeThreadIds->size();

   return m_tasksQueue.size() + activeTasksCount;
}

///////////////////////////////////////////////////////////////////////////////

uint MultithreadedTasksScheduler::getScheduledTasksCount() const
{
   return m_tasksQueue.size();
}

///////////////////////////////////////////////////////////////////////////////

uint MultithreadedTasksScheduler::getRunningTasksCount() const
{
   CriticalSectionedSection lock( *m_runningTasksLock, false );
   uint activeTasksCount = m_threadContexts.size() - m_freeThreadIds->size();

   return activeTasksCount;
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::run()
{
   while ( !m_closeRequest )
   {
      scheduleTasks();
   }
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::scheduleTasks()
{
   CriticalSectionedSection lock( *m_runningTasksLock, false );

   // Activate new tasks
   int taskIdx = 0;
   for ( TasksQueue::iterator it = m_tasksQueue.begin(); !it.isEnd() && !m_freeThreadIds->empty(); )
   {
      MultithreadedTask* scheduledTask = *it;

      // The user might have scheduled the same task twice in a row - same instance that is.
      // But if we're scheduling on multiple threads, we can't allow for the same instance to be 
      // running on two threads at the same time.
      if ( scheduledTask->getStatus() == MultithreadedTask::MTS_Running )
      {
         ++taskIdx;
         ++it;
         continue;
      }

      // get the index of the next free thread
      uint threadIdx = m_freeThreadIds->pop();

      // start the task
      m_threadContexts[threadIdx]->assignTask( scheduledTask );

      // remove the task from scheduled tasks queue
      {
         TasksQueue::iterator removedTaskIt = it;

         if ( taskIdx > 0 )
         {
            ++it;
            m_tasksQueue.remove( removedTaskIt );
         }
         else
         {
            m_tasksQueue.remove( removedTaskIt );
            it = m_tasksQueue.begin();
         }
      }
   }


   // claim unused temporary contexts back
   int threadsCount = m_threadContexts.size();
   for ( int i = threadsCount - 1; i >= (int)m_fixedThreadsCount; --i )
   {
      ThreadContext* context = m_threadContexts[i];
      if ( !context->isProcessing() )
      {
         delete m_threadContexts[i];
         m_threadContexts.remove(i);
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::scheduleImmediateTask( MultithreadedTask& task )
{
   CriticalSectionedSection lock( *m_runningTasksLock, false );

   // acquire an execution context for the task
   uint threadIdx = -1;
   if ( m_freeThreadIds->empty() )
   {
      // there are no free threads to run the task on - create a new one
      threadIdx = m_threadContexts.size();
      m_threadContexts.push_back( new ThreadContext( this, threadIdx ) );
   }
   else
   {
      threadIdx = m_freeThreadIds->pop();
   }

   // run the task
   ThreadContext* context = m_threadContexts[threadIdx];
   context->assignTask( &task );
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::reportTaskCompleted( uint threadIdx )
{
   if ( threadIdx < m_fixedThreadsCount )
   {
      // return the thread to the pool of free threads only if it wasn't a temporary
      m_runningTasksLock->enter();
      m_freeThreadIds->push( threadIdx );
      m_runningTasksLock->leave();
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MultithreadedTasksScheduler::ThreadContext::ThreadContext( MultithreadedTasksScheduler* scheduler, int threadIdx )
   : m_thread( new Thread() )
   , m_scheduler( scheduler )
   , m_threadIdx( threadIdx )
   , m_taskLock( new CriticalSection() )
   , m_task( NULL )
   , m_newTaskAvailable( false )
   , m_forceClose( false )
{
   m_thread->start( *this );
}

///////////////////////////////////////////////////////////////////////////////

MultithreadedTasksScheduler::ThreadContext::~ThreadContext()
{
   // signal the thread that it should stop working and wait for the thread to finish
   m_taskLock->enter();
   if ( m_task )
   {
      m_task->forceClose();
   }
   m_taskLock->leave();

   m_forceClose = true;
   m_thread->join();

   m_scheduler = NULL;
   m_threadIdx = 0;

   delete m_taskLock;
   m_taskLock = NULL;

   delete m_thread;
   m_thread = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::ThreadContext::assignTask( MultithreadedTask* task )
{
   m_taskLock->enter();

   ASSERT_MSG( m_task == NULL && !m_newTaskAvailable, "Another task is currently running in this context" );

   m_task = task;
   m_task->setStatus( MultithreadedTask::MTS_Running );
   m_newTaskAvailable = true;

   m_taskLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::ThreadContext::run()
{
   while( !m_forceClose )
   {      
      // NOTE: we're not locking task access, because we're only performing atomic read operations
      // whenever a task might be active and running, and we're assigning a new task when we the old
      // one has reported completion and was safely removed from the queue

      // wait for a task to become available
      while ( !m_newTaskAvailable && !m_forceClose )
      {
         Thread::yield();
      }

      if ( m_forceClose )
      {
         // stop execution
         break;
      }

      // another task was scheduled for execution - run it
      m_taskLock->enter();
      MultithreadedTask* task = m_task; 
      m_taskLock->leave();

      if ( task )
      {
         task->run();
      }

      m_taskLock->enter();
      m_newTaskAvailable = false;
      m_task = NULL;
      m_taskLock->leave();

      task->setStatus( MultithreadedTask::MTS_Completed );

      // report back to the scheduler that the task has completed ( only if we're not stopping the entire context )
      if ( !m_forceClose )
      {
         m_scheduler->reportTaskCompleted( m_threadIdx );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void MultithreadedTasksScheduler::ThreadContext::stopTask()
{
   m_taskLock->enter();

   if ( !m_task )
   {
      m_taskLock->leave();
      return;
   }

   m_task->forceClose();
   m_taskLock->leave();

   // wait for the task to complete
   while ( true )
   {
      m_taskLock->enter();
      bool isStopped = ( m_task == NULL );
      m_taskLock->leave();

      if ( isStopped )
      {
         return;
      }

      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////

bool MultithreadedTasksScheduler::ThreadContext::isProcessing() const
{
   m_taskLock->enter();
   bool isRunning = ( m_task != NULL );
   m_taskLock->leave();

   return isRunning;
}

///////////////////////////////////////////////////////////////////////////////

