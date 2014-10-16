#include "core-TestFramework\TestFramework.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core\Runnable.h"
#include "core\Mutex.h"
#include "core\Semaphore.h"
#include "core\CriticalSection.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   struct TaskMock : public Runnable
   {
      int   m_a, m_b, m_result;   
   
      TaskMock( int a, int b )
         : m_a( a )
         , m_b( b )
         , m_result( 0 )
      {}

      void run()
      {
         m_result = m_a + m_b;
      }
   };

   // -------------------------------------------------------------------------

   struct MutexedTaskMock : public Runnable
   {
      static Mutex            s_mutex;

      std::vector< int >*     m_sequence;
      int                     m_taskIdx;
      bool*                   m_activate;

      MutexedTaskMock()
         : m_sequence( NULL )
         , m_taskIdx( -1 )
         , m_activate( NULL )
      {
      }

      void init( std::vector< int >& sequence, int taskIdx, bool& activate )
      {
         m_sequence = &sequence;
         m_taskIdx = taskIdx;
         m_activate = &activate;
      }

      void run()
      {
         while( m_taskIdx >=0 )
         {
            MutexedSection section( s_mutex, true );

            if ( !m_activate )
            {
               continue;
            }

            if ( m_sequence->size() != m_taskIdx )
            {
               continue;
            }

            m_sequence->push_back( m_taskIdx );
            m_taskIdx = -1;
         }

      }
   };

   Mutex MutexedTaskMock::s_mutex;

   // -------------------------------------------------------------------------

   struct SemaphoredTaskMock : public Runnable
   {
      static Semaphore        s_semaphore;

      std::vector< int >*     m_sequence;
      int                     m_taskIdx;
      bool*                   m_activate;

      SemaphoredTaskMock()
         : m_sequence( NULL )
         , m_taskIdx( -1 )
         , m_activate( NULL )
      {
      }

      void init( std::vector< int >& sequence, int taskIdx, bool& activate )
      {
         m_sequence = &sequence;
         m_taskIdx = taskIdx;
         m_activate = &activate;
      }

      void run()
      {
         while( m_taskIdx >=0 )
         {
            SemaphoredSection section( s_semaphore, true );

            if ( !m_activate )
            {
               continue;
            }

            if ( m_sequence->size() != m_taskIdx )
            {
               continue;
            }

            m_sequence->push_back( m_taskIdx );
            m_taskIdx = -1;
         }

      }
   };

   Semaphore SemaphoredTaskMock::s_semaphore( 1 );

   // -------------------------------------------------------------------------

   struct CriticalSectionedTaskMock : public Runnable
   {
      static CriticalSection  s_criticalSection;

      std::vector< int >*     m_sequence;
      int                     m_taskIdx;
      bool*                   m_activate;

      CriticalSectionedTaskMock()
         : m_sequence( NULL )
         , m_taskIdx( -1 )
         , m_activate( NULL )
      {
      }

      void init( std::vector< int >& sequence, int taskIdx, bool& activate )
      {
         m_sequence = &sequence;
         m_taskIdx = taskIdx;
         m_activate = &activate;
      }

      void run()
      {
         while( m_taskIdx >=0 )
         {
            CriticalSectionedSection section( s_criticalSection, true );

            if ( !m_activate )
            {
               continue;
            }

            if ( m_sequence->size() != m_taskIdx )
            {
               continue;
            }

            m_sequence->push_back( m_taskIdx );
            m_taskIdx = -1;
         }

      }
   };

   CriticalSection CriticalSectionedTaskMock::s_criticalSection;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Thread, runningFunctionOnThread )
{
   TaskMock task( 1, 2 );

   Thread thread;
   bool result = thread.start( task );
   thread.join();

   CPPUNIT_ASSERT( result );
   CPPUNIT_ASSERT_EQUAL( 3, task.m_result );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Mutex, exclusiveAccessToResource )
{
   std::vector< int > sequence;
   bool activate = false;

   Thread threads[3];
   MutexedTaskMock tasks[3];
   int randomOrder[] = { 2, 0, 1 };

   for ( int i = 0; i < 3; ++i )
   {
      int taskIdx = randomOrder[i];
      tasks[taskIdx].init( sequence, taskIdx, activate );
      bool wasStarted = threads[taskIdx].start( tasks[taskIdx] );
      CPPUNIT_ASSERT( wasStarted );
   }

   // activate all tasks
   activate = true;

   // wait for them to complete
   for ( int i = 0; i < 3; ++i )
   {
      threads[i].join();
   }

   // check the results
   CPPUNIT_ASSERT_EQUAL( (uint)3, sequence.size() );
   CPPUNIT_ASSERT_EQUAL( 0, sequence[0] );
   CPPUNIT_ASSERT_EQUAL( 1, sequence[1] );
   CPPUNIT_ASSERT_EQUAL( 2, sequence[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Semaphore, exclusiveAccessToResource )
{
   std::vector< int > sequence;
   bool activate = false;

   Thread threads[3];
   SemaphoredTaskMock tasks[3];
   int randomOrder[] = { 2, 0, 1 };

   for ( int i = 0; i < 3; ++i )
   {
      int taskIdx = randomOrder[i];
      tasks[taskIdx].init( sequence, taskIdx, activate );
      bool wasStarted = threads[taskIdx].start( tasks[taskIdx] );
      CPPUNIT_ASSERT( wasStarted );
   }

   // activate all tasks
   activate = true;

   // wait for them to complete
   for ( int i = 0; i < 3; ++i )
   {
      threads[i].join();
   }

   // check the results
   CPPUNIT_ASSERT_EQUAL( (uint)3, sequence.size() );
   CPPUNIT_ASSERT_EQUAL( 0, sequence[0] );
   CPPUNIT_ASSERT_EQUAL( 1, sequence[1] );
   CPPUNIT_ASSERT_EQUAL( 2, sequence[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( CriticalSection, exclusiveAccessToResource )
{
   std::vector< int > sequence;
   bool activate = false;

   Thread threads[3];
   CriticalSectionedTaskMock tasks[3];
   int randomOrder[] = { 2, 0, 1 };

   for ( int i = 0; i < 3; ++i )
   {
      int taskIdx = randomOrder[i];
      tasks[taskIdx].init( sequence, taskIdx, activate );
      bool wasStarted = threads[taskIdx].start( tasks[taskIdx] );
      CPPUNIT_ASSERT( wasStarted );
   }

   // activate all tasks
   activate = true;

   // wait for them to complete
   for ( int i = 0; i < 3; ++i )
   {
      threads[i].join();
   }

   // check the results
   CPPUNIT_ASSERT_EQUAL( (uint)3, sequence.size() );
   CPPUNIT_ASSERT_EQUAL( 0, sequence[0] );
   CPPUNIT_ASSERT_EQUAL( 1, sequence[1] );
   CPPUNIT_ASSERT_EQUAL( 2, sequence[2] );
}

///////////////////////////////////////////////////////////////////////////////
