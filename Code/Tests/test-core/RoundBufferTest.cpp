#include "core-TestFramework\TestFramework.h"
#include "core\RoundBuffer.h"
#include "core\Timer.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class RoundBufferObjectMock
   {
      DECLARE_ALLOCATOR( RoundBufferObjectMock, AM_DEFAULT );

      int      m_val;
      RoundBufferObjectMock( int val = 0 ) : m_val( val ) {}
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, oneObject )
{
   RoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObjectMock* obj = new( &buffer ) RoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj, buffer.front< RoundBufferObjectMock >() );

   delete obj;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( (RoundBufferObjectMock*)NULL, buffer.front< RoundBufferObjectMock >() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, twoObjects )
{
   RoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObjectMock* obj1 = new( &buffer ) RoundBufferObjectMock();
   RoundBufferObjectMock* obj2 = new( &buffer ) RoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)2, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj1, buffer.front< RoundBufferObjectMock >() );

   delete obj1;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj2, buffer.front< RoundBufferObjectMock >() );

   delete obj2;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, buffer.getMemoryUsed() );
   CPPUNIT_ASSERT_EQUAL( (RoundBufferObjectMock*)NULL, buffer.front< RoundBufferObjectMock >() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, manyAllocations )
{
   RoundBuffer buffer( 4096 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObjectMock* arr[ OBJECTS_COUNT ];

   // allocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      arr[j] = new( &buffer ) RoundBufferObjectMock();
      CPPUNIT_ASSERT_EQUAL( arr[0], buffer.front< RoundBufferObjectMock >() );
   }

   // deallocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      delete arr[j];
      if ( j < OBJECTS_COUNT - 1 )
      {
         CPPUNIT_ASSERT_EQUAL( arr[ j + 1 ], buffer.front< RoundBufferObjectMock >() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

#ifndef _TRACK_MEMORY_ALLOCATIONS

TEST( RoundBuffer, performance )
{
   RoundBuffer buffer( 4096 );

   const int OBJECTS_COUNT = 100;
   const int LOOPS_COUNT = 65535;
   RoundBufferObjectMock* arr[ OBJECTS_COUNT ];

   CTimer timer;
   timer.tick();
   // check the execution time of an empty loop
   for ( int i = 0; i < LOOPS_COUNT; ++i ) {}
   timer.tick();
   float emptyLoopDuration = timer.getTimeElapsed();

   // now measure the time of multiple allocations / deallocations
   timer.tick();
   for ( int i = 0; i < LOOPS_COUNT; ++i )
   {
      // allocate
      for ( int j = 0; j < OBJECTS_COUNT; ++j )
      {
         arr[j] = new( &buffer ) RoundBufferObjectMock();
      }

      // deallocate
      for ( int j = 0; j < OBJECTS_COUNT; ++j )
      {
         delete arr[j];
      }
   }
   timer.tick();
   float allocDeallocDuration = timer.getTimeElapsed();
   CPPUNIT_ASSERT( 2.5f >= allocDeallocDuration );
}

#endif

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, circularAllocations )
{
   // create a buffer with an unaligned size - we want to test if subsequent allocations
   // and deallocations that will wrap around the buffer's end will work well
   uint allocSize = RoundBuffer::sizeOf< RoundBufferObjectMock >();
   RoundBuffer buffer( ( allocSize * 2 ) + 3 );

   new ( &buffer ) RoundBufferObjectMock( 0 );
   for ( int i = 1; i < 100; ++i )
   {
      new ( &buffer ) RoundBufferObjectMock( i );

      RoundBufferObjectMock* restoredVal = buffer.front< RoundBufferObjectMock >();
      CPPUNIT_ASSERT( NULL != restoredVal );
      CPPUNIT_ASSERT_EQUAL( i - 1, restoredVal->m_val );

      delete restoredVal;
   }

   RoundBufferObjectMock* restoredVal = buffer.front< RoundBufferObjectMock >();
   delete restoredVal;
}

///////////////////////////////////////////////////////////////////////////////
