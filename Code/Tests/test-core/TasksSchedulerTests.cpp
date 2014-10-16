#include "core-TestFramework\TestFramework.h"
#include "core\MultithreadedTasksScheduler.h"
#include "core\MultithreadedTask.h"
#include "core\Thread.h"
#include "core\CriticalSection.h"
#include "core\math.h"
#include <set>


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   struct ThreadReportingTaskMock : public MultithreadedTask
   {
      DECLARE_ALLOCATOR( ThreadReportingTaskMock, AM_DEFAULT );


      ulong             m_threadId;
      volatile bool*    m_waitBeforeFinish;

      ThreadReportingTaskMock( volatile bool* waitBeforeFinish = NULL )
         : m_waitBeforeFinish( waitBeforeFinish )
      {
         m_threadId = rand();
      }

      void run()
      {
         // simply get the id of the current thread of execution and exit
         m_threadId = Thread::getCurrentThreadId();

         if ( m_waitBeforeFinish )
         {
            while( *m_waitBeforeFinish == false )
            {
               Thread::yield();
            }
         }
      }
   };

   // -------------------------------------------------------------------------

   class TimerMock : public MultithreadedTask
   {
      DECLARE_ALLOCATOR( TimerMock, AM_DEFAULT )

   private:
      uint                  m_timestamp;

      CriticalSection*     m_timestampLock;

   public:
      TimerMock()
         : m_timestamp( 0 )
         , m_timestampLock( new CriticalSection() )
      {}

      ~TimerMock()
      {
         delete m_timestampLock;
         m_timestampLock = NULL;
      }

      void run()
      {
         while( !shouldClose() )
         {
            m_timestampLock->enter();
            ++m_timestamp;
            m_timestampLock->leave();
         }
      }

      uint getTimestamp() const
      {
         m_timestampLock->enter();
         uint val = m_timestamp;
         m_timestampLock->leave();

         return val;
      }
   };

   // -------------------------------------------------------------------------

   struct TimedTaskMock : public MultithreadedTask
   {
      DECLARE_ALLOCATOR( TimedTaskMock, AM_DEFAULT );

      TimerMock&        m_timer;
      Array< uint >     m_startTime;
      Array< uint >     m_endTime;

      bool              m_stopExecution;
      CriticalSection*  m_lock;


      TimedTaskMock( TimerMock& timer )
         : m_timer( timer )
         , m_stopExecution( false )
         , m_lock( new CriticalSection() )
      {
      }

      ~TimedTaskMock()
      {
         delete m_lock;
         m_lock = NULL;
      }

      void lockExecution( bool lockFlag )
      {
         m_lock->enter();
         m_stopExecution = lockFlag;
         m_lock->leave();
      }

      void run()
      {
         m_startTime.push_back( m_timer.getTimestamp() );


         while( true )
         {
            m_lock->enter();
            bool stopExecutionFlag = m_stopExecution;
            m_lock->leave();

            if ( !stopExecutionFlag )
            {
               break;
            }

            Thread::yield();
         }

         // let it run for a little while
         for( uint i = 0; i < 100; ++i )
         {
            Thread::yield();
         }

         m_endTime.push_back( m_timer.getTimestamp() );
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( TasksScheduler, schedulingTasksOnSingleThread )
{
   MultithreadedTasksScheduler scheduler( 1 );

   // create and run tasks
   const int tasksCount = 3;
   ThreadReportingTaskMock* tasks = new ThreadReportingTaskMock[tasksCount];
   bool stopTask = false;

   for ( int i = 0; i < tasksCount; ++i )
   {
      tasks[i].m_waitBeforeFinish = &stopTask;
      scheduler.run( tasks[i] );
   }


   // wait for the tasks to complete
   while( scheduler.getAllTasksCount() > 0 )
   {
      // wait a moment until the task starts running and confirm that there's only one in the queue
      for ( uint i = 0; i < 100; ++i )
      {
         Thread::yield();
      }
      CPPUNIT_ASSERT_EQUAL( (uint)1, scheduler.getRunningTasksCount() );

      // stop the task
      stopTask = true;

      // wait until the task finished
      while( scheduler.getRunningTasksCount() > 0 )
      {
         Thread::yield();
      }
      stopTask = false;

      Thread::yield();
   }

   // check what thread they ran on - they should all run on the same thread
   ulong expectedThreadIdx = tasks[0].m_threadId;
   for ( int i = 1; i < tasksCount; ++i )
   {
      CPPUNIT_ASSERT_EQUAL( expectedThreadIdx, tasks[i].m_threadId );
   }

   // cleanup
   delete [] tasks;
}

///////////////////////////////////////////////////////////////////////////////

TEST( TasksScheduler, schedulingTasksOnMultipleThreads )
{   
   const int tasksCount = 3;

   MultithreadedTasksScheduler scheduler( tasksCount );

   // create and run tasks
   ThreadReportingTaskMock* tasks = new ThreadReportingTaskMock[tasksCount];
   bool stopTasks = false;

   for ( int i = 0; i < tasksCount; ++i )
   {
      tasks[i].m_waitBeforeFinish = &stopTasks;
      scheduler.run( tasks[i] );
   }

   // wait a moment until all tasks run simultaneously and confirm that indeed 3 were scheduled to run
   // at the same time
   for ( uint i = 0; i < 100; ++i )
   {
      Thread::yield();
   }
   CPPUNIT_ASSERT_EQUAL( (uint)3, scheduler.getRunningTasksCount() );

   // wait until the task finished
   stopTasks = true;
   while( scheduler.getRunningTasksCount() > 0 )
   {
      Thread::yield();
   }

   // check what thread they ran on - they should all run on the same thread
   std::set< ulong > threadIds;
   for ( int i = 0; i < tasksCount; ++i )
   {
      ulong id = tasks[i].m_threadId;

      std::set< ulong >::const_iterator iter = threadIds.find( id );
      CPPUNIT_ASSERT( iter == threadIds.end() );

      threadIds.insert( id );
   }

   // cleanup
   delete [] tasks;
}

///////////////////////////////////////////////////////////////////////////////

TEST( TasksScheduler, schedulingMultipleInstancesOfTheSameTask )
{
   MultithreadedTasksScheduler scheduler( 3 );
   bool stopTask = false;

   // schedule the same task twice
   TimerMock timer;
   scheduler.run( timer );

   TimedTaskMock task( timer );
   const int tasksCount = 2;
   for ( int i = 0; i < tasksCount; ++i )
   {
      scheduler.run( task );
   }

   // wait for the tasks to complete
   while( scheduler.getAllTasksCount() > 1 )
   {
      Thread::yield();
   }


   timer.forceClose();
   timer.join();

   // check if the execution went as expected
   CPPUNIT_ASSERT_EQUAL( (uint)2, task.m_startTime.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)2, task.m_endTime.size() );

   CPPUNIT_ASSERT( task.m_startTime[0] < task.m_startTime[1] );
   CPPUNIT_ASSERT( task.m_endTime[0] < task.m_endTime[1] );
   CPPUNIT_ASSERT( task.m_endTime[0] < task.m_startTime[1] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( TasksScheduler, workerThreads )
{
   MultithreadedTasksScheduler scheduler( 1 );

   bool stopTask = false;

   // schedule the same task twice
   TimerMock timer;
   scheduler.run( timer, true );

   TimedTaskMock task1( timer );
   TimedTaskMock task2( timer );
   task1.lockExecution( true );
   task2.lockExecution( true );

   scheduler.run( task1, true );
   scheduler.run( task2, true );

   task1.lockExecution( false );
   task2.lockExecution( false );

   // wait for the tasks to complete
   while( scheduler.getAllTasksCount() > 1 )
   {
      Thread::yield();
   }

   timer.forceClose();
   timer.join();


   // check if the execution went as expected
   CPPUNIT_ASSERT_EQUAL( (uint)1, task1.m_startTime.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, task1.m_endTime.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, task2.m_startTime.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, task2.m_endTime.size() );

   // both tasks should start before the other task ends
   CPPUNIT_ASSERT(  task1.m_startTime[0] < task2.m_endTime[0] );
   CPPUNIT_ASSERT(  task2.m_startTime[0] < task1.m_endTime[0] );
}

///////////////////////////////////////////////////////////////////////////////
