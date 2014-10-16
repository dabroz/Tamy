#include "core-TestFramework\TestFramework.h"
#include "core\Dequeue.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Dequeue, addingNewElements )
{
   Dequeue<int> queue;

   CPPUNIT_ASSERT_EQUAL( (uint)0, queue.size() );

   // append elements to the queue
   queue.push(0);
   CPPUNIT_ASSERT_EQUAL( (uint)1, queue.size() );
   CPPUNIT_ASSERT_EQUAL( 0, queue.front() );

   queue.push(1);
   CPPUNIT_ASSERT_EQUAL( (uint)2, queue.size() );
   CPPUNIT_ASSERT_EQUAL( 0, queue.front() );

   queue.push(2);
   CPPUNIT_ASSERT_EQUAL( (uint)3, queue.size() );
   CPPUNIT_ASSERT_EQUAL( 0, queue.front() );

   // remove elements from the queue
   CPPUNIT_ASSERT_EQUAL( 0, queue.pop() );
   CPPUNIT_ASSERT_EQUAL( (uint)2, queue.size() );

   CPPUNIT_ASSERT_EQUAL( 1, queue.pop() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, queue.size() );

   CPPUNIT_ASSERT_EQUAL( 2, queue.pop() );
   CPPUNIT_ASSERT_EQUAL( (uint)0, queue.size() );
};

///////////////////////////////////////////////////////////////////////////////

TEST( Dequeue, constantMemoryForOneElem )
{
   Dequeue< int > queue( 1 );
   for ( int i = 0; i < 10; ++i )
   {
      queue.push( i );

      int val = queue.pop();
      CPPUNIT_ASSERT_EQUAL( i, val );
   }

   CPPUNIT_ASSERT( queue.empty() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Dequeue, constantMemoryForTwoElems )
{
   Dequeue< int > queue( 2 );

   queue.push( 0 );
   for ( int i = 1; i < 10; ++i )
   {
      queue.push( i );

      int val = queue.pop();
      CPPUNIT_ASSERT_EQUAL( i - 1, val );
   }

   int val = queue.pop();
   CPPUNIT_ASSERT_EQUAL( 9, val );

   CPPUNIT_ASSERT( queue.empty() );
}

///////////////////////////////////////////////////////////////////////////////
