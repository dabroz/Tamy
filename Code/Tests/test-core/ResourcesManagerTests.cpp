#include "core-TestFramework\TestFramework.h"
#include "core\MemoryRouter.h"
#include "core\Filesystem.h"
#include "core\File.h"
#include "core\Resource.h"
#include "core\ResourcesManager.h"

// patching
#include "core\PatchesDB.h"

// multithreaded resources management
#include "core\MultithreadedTasksScheduler.h"
#include "core\Thread.h"
#include "core\ThreadSafe.h"
#include "core\CriticalSection.h"

// utils
#include <string>
#include <map>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class ResourceMock : public Resource
   {
      DECLARE_RESOURCE()
      DECLARE_ALLOCATOR( ResourceMock, AM_DEFAULT );

   private:
      int m_val;

   public:
      ResourceMock( const FilePath& resourceName = FilePath(), int val = -1 ) 
         : Resource( resourceName )
         , m_val( val ) 
      {}

      int getValue() const { return m_val; }
   };
   BEGIN_RESOURCE(ResourceMock, txt, AM_BINARY);
      PROPERTY_EDIT("val", int, m_val);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class ResourceWithPointerMock : public Resource
   {
      DECLARE_RESOURCE()
      DECLARE_ALLOCATOR( ResourceWithPointerMock, AM_DEFAULT );

   public:
      Resource*		 m_referencedRes;

   public:
      ResourceWithPointerMock( const FilePath& resourceName = FilePath() ) 
         : Resource( resourceName )
         , m_referencedRes( NULL )
      {}

   };
   BEGIN_RESOURCE( ResourceWithPointerMock, rwp, AM_BINARY );
      PROPERTY_EDIT( "m_referencedRes", Resource*, m_referencedRes );
   END_OBJECT();

} // anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( ResourceMock )
DEFINE_TYPE_ID( ResourceWithPointerMock )

///////////////////////////////////////////////////////////////////////////////

TEST(ResourcesManager, basic)
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< ResourceMock >( "ResourceMock", new TSerializableTypeInstantiator< ResourceMock >() );
   typesRegistry.addSerializableType< ResourceHandle >( "ResourceHandle", NULL  );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   ResourcesManager manager;
   Filesystem filesystem( "..\\Data" );
   manager.setFilesystem( filesystem );

   ResourceMock* resourceMock = new ResourceMock( FilePath( "resourceMock.txt" ), 5 );
   manager.addResource( resourceMock );
   resourceMock->saveResource();

   manager.reset();

   ResourceMock* res1 = dynamic_cast< ResourceMock* >( manager.create( FilePath( "resourceMock.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(5, res1->getValue());
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, manager.getResourcesCount());

   ResourceMock* res2 = dynamic_cast< ResourceMock* >( manager.create( FilePath( "resourceMock.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, manager.getResourcesCount());
   CPPUNIT_ASSERT(res1 == res2);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Resource, allRelatedResourcesAreSerializedTogether )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< ResourceWithPointerMock >( "ResourceWithPointerMock", new TSerializableTypeInstantiator< ResourceWithPointerMock >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the resources
   ResourcesManager manager;
   Filesystem filesystem( "..\\Data" );
   manager.setFilesystem( filesystem );

   FilePath resource1Name( "res1.rwp" );
   ResourceWithPointerMock* res1 = new ResourceWithPointerMock( resource1Name );
   manager.addResource( res1 );

   FilePath resource2Name( "res2.rwp" );
   ResourceWithPointerMock* res2 = new ResourceWithPointerMock( resource2Name );
   manager.addResource( res2 );

   // define the dependencies between the resources
   res1->m_referencedRes = res2;
   res2->m_referencedRes = res1;

   // save the one of the resources
   res1->saveResource();

   // reset the resources manager, so that the original resources get deleted
   manager.reset();
   res1 = NULL;
   res2 = NULL;

   // restore the resources
   ResourceWithPointerMock* restoredRes1 = static_cast< ResourceWithPointerMock* >( manager.create( resource1Name ) );
   CPPUNIT_ASSERT( restoredRes1 != NULL );
   
   // but you'll be able to reclaim it from the resources manager
   Resource* restoredResourceGeneric = manager.findResource( resource2Name );
   CPPUNIT_ASSERT( restoredResourceGeneric != NULL );
   CPPUNIT_ASSERT( restoredResourceGeneric->isExactlyA< ResourceWithPointerMock >() );

   ResourceWithPointerMock* restoredRes2 = static_cast< ResourceWithPointerMock* >( restoredResourceGeneric );
   CPPUNIT_ASSERT( restoredRes1 != restoredRes2 );
   CPPUNIT_ASSERT( restoredRes1->m_referencedRes != NULL );
   CPPUNIT_ASSERT( restoredRes2->m_referencedRes != NULL );
   CPPUNIT_ASSERT( restoredRes1->m_referencedRes == restoredRes2 );
   CPPUNIT_ASSERT( restoredRes2->m_referencedRes == restoredRes1 );

   // cleanup
   restoredRes1 = NULL;
   restoredRes2 = NULL;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Resource, loadingResourcesAsynchronously )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< ResourceWithPointerMock >( "ResourceWithPointerMock", new TSerializableTypeInstantiator< ResourceWithPointerMock >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the resources
   ResourcesManager manager;
   Filesystem filesystem( "..\\Data" );
   manager.setFilesystem( filesystem );

   FilePath resource1Name( "res1.rwp" );
   ResourceWithPointerMock* res1 = new ResourceWithPointerMock( resource1Name );

   manager.addResource( res1 );

   FilePath resource2Name( "res2.rwp" );
   ResourceWithPointerMock* res2 = new ResourceWithPointerMock( resource2Name );
   manager.addResource( res2 );

   // define the dependencies between the resources
   res1->m_referencedRes = res2;
   res2->m_referencedRes = res1;

   // save the one of the 
   res1->saveResource();

   // reset the resources manager, so that the original resources get deleted
   manager.reset();
   res1 = NULL;
   res2 = NULL;

   // restore the resources
   manager.create( resource1Name, true );

   ResourceWithPointerMock* restoredRes1 = manager.findResource< ResourceWithPointerMock >( resource1Name ); 
   CPPUNIT_ASSERT( restoredRes1 != NULL );

   // but you'll be able to reclaim it from the resources manager
   Resource* restoredResourceGeneric = manager.findResource( resource2Name );
   CPPUNIT_ASSERT( restoredResourceGeneric != NULL );
   CPPUNIT_ASSERT( restoredResourceGeneric->isExactlyA< ResourceWithPointerMock >() );

   ResourceWithPointerMock* restoredRes2 = static_cast< ResourceWithPointerMock* >( restoredResourceGeneric );
   CPPUNIT_ASSERT( restoredRes1 != restoredRes2 );
   CPPUNIT_ASSERT( restoredRes1->m_referencedRes != NULL );
   CPPUNIT_ASSERT( restoredRes2->m_referencedRes != NULL );
   CPPUNIT_ASSERT( restoredRes1->m_referencedRes == restoredRes2 );
   CPPUNIT_ASSERT( restoredRes2->m_referencedRes == restoredRes1 );

   // cleanup
   restoredRes1 = NULL;
   restoredRes2 = NULL;
}

///////////////////////////////////////////////////////////////////////////////

TEST( ResourceHandle, asynchronousHandleLoading )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< ResourceMock >( "ResourceMock", new TSerializableTypeInstantiator< ResourceMock >() );
   typesRegistry.addSerializableType< ResourceHandle >( "ResourceHandle", NULL  );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );


   // prepare the resources
   ResourcesManager manager;
   Filesystem filesystem( "..\\Data" );
   manager.setFilesystem( filesystem );

   FilePath resourceName( "res3.txt" );
   ResourceMock* res = new ResourceMock( resourceName );

   manager.addResource( res );
   res->saveResource();

   // reset the resources manager, so that the original resources get deleted
   manager.reset();
   res = NULL;

   TResourceHandle< ResourceMock > hObj( resourceName, &manager );

   // the first query should return NULL - after all the resource is not there, and a query's just been placed to get it loaded
   CPPUNIT_ASSERT( NULL == hObj.get() );

   // but eventually it should come around
   while( NULL == hObj.get() )
   {
      Thread::yield(); 
   }
   CPPUNIT_ASSERT( true );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ResourceHandle, unloadedResources )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< ResourceMock >( "ResourceMock", new TSerializableTypeInstantiator< ResourceMock >() );
   typesRegistry.addSerializableType< ResourceHandle >( "ResourceHandle", NULL  );


   // prepare the resources
   ResourcesManager manager;
   Filesystem filesystem( "..\\Data" );
   manager.setFilesystem( filesystem );

   FilePath resourceName( "testResource.txt" );
   ResourceMock* res = new ResourceMock( resourceName );

   manager.addResource( res );

   // create a resource handle and wait until it's loaded
   TResourceHandle< ResourceMock > hObj( resourceName, &manager );
   while( NULL == hObj.get() )
   {
      Thread::yield(); 
   }

   CPPUNIT_ASSERT( NULL != hObj.get() );

   // now unload the resource - and the handle should follow, loosing its instance
   manager.unloadResource( res );
   CPPUNIT_ASSERT( hObj.isNull() );
}

///////////////////////////////////////////////////////////////////////////////
