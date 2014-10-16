#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionTypeComponent.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionSaver.h"
#include "core\ReflectionLoader.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionObjectsTracker.h"
#include "core\ReflectionPropertiesView.h"
#include "core\Resource.h"
#include "core\InArrayStream.h"
#include "core\OutArrayStream.h"
#include "core\PatchesDB.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   // -------------------------------------------------------------------------

   struct SerializationTestClass : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_val1;
      int      m_val2;

      SerializationTestClass( int val1 = 0, int val2 = 0, const char* id = NULL ) 
         : ReflectionObject( id )
         , m_val1( val1 )
         , m_val2( val2 ) 
      {
      }
      virtual ~SerializationTestClass() {}
   };
   BEGIN_OBJECT( SerializationTestClass );
      PROPERTY( int, m_val1 );
      PROPERTY( int, m_val2 );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct DerivedSerializationTestClass : public SerializationTestClass
   {
      DECLARE_STRUCT()

      int      m_val3;

      DerivedSerializationTestClass( int val1 = 0, int val2 = 0, int val3 = 0 ) : SerializationTestClass( val1, val2 ), m_val3( val3 ) {}
   };
   BEGIN_OBJECT( DerivedSerializationTestClass );
      PARENT( SerializationTestClass );
      PROPERTY( int, m_val3 );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct SerializationTestClassWithPointers : public ReflectionObject
   {
      DECLARE_STRUCT()

      int                           m_val;
      SerializationTestClassWithPointers*        m_ptr;

      SerializationTestClassWithPointers( int val = 0, SerializationTestClassWithPointers* ptr = NULL ) : m_val( val ), m_ptr( ptr ) {}
      ~SerializationTestClassWithPointers() { delete m_ptr; m_ptr = NULL; }
   };
   BEGIN_OBJECT( SerializationTestClassWithPointers );
      PROPERTY( int, m_val );
      PROPERTY( SerializationTestClassWithPointers*, m_ptr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct SerializationTestClassWithPODArray : public ReflectionObject
   {
      DECLARE_STRUCT()

      std::vector< int >                  m_arr;
   };
   BEGIN_OBJECT( SerializationTestClassWithPODArray );
      PROPERTY( std::vector< int >, m_arr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct SerializationTestClassWithPtrArray : public ReflectionObject
   {
      DECLARE_STRUCT()

      std::vector< SerializationTestClass* >           m_arr;

      ~SerializationTestClassWithPtrArray()
      {
         uint count = m_arr.size();
         for ( uint i = 0; i < count; ++i )
         {
            delete m_arr[i];
         }
         m_arr.clear();
      }
   };
   BEGIN_OBJECT( SerializationTestClassWithPtrArray );
      PROPERTY( std::vector< SerializationTestClass* >, m_arr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct SerializationTestClassWithSharedPointers : public ReflectionObject
   {
      DECLARE_STRUCT()

      ReflectionObject*        m_ptr;

      SerializationTestClassWithSharedPointers( const char* id = NULL, ReflectionObject* ptr = NULL ) : ReflectionObject( id ), m_ptr( ptr ) {}
      ~SerializationTestClassWithSharedPointers() { m_ptr = NULL; }
   };
   BEGIN_OBJECT( SerializationTestClassWithSharedPointers );
      PROPERTY( ReflectionObject*, m_ptr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct SerializationTestClassNotifiable : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_valOrig;
      int      m_valPreSave;

      SerializationTestClassNotifiable( int val = 0 ) : m_valOrig( val ), m_valPreSave( 0 ) {}
      virtual ~SerializationTestClassNotifiable() {}

      // ----------------------------------------------------------------------
      // ReflectionObject implementation
      // ----------------------------------------------------------------------
      void onObjectPreSave() { m_valPreSave = 1; }
   };
   BEGIN_OBJECT( SerializationTestClassNotifiable );
      PROPERTY( int, m_valOrig );
      PROPERTY( int, m_valPreSave );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class ReflectionObjectsTrackerMock : public ReflectionObjectsTracker
   {
   private:
      std::vector< ReflectionObject* >       m_trackedObjects;

   public:
      ~ReflectionObjectsTrackerMock()
      {
         uint count = m_trackedObjects.size();
         for ( uint i = 0; i < count; ++i )
         {
            delete m_trackedObjects[i];
         }
         m_trackedObjects.clear();
      }

      // ----------------------------------------------------------------------
      // ReflectionObjectsTrackerMock implementation
      // ----------------------------------------------------------------------
      void trackInstance( ReflectionObject* object )
      {
         m_trackedObjects.push_back( object );
      }

      ReflectionObject* findInstance( const ReflectionObject::UniqueId& id )
      {
         uint count = m_trackedObjects.size();
         for ( uint i = 0; i < count; ++i )
         {
            if ( m_trackedObjects[i]->m_uniqueId == id )
            {
               return m_trackedObjects[i];
            }
         }

         return NULL;
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( SerializationTestClass )
DEFINE_TYPE_ID( SerializationTestClassNotifiable )
DEFINE_TYPE_ID( SerializationTestClassWithSharedPointers )
DEFINE_TYPE_ID( SerializationTestClassWithPtrArray )
DEFINE_TYPE_ID( SerializationTestClassWithPODArray )
DEFINE_TYPE_ID( SerializationTestClassWithPointers )
DEFINE_TYPE_ID( DerivedSerializationTestClass )

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, simpleTypes )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() ); 

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );
   
   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the object
   SerializationTestClass obj( 5, 10, "obj.m_uniqueId" ); 
   ReflectionSaver saver( outStream );
   saver.save( &obj );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClass* restoredObject = loader.getNextObject< SerializationTestClass >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   CPPUNIT_ASSERT_EQUAL( 5, restoredObject->m_val1 );
   CPPUNIT_ASSERT_EQUAL( 10, restoredObject->m_val2 );

   // check if the unique IDs have been restored correctly
   CPPUNIT_ASSERT_EQUAL( obj.m_uniqueId, restoredObject->m_uniqueId );


   // cleanup
   delete restoredObject;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, inheritance )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() );
   typesRegistry.addSerializableType< DerivedSerializationTestClass >( "DerivedSerializationTestClass", new TSerializableTypeInstantiator< DerivedSerializationTestClass >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the object
   DerivedSerializationTestClass obj( 1, 4, 7 ); 
   ReflectionSaver saver( outStream );
   saver.save( &obj );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClass* restoredObjectBase = loader.getNextObject< SerializationTestClass >();
   CPPUNIT_ASSERT( restoredObjectBase != NULL );
   CPPUNIT_ASSERT_EQUAL( 1, restoredObjectBase->m_val1 );
   CPPUNIT_ASSERT_EQUAL( 4, restoredObjectBase->m_val2 );

   DerivedSerializationTestClass* restoredObjectProper = static_cast< DerivedSerializationTestClass* >( restoredObjectBase );
   CPPUNIT_ASSERT_EQUAL( 7, restoredObjectProper->m_val3 );

   // cleanup
   delete restoredObjectBase;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, pointers )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClassWithPointers >( "SerializationTestClassWithPointers", new TSerializableTypeInstantiator< SerializationTestClassWithPointers >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   SerializationTestClassWithPointers* obj1 = new SerializationTestClassWithPointers( 1 );
   SerializationTestClassWithPointers obj2( 2, obj1 );
   ReflectionSaver saver( outStream );
   saver.save( &obj2 );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassWithPointers* restoredObject = loader.getNextObject< SerializationTestClassWithPointers >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   CPPUNIT_ASSERT_EQUAL( 2, restoredObject->m_val );

   CPPUNIT_ASSERT( restoredObject->m_ptr != NULL );
   CPPUNIT_ASSERT_EQUAL( 1, restoredObject->m_ptr->m_val );

   // cleanup
   delete restoredObject;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, podsArrays )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClassWithPODArray >( "SerializationTestClassWithPODArray", new TSerializableTypeInstantiator< SerializationTestClassWithPODArray >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   SerializationTestClassWithPODArray obj;
   obj.m_arr.push_back( 1 );
   obj.m_arr.push_back( 2 );
   obj.m_arr.push_back( 3 );
   ReflectionSaver saver( outStream );
   saver.save( &obj );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassWithPODArray* restoredObject = loader.getNextObject< SerializationTestClassWithPODArray >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   CPPUNIT_ASSERT_EQUAL( (uint)3, restoredObject->m_arr.size() );
   CPPUNIT_ASSERT_EQUAL( 1, restoredObject->m_arr[0] );
   CPPUNIT_ASSERT_EQUAL( 2, restoredObject->m_arr[1] );
   CPPUNIT_ASSERT_EQUAL( 3, restoredObject->m_arr[2] );

   // cleanup
   delete restoredObject;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, pointersArrays )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() );
   typesRegistry.addSerializableType< SerializationTestClassWithPtrArray >( "SerializationTestClassWithPtrArray", new TSerializableTypeInstantiator< SerializationTestClassWithPtrArray >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   SerializationTestClassWithPtrArray obj;
   obj.m_arr.push_back( new SerializationTestClass( 1, 2 ) );
   obj.m_arr.push_back( new SerializationTestClass( 3, 4 ) );
   obj.m_arr.push_back( new SerializationTestClass( 5, 6 ) );
   ReflectionSaver saver( outStream );
   saver.save( &obj );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassWithPtrArray* restoredObject = loader.getNextObject< SerializationTestClassWithPtrArray >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   CPPUNIT_ASSERT_EQUAL( (uint)3, restoredObject->m_arr.size() );
   CPPUNIT_ASSERT( NULL != restoredObject->m_arr[0] );
   CPPUNIT_ASSERT( NULL != restoredObject->m_arr[1] );
   CPPUNIT_ASSERT( NULL != restoredObject->m_arr[2] );
   CPPUNIT_ASSERT_EQUAL( 1, restoredObject->m_arr[0]->m_val1 );
   CPPUNIT_ASSERT_EQUAL( 2, restoredObject->m_arr[0]->m_val2 );
   CPPUNIT_ASSERT_EQUAL( 3, restoredObject->m_arr[1]->m_val1 );
   CPPUNIT_ASSERT_EQUAL( 4, restoredObject->m_arr[1]->m_val2 );
   CPPUNIT_ASSERT_EQUAL( 5, restoredObject->m_arr[2]->m_val1 );
   CPPUNIT_ASSERT_EQUAL( 6, restoredObject->m_arr[2]->m_val2 );

   // cleanup
   delete restoredObject;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, sharedPointers )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() );
   typesRegistry.addSerializableType< SerializationTestClassWithSharedPointers >( "SerializationTestClassWithSharedPointers", new TSerializableTypeInstantiator< SerializationTestClassWithSharedPointers >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   SerializationTestClass* sharedObj = new SerializationTestClass( 1, 2 );
   SerializationTestClassWithSharedPointers obj1( "obj1", sharedObj );
   SerializationTestClassWithSharedPointers obj2( "obj1", sharedObj );
   ReflectionSaver saver( outStream );
   saver.save( &obj1 );
   saver.save( &obj2 );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassWithSharedPointers* restoredObject1 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   SerializationTestClassWithSharedPointers* restoredObject2 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   CPPUNIT_ASSERT( restoredObject1 != NULL );
   CPPUNIT_ASSERT( restoredObject2 != NULL );
   CPPUNIT_ASSERT( restoredObject1 != restoredObject2 );
   CPPUNIT_ASSERT( restoredObject1->m_ptr != NULL );
   CPPUNIT_ASSERT( restoredObject2->m_ptr != NULL );
   CPPUNIT_ASSERT( restoredObject1->m_ptr == restoredObject2->m_ptr );
   CPPUNIT_ASSERT( sharedObj != restoredObject1->m_ptr );

   // cleanup
   delete restoredObject1->m_ptr;
   delete restoredObject1;
   delete restoredObject2;
   delete sharedObj;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, deepNestingOfTheSharedPointers )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() );
   typesRegistry.addSerializableType< SerializationTestClassWithSharedPointers >( "SerializationTestClassWithSharedPointers", new TSerializableTypeInstantiator< SerializationTestClassWithSharedPointers >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   SerializationTestClass* sharedObjLevel2 = new SerializationTestClass( 1, 2 );
   SerializationTestClassWithSharedPointers* sharedObjLevel1 = new SerializationTestClassWithSharedPointers( "sharedObjLevel1", sharedObjLevel2 );
   SerializationTestClassWithSharedPointers obj1( "obj1", sharedObjLevel1 );
   SerializationTestClassWithSharedPointers obj2( "obj2", sharedObjLevel1 );
   ReflectionSaver saver( outStream );
   saver.save( &obj1 );
   saver.save( &obj2 );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassWithSharedPointers* restoredObject1 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   SerializationTestClassWithSharedPointers* restoredObject2 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   CPPUNIT_ASSERT( restoredObject1 != NULL );
   CPPUNIT_ASSERT( restoredObject2 != NULL );
   CPPUNIT_ASSERT( restoredObject1 != restoredObject2 );
   CPPUNIT_ASSERT( restoredObject1->m_ptr != NULL );
   CPPUNIT_ASSERT( restoredObject2->m_ptr != NULL );
   CPPUNIT_ASSERT( restoredObject1->m_ptr == restoredObject2->m_ptr );
   CPPUNIT_ASSERT( sharedObjLevel1 != restoredObject1->m_ptr );

   SerializationTestClassWithSharedPointers* restoredSharedObjLevel1 = dynamic_cast< SerializationTestClassWithSharedPointers * >( restoredObject1->m_ptr );
   CPPUNIT_ASSERT( NULL != restoredSharedObjLevel1->m_ptr );
   CPPUNIT_ASSERT( sharedObjLevel2 != restoredSharedObjLevel1->m_ptr );

   // cleanup
   delete restoredObject1;
   delete restoredObject2;
   delete restoredSharedObjLevel1->m_ptr;
   delete restoredSharedObjLevel1;
   delete sharedObjLevel2;
   delete sharedObjLevel1;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, instancesTracking )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClass >( "SerializationTestClass", new TSerializableTypeInstantiator< SerializationTestClass >() );
   typesRegistry.addSerializableType< SerializationTestClassWithSharedPointers >( "SerializationTestClassWithSharedPointers", new TSerializableTypeInstantiator< SerializationTestClassWithSharedPointers >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // set up an instances tracker
   ReflectionObjectsTrackerMock tracker;

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the objects - first one, than the other, so that their dependencies
   // are mapped independent of each other
   SerializationTestClass* sharedObj = new SerializationTestClass( 1, 2, "sharedObj" );
   SerializationTestClassWithSharedPointers obj1( "obj1", sharedObj );
   SerializationTestClassWithSharedPointers obj2( "obj2", sharedObj );
   ReflectionSaver saver( outStream );
   saver.save( &obj1 );
   saver.flush();
   saver.save( &obj2 );
   saver.flush();

   // restore the object
   ReflectionLoader loader( &tracker );
   loader.deserialize( inStream );
   SerializationTestClassWithSharedPointers* restoredObject1 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   loader.deserialize( inStream );
   SerializationTestClassWithSharedPointers* restoredObject2 = loader.getNextObject< SerializationTestClassWithSharedPointers >();
   CPPUNIT_ASSERT( restoredObject1 != NULL );
   CPPUNIT_ASSERT( restoredObject2 != NULL );
   CPPUNIT_ASSERT( restoredObject1 != restoredObject2 );

   // let's see if the tracker managed to map the shared object - if it has,
   // both instances should have the same pointer, even though they were serialized in separate batches
   CPPUNIT_ASSERT( restoredObject1->m_ptr == restoredObject2->m_ptr );
   CPPUNIT_ASSERT( sharedObj != restoredObject1->m_ptr );

   // cleanup - and now we don't need to delete the restored objects ourselves - the tracker will take care of that
   delete sharedObj;
}

///////////////////////////////////////////////////////////////////////////////

TEST( Serialization, loadSaveNotifications )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< SerializationTestClassNotifiable >( "SerializationTestClassNotifiable", new TSerializableTypeInstantiator< SerializationTestClassNotifiable >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the object
   SerializationTestClassNotifiable obj( 5 ); 

   // check if the pre-notification managed to populate the other value

   CPPUNIT_ASSERT_EQUAL( 5, obj.m_valOrig );
   CPPUNIT_ASSERT_EQUAL( 0, obj.m_valPreSave );

   ReflectionSaver saver( outStream );
   saver.save( &obj );

   CPPUNIT_ASSERT_EQUAL( 5, obj.m_valOrig );
   CPPUNIT_ASSERT_EQUAL( 1, obj.m_valPreSave );

   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   SerializationTestClassNotifiable* restoredObject = loader.getNextObject< SerializationTestClassNotifiable >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   
   CPPUNIT_ASSERT_EQUAL( 5, obj.m_valOrig );
   CPPUNIT_ASSERT_EQUAL( 1, obj.m_valPreSave );

   CPPUNIT_ASSERT_EQUAL( 5, restoredObject->m_valOrig );
   CPPUNIT_ASSERT_EQUAL( 1, restoredObject->m_valPreSave );

   // cleanup
   delete restoredObject;
}

///////////////////////////////////////////////////////////////////////////////
