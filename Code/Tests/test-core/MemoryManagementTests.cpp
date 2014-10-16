#include "core-TestFramework/TestFramework.h"
#include "core/MemoryRouter.h"
#include "core/MemoryUtils.h"
#include "core/Array.h"
#include "core/Thread.h"
#include "core/Runnable.h"
#include "core/DefaultAllocator.h"
#include "core/TSContinuousMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct TestClass
   {
      DECLARE_ALLOCATOR( TestClass, AM_DEFAULT );
   };

   // -------------------------------------------------------------------------

   template< typename T >
   struct TemplateTestClass
   {
      DECLARE_ALLOCATOR( TemplateTestClass, AM_ALIGNED_16 );

      struct NestedClass
      {
         DECLARE_ALLOCATOR( NestedClass, AM_ALIGNED_16 );
      };
   };

   // -------------------------------------------------------------------------

   struct MisalignedData
   {
      DECLARE_ALLOCATOR( MisalignedData, AM_ALIGNED_16 );

      char     m_singleByte;

      ALIGN_16 struct 
      {
         float v[4];
      } m_quad;
   };

   // -------------------------------------------------------------------------

   struct MultithreadedAllocFunc : public Runnable
   {
      MemoryAllocator&     m_allocator;

      MultithreadedAllocFunc( MemoryAllocator& allocator )
         : m_allocator( allocator )
      {}

      void run()
      {
         for ( int i = 0; i < 1000; ++i )
         {
            void* ptr = m_allocator.alloc( 16 );
            Thread::yield();

            m_allocator.dealloc( ptr );
            Thread::yield();
         }
      }
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, defaultMemoryAllocation )
{
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   void* ptr = router.alloc( 12, AM_DEFAULT, &router.m_defaultAllocator );
   
   // all we can test is that the memory is correctly allocated and deallocated
   CPPUNIT_ASSERT( ptr != NULL );

   // cleanup
   router.dealloc( ptr, AM_DEFAULT );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, alignedMemoryAllocation )
{
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   void* ptr = router.alloc( 12, AM_ALIGNED_16, &router.m_defaultAllocator );

   // all we can test is that the memory is correctly allocated and deallocated
   CPPUNIT_ASSERT( ptr != NULL );

   // and that the returned address is aligned to a 16 byte boundary
   bool isAligned = MemoryUtils::isAddressAligned( ptr );
   CPPUNIT_ASSERT( isAligned );

   // cleanup
   router.dealloc( ptr, AM_ALIGNED_16 );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, differentAllocationMethods )
{
   DefaultAllocator defaultAllocator;

   // at the beginning, no memory is allocated
   CPPUNIT_ASSERT_EQUAL( (ulong)0, defaultAllocator.getMemoryUsed() );
   
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   void* ptr = router.alloc( 12, AM_ALIGNED_16, &defaultAllocator );

   // memory was indeed allocated - but since we were allocating an aligned block,
   // a larger block was allocated
   CPPUNIT_ASSERT( ptr != NULL );
   CPPUNIT_ASSERT( (ulong)12 < defaultAllocator.getMemoryUsed() );

   // and that the returned address is aligned to a 16 byte boundary
   bool isAligned = MemoryUtils::isAddressAligned( ptr );
   CPPUNIT_ASSERT( isAligned );

   // cleanup
   router.dealloc( ptr, AM_ALIGNED_16 );

   // allocated memory was correctly released - the entire amount
   CPPUNIT_ASSERT_EQUAL( (ulong)0, defaultAllocator.getMemoryUsed() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, objectsAllocation )
{
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   ulong initialAllocatedMemorySize = router.getMemoryUsed();

   TestClass* obj = new TestClass();
   CPPUNIT_ASSERT( obj != NULL );
   ulong expectedSize = MemoryUtils::calcAlignedSize( (ulong)(sizeof( TestClass ) + sizeof(void*)) );
   CPPUNIT_ASSERT_EQUAL( expectedSize, router.getMemoryUsed() - initialAllocatedMemorySize );

   delete obj;
   CPPUNIT_ASSERT_EQUAL( (ulong)0, router.getMemoryUsed() - initialAllocatedMemorySize );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, objectsPlacement )
{
   DefaultAllocator defaultAllocator;
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   ulong initialAllocatedMemorySize = router.getMemoryUsed();

   TestClass* obj = new ( &defaultAllocator ) TestClass();
   CPPUNIT_ASSERT( obj != NULL );

   // no memory was allocated in the pool internally managed by the router
   CPPUNIT_ASSERT_EQUAL( (ulong)0, router.getMemoryUsed() - initialAllocatedMemorySize );

   // entire memory was allocated in the specified external pool
   ulong expectedSize = MemoryUtils::calcAlignedSize( (ulong)(sizeof( TestClass ) + sizeof(void*)) );
   CPPUNIT_ASSERT_EQUAL( expectedSize, defaultAllocator.getMemoryUsed() );

   delete obj;
   CPPUNIT_ASSERT_EQUAL( (ulong)0, router.getMemoryUsed() - initialAllocatedMemorySize );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, defaultAllocator.getMemoryUsed() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, ensureStaticallyAllocatedMembersAlignment )
{
   MisalignedData data;

   int startAddr = (int)&data;
   int singleByteMemberOffset = (int)&data.m_singleByte - startAddr;
   int quadOffset = (int)&data.m_quad - startAddr;

   CPPUNIT_ASSERT_EQUAL( 1, (int)sizeof( data.m_singleByte ) );
   CPPUNIT_ASSERT_EQUAL( 0, singleByteMemberOffset );
   CPPUNIT_ASSERT_EQUAL( 0, quadOffset % 16 );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRouter, ensureDynamicallyAllocatedMembersAlignment )
{
   MisalignedData* data = new MisalignedData();

   int startAddr = (int)data;
   int singleByteMemberOffset = (int)&data->m_singleByte - startAddr;
   int quadOffset = (int)&data->m_quad - startAddr;

   CPPUNIT_ASSERT_EQUAL( 0, singleByteMemberOffset );
   CPPUNIT_ASSERT_EQUAL( 0, quadOffset % 16 );

   delete data;
}

///////////////////////////////////////////////////////////////////////////////

TEST( DefaultAllocator, threadSafety )
{
   Thread thread1;
   Thread thread2;

   DefaultAllocator allocator;
   CPPUNIT_ASSERT_EQUAL( (ulong)0, allocator.getMemoryUsed() );

   MultithreadedAllocFunc allocOperator1( allocator );
   MultithreadedAllocFunc allocOperator2( allocator );

   thread1.start( allocOperator1 );
   thread2.start( allocOperator2 );

   thread1.join();
   thread2.join();

   CPPUNIT_ASSERT_EQUAL( (ulong)0, allocator.getMemoryUsed() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ContinuousMemoryPool, threadSafety )
{
   Thread thread1;
   Thread thread2;

   TSContinuousMemoryPool allocator( 512 );
   CPPUNIT_ASSERT_EQUAL( (ulong)0, allocator.getMemoryUsed() );

   MultithreadedAllocFunc allocOperator1( allocator );
   MultithreadedAllocFunc allocOperator2( allocator );

   thread1.start( allocOperator1 );
   thread2.start( allocOperator2 );

   thread1.join();
   thread2.join();

   CPPUNIT_ASSERT_EQUAL( (ulong)0, allocator.getMemoryUsed() );
}

///////////////////////////////////////////////////////////////////////////////
