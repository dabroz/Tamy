#include "core-TestFramework/TestFramework.h"
#include "core/CallstackTracer.h"
#include "core/CallstackTree.h"


///////////////////////////////////////////////////////////////////////////////

TEST( CallstackTracer, basicCallstack )
{
   CallstackTracer tracer;

   ulong trace[32];
   uint numTraces = tracer.getStackTrace( trace, 32 );

#ifdef _DEBUG
   CPPUNIT_ASSERT_EQUAL( (uint)22, numTraces );
#else
   CPPUNIT_ASSERT_EQUAL( (uint)17, numTraces );
#endif 

   // check the names
   char fileName[512];
   uint lineNumber;
   char methodName[512];
   tracer.getTraceName( trace[numTraces-1], fileName, 512, methodName, 512, lineNumber );
   CPPUNIT_ASSERT_EQUAL( std::string( "RtlInitializeExceptionChain" ), std::string( methodName ) );

   tracer.getTraceName( trace[1], fileName, 512, methodName, 512, lineNumber );

   // extract the method name
#ifdef _DEBUG
   CPPUNIT_ASSERT_EQUAL( std::string( "TestCallstackTracerbasicCallstack::testbasicCallstack" ), std::string( methodName ) );
#else
   CPPUNIT_ASSERT_EQUAL( std::string( "CppUnit::TestCaseMethodFunctor::operator()" ), std::string( methodName ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////

TEST( CallstackTracer, singleCallstack )
{
   CallstackTree tree;

   ulong callstack_1[] = { 0, 5, 3 };

   tree.insert( 0, callstack_1, 3 );
   CPPUNIT_ASSERT_EQUAL( (uint)1, tree.getCallstacksCount() );
   CPPUNIT_ASSERT_EQUAL( (uint)3, tree.getDepth() );
   
   ulong retrievedCallstack[3];
   uint callstackSize = tree.getCallstack( 0, retrievedCallstack, 3 );
   CPPUNIT_ASSERT_EQUAL( (uint)3, callstackSize );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, retrievedCallstack[0] );
   CPPUNIT_ASSERT_EQUAL( (ulong)5, retrievedCallstack[1] );
   CPPUNIT_ASSERT_EQUAL( (ulong)3, retrievedCallstack[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( CallstackTracer, twoSeparateCallstacks )
{
   CallstackTree tree;

   ulong callstack_1[] = { 0, 5, 3 };
   ulong callstack_2[] = { 2, 6, 4, 7 };

   tree.insert( 0, callstack_1, 3 );
   tree.insert( 1, callstack_2, 4 );
   CPPUNIT_ASSERT_EQUAL( (uint)2, tree.getCallstacksCount() );
   CPPUNIT_ASSERT_EQUAL( (uint)4, tree.getDepth() );

   ulong retrievedCallstack[4];
   uint callstackSize;

   // callstack 1
   {
      callstackSize = tree.getCallstack( 0, retrievedCallstack, 4 );
      CPPUNIT_ASSERT_EQUAL( (uint)3, callstackSize );
      CPPUNIT_ASSERT_EQUAL( (ulong)0, retrievedCallstack[0] );
      CPPUNIT_ASSERT_EQUAL( (ulong)5, retrievedCallstack[1] );
      CPPUNIT_ASSERT_EQUAL( (ulong)3, retrievedCallstack[2] );
   }

   // callstack 2
   {
      callstackSize = tree.getCallstack( 1, retrievedCallstack, 4 );
      CPPUNIT_ASSERT_EQUAL( (uint)4, callstackSize );
      CPPUNIT_ASSERT_EQUAL( (ulong)2, retrievedCallstack[0] );
      CPPUNIT_ASSERT_EQUAL( (ulong)6, retrievedCallstack[1] );
      CPPUNIT_ASSERT_EQUAL( (ulong)4, retrievedCallstack[2] );
      CPPUNIT_ASSERT_EQUAL( (ulong)7, retrievedCallstack[3] );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( CallstackTracer, twoOverlappingCallstacks )
{
   CallstackTree tree;

   ulong callstack_1[] = { 0, 5, 3 };
   ulong callstack_2[] = { 0, 5, 4, 7 };

   tree.insert( 0, callstack_1, 3 );
   tree.insert( 1, callstack_2, 4 );
   CPPUNIT_ASSERT_EQUAL( (uint)2, tree.getCallstacksCount() );
   CPPUNIT_ASSERT_EQUAL( (uint)4, tree.getDepth() );

   ulong retrievedCallstack[4];
   uint callstackSize;

   // callstack 1
   {
      callstackSize = tree.getCallstack( 0, retrievedCallstack, 4 );
      CPPUNIT_ASSERT_EQUAL( (uint)3, callstackSize );
      CPPUNIT_ASSERT_EQUAL( (ulong)0, retrievedCallstack[0] );
      CPPUNIT_ASSERT_EQUAL( (ulong)5, retrievedCallstack[1] );
      CPPUNIT_ASSERT_EQUAL( (ulong)3, retrievedCallstack[2] );
   }

   // callstack 2
   {
      callstackSize = tree.getCallstack( 1, retrievedCallstack, 4 );
      CPPUNIT_ASSERT_EQUAL( (uint)4, callstackSize );
      CPPUNIT_ASSERT_EQUAL( (ulong)0, retrievedCallstack[0] );
      CPPUNIT_ASSERT_EQUAL( (ulong)5, retrievedCallstack[1] );
      CPPUNIT_ASSERT_EQUAL( (ulong)4, retrievedCallstack[2] );
      CPPUNIT_ASSERT_EQUAL( (ulong)7, retrievedCallstack[3] );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( CallstackTracer, removingAnOverlappingCallstack )
{
   CallstackTree tree;

   ulong callstack_1[] = { 0, 5, 3 };
   ulong callstack_2[] = { 0, 5, 4, 7 };

   ulong memoryConsumedBefore = tree.getMemoryConsumption();

   tree.insert( 0, callstack_1, 3 );
   tree.insert( 1, callstack_2, 4 );
   memoryConsumedBefore = tree.getMemoryConsumption();
   tree.remove( 1 );
   ulong memoryConsumedAfter = tree.getMemoryConsumption();

   CPPUNIT_ASSERT( memoryConsumedBefore > memoryConsumedAfter );
   CPPUNIT_ASSERT_EQUAL( (uint)1, tree.getCallstacksCount() );
   CPPUNIT_ASSERT_EQUAL( (uint)3, tree.getDepth() );

   ulong retrievedCallstack[3];
   uint callstackSize = tree.getCallstack( 0, retrievedCallstack, 3 );
   CPPUNIT_ASSERT_EQUAL( (uint)3, callstackSize );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, retrievedCallstack[0] );
   CPPUNIT_ASSERT_EQUAL( (ulong)5, retrievedCallstack[1] );
   CPPUNIT_ASSERT_EQUAL( (ulong)3, retrievedCallstack[2] );

}

///////////////////////////////////////////////////////////////////////////////
