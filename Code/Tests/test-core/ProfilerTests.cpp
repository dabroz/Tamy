#include "core-TestFramework\TestFramework.h"
#include "core\types.h"
#include "core\Profiler.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Profiler, basics )
{
   Profiler& profiler = TSingleton< Profiler >::getInstance();
   uint timerId1 = profiler.registerTimer( "myMethod1" );
   uint timerId2 = profiler.registerTimer( "myMethod2" );

   // query timer data
   CPPUNIT_ASSERT_EQUAL( std::string( "myMethod1" ), profiler.getTimerName( timerId1 ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "myMethod2" ), profiler.getTimerName( timerId2 ) );

   // query basic stack
   {
      profiler.beginFrame();
      profiler.start( timerId1 );
      profiler.end( timerId1 );

      CPPUNIT_ASSERT_EQUAL( (uint)1, profiler.getTracesCount() );
      CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(0).m_timerId );
   }
   
   // query a top level method called twice
   {
      profiler.beginFrame();
      profiler.start( timerId1 );
      profiler.end( timerId1 );
      profiler.start( timerId2 );
      profiler.end( timerId2 );

      CPPUNIT_ASSERT_EQUAL( (uint)2, profiler.getTracesCount() );
      
      CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(0).m_timerId );
      CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(1).m_timerId );
   }

   // query a simple nested callstack
   {
      profiler.beginFrame();
      profiler.start( timerId1 );
         profiler.start( timerId2 );
         profiler.end( timerId2 );
      profiler.end( timerId1 );

      CPPUNIT_ASSERT_EQUAL( (uint)2, profiler.getTracesCount() );

      CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(0).m_timerId );
      CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(1).m_timerId );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Profiler, recurrency )
{
   Profiler& profiler = TSingleton< Profiler >::getInstance();
   uint timerId1 = profiler.registerTimer( "myMethod1" );
   uint timerId2 = profiler.registerTimer( "myMethod2" );
   uint timerId3 = profiler.registerTimer( "myMethod3" );

   profiler.beginFrame();
   profiler.start( timerId1 );
      profiler.start( timerId2 );
         profiler.start( timerId2 );

            profiler.start( timerId3 );
               profiler.start( timerId1 );
               profiler.end( timerId1 );
            profiler.end( timerId3 );

         profiler.end( timerId2 );

         profiler.start( timerId3 );
            profiler.start( timerId1 );
            profiler.end( timerId1 );
         profiler.end( timerId3 );

      profiler.end( timerId2 );
   profiler.end( timerId1 );

   CPPUNIT_ASSERT_EQUAL( (uint)7, profiler.getTracesCount() );

   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(0).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(1).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(2).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId3, profiler.getTrace(3).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(4).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId3, profiler.getTrace(5).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(6).m_timerId );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Profiler, valuesProfiling )
{
   Profiler& profiler = TSingleton< Profiler >::getInstance();
   uint timerId1 = profiler.registerTimer( "myMethod1" );
   uint timerId2 = profiler.registerTimer( "myMethod2" );
   uint timerId3 = profiler.registerTimer( "myMethod3" );

   profiler.beginFrame();
   profiler.start( timerId1 );
   profiler.start( timerId2 );
   profiler.start( timerId2 );

   profiler.start( timerId3 );
   profiler.start( timerId1 );
   profiler.end( timerId1 );
   profiler.end( timerId3 );

   profiler.end( timerId2 );

   profiler.start( timerId3 );
   profiler.start( timerId1 );
   profiler.end( timerId1 );
   profiler.end( timerId3 );

   profiler.end( timerId2 );
   profiler.end( timerId1 );

   CPPUNIT_ASSERT_EQUAL( (uint)7, profiler.getTracesCount() );

   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(0).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(1).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId2, profiler.getTrace(2).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId3, profiler.getTrace(3).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(4).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId3, profiler.getTrace(5).m_timerId );
   CPPUNIT_ASSERT_EQUAL( timerId1, profiler.getTrace(6).m_timerId );
}

///////////////////////////////////////////////////////////////////////////////
