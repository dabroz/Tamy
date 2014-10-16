#include "core-TestFramework\TestFramework.h"
#include "core\ClientServerRoundBuffer.h"
#include "core\RoundBuffer.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{

   struct ClientServerRoundBufferObjectMock
   {
      DECLARE_ALLOCATOR( ClientServerRoundBufferObjectMock, AM_DEFAULT );

      int      m_val;
      ClientServerRoundBufferObjectMock( int val = 0 ) : m_val( val ) {}
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( ClientServerRoundBuffer, oneObject )
{
   ClientServerRoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   ClientServerRoundBufferObjectMock* obj = new( &buffer ) ClientServerRoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getCommandsCount() );
   CPPUNIT_ASSERT_EQUAL( obj, buffer.front< ClientServerRoundBufferObjectMock >() );

   delete obj;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getCommandsCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ClientServerRoundBuffer, twoObjects )
{
   ClientServerRoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   ClientServerRoundBufferObjectMock* obj1 = new( &buffer ) ClientServerRoundBufferObjectMock();
   ClientServerRoundBufferObjectMock* obj2 = new( &buffer ) ClientServerRoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)2, buffer.getCommandsCount() );
   CPPUNIT_ASSERT_EQUAL( obj1, buffer.front< ClientServerRoundBufferObjectMock >() );

   delete obj1;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getCommandsCount() );
   CPPUNIT_ASSERT_EQUAL( obj2, buffer.front< ClientServerRoundBufferObjectMock >() );

   delete obj2;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getCommandsCount() );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, buffer.getMemoryUsed() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ClientServerRoundBuffer, manyAllocations )
{
   ClientServerRoundBuffer buffer( 4096 );

   const int OBJECTS_COUNT = 100;
   ClientServerRoundBufferObjectMock* arr[ OBJECTS_COUNT ];

   // allocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      arr[j] = new( &buffer ) ClientServerRoundBufferObjectMock();
      CPPUNIT_ASSERT_EQUAL( arr[0], buffer.front< ClientServerRoundBufferObjectMock >() );
   }

   // deallocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      delete arr[j];
      if ( j < OBJECTS_COUNT - 1 )
      {
         CPPUNIT_ASSERT_EQUAL( arr[ j + 1 ], buffer.front< ClientServerRoundBufferObjectMock >() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( ClientServerRoundBuffer, circularAllocations )
{
   // create a buffer with an unaligned size - we want to test if subsequent allocations
   // and deallocations that will wrap around the buffer's end will work well
   uint allocSize = RoundBuffer::sizeOf< ClientServerRoundBufferObjectMock >();
   ClientServerRoundBuffer buffer( ( allocSize * 2 ) + 3 );

   new ( &buffer ) ClientServerRoundBufferObjectMock( 0 );
   for ( int i = 1; i < 100; ++i )
   {
      new ( &buffer ) ClientServerRoundBufferObjectMock( i );

      ClientServerRoundBufferObjectMock* restoredVal = buffer.front< ClientServerRoundBufferObjectMock >() ;
      CPPUNIT_ASSERT( NULL != restoredVal );
      CPPUNIT_ASSERT_EQUAL( i - 1, restoredVal->m_val );

      delete restoredVal;
   }

   ClientServerRoundBufferObjectMock* restoredVal = buffer.front< ClientServerRoundBufferObjectMock >() ;
   delete restoredVal;
}

///////////////////////////////////////////////////////////////////////////////
