#include "core-TestFramework\TestFramework.h"
#include "core\ContinuousMemoryPool.h"
#include "core\FragmentedMemoryPool.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class MemoryPoolObjectMock
   {
      DECLARE_ALLOCATOR( MemoryPoolObjectMock, AM_DEFAULT );

   public:
      static int        s_instancesCount;

   private:
      int               m_val;

   public:
      MemoryPoolObjectMock( int val ) 
         : m_val( val ) 
      {
         ++s_instancesCount;
      }
      ~MemoryPoolObjectMock()
      {
         --s_instancesCount;
      }

      inline int getVal() const { return m_val; }
   };

   int MemoryPoolObjectMock::s_instancesCount = 0;

   // ------------------------------------------------------------------------

   struct NewMemoryPoolObjectMock
   {
      DECLARE_ALLOCATOR( NewMemoryPoolObjectMock, AM_DEFAULT );

      int      m_val;
      NewMemoryPoolObjectMock( int val ) : m_val( val ) {}
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST( ContinuousMemoryPool, simpleAllocationsAndDeallocations )
{
   ContinuousMemoryPool pool( 512 );

   CPPUNIT_ASSERT_EQUAL( 0, MemoryPoolObjectMock::s_instancesCount );

   MemoryPoolObjectMock* obj = new ( &pool ) MemoryPoolObjectMock( 1 );
   CPPUNIT_ASSERT_EQUAL( 1, dynamic_cast< MemoryPoolObjectMock* >(obj)->getVal() );
   CPPUNIT_ASSERT_EQUAL( 1, MemoryPoolObjectMock::s_instancesCount );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, pool.getAllocationsCount() );
   delete obj;
   CPPUNIT_ASSERT_EQUAL( 0, MemoryPoolObjectMock::s_instancesCount );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, pool.getAllocationsCount() );

   obj = new ( &pool ) MemoryPoolObjectMock( 2 ); 
   CPPUNIT_ASSERT_EQUAL( 2, dynamic_cast< MemoryPoolObjectMock* >(obj)->getVal() );
   CPPUNIT_ASSERT_EQUAL( 1, MemoryPoolObjectMock::s_instancesCount );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, pool.getAllocationsCount() );
   delete obj;
   CPPUNIT_ASSERT_EQUAL( 0, MemoryPoolObjectMock::s_instancesCount );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, pool.getAllocationsCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ContinuousMemoryPool, poolReset )
{
   ContinuousMemoryPool pool( 512 );

   CPPUNIT_ASSERT_EQUAL( 0, MemoryPoolObjectMock::s_instancesCount );
 
   MemoryPoolObjectMock* obj1 = new ( &pool ) MemoryPoolObjectMock( 1 );
   MemoryPoolObjectMock* obj2 = new ( &pool ) MemoryPoolObjectMock( 2 );

   long ptr1 = (long)obj1;
   long ptr2 = (long)obj2;

   CPPUNIT_ASSERT_EQUAL( (unsigned int)2, pool.getAllocationsCount() );

   // compare the pointers - the two objects should not have the same allocation address
   CPPUNIT_ASSERT( ptr1 != ptr2 );

   // delete the objects
   delete obj1;
   delete obj2;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, pool.getAllocationsCount() );

   // reset the pool
   pool.reset();

   // allocate objects from the start
   obj1 = new ( &pool ) MemoryPoolObjectMock( 1 );
   obj2 = new ( &pool ) MemoryPoolObjectMock( 2 );

   CPPUNIT_ASSERT_EQUAL( (unsigned int)2, pool.getAllocationsCount() );

   // compare the pointers - the respective objects should receive the same allocation addresses
   long newPtr1 = (long)obj1;
   long newPtr2 = (long)obj2;
   CPPUNIT_ASSERT_EQUAL( ptr1, newPtr1 );
   CPPUNIT_ASSERT_EQUAL( ptr2, newPtr2 );

   // delete the objects
   delete obj1;
   delete obj2;
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, pool.getAllocationsCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( FragmentedMemoryPool, basicAllocation )
{
   uint chunkSize = 32;
   uint chunksCount = 4;
   FragmentedMemoryPool memPool( chunksCount, chunkSize );

   CPPUNIT_ASSERT( memPool.getMemoryUsed() == 0 );

   // allocate a few pointers and then release them, but in a random order, so that
   // the pool would get fragmented ( or would it >:) )

   NewMemoryPoolObjectMock* obj[3];

   for ( uint i = 0; i < 10; ++i )
   {
      
      obj[0] = new ( &memPool ) NewMemoryPoolObjectMock( 0 );
      obj[1] = new ( &memPool ) NewMemoryPoolObjectMock( 1 );
      obj[2] = new ( &memPool ) NewMemoryPoolObjectMock( 2 );

      CPPUNIT_ASSERT( NULL != obj[0] );
      CPPUNIT_ASSERT( NULL != obj[1] );
      CPPUNIT_ASSERT( NULL != obj[2] );

      CPPUNIT_ASSERT_EQUAL( 0, obj[0]->m_val );
      CPPUNIT_ASSERT_EQUAL( 1, obj[1]->m_val );
      CPPUNIT_ASSERT_EQUAL( 2, obj[2]->m_val );

      CPPUNIT_ASSERT( memPool.getMemoryUsed() == 96 );

      // verify and delete the pointers in random order
      uint startIdx = rand() % 3;
      int idx = startIdx;
      for ( uint j = 0; j < 3; ++j, idx = ( idx + 1 ) % 3 )
      {
         CPPUNIT_ASSERT( NULL != obj[idx] );
         CPPUNIT_ASSERT_EQUAL( idx, obj[idx]->m_val );

         delete obj[idx];
      }

      CPPUNIT_ASSERT( memPool.getMemoryUsed() == 0 );
   }
}

///////////////////////////////////////////////////////////////////////////////
