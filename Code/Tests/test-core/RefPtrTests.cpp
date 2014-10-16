#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionObject.h"
#include "core\Resource.h"
#include "core\RefPtr.h"
#include "core\InArrayStream.h"
#include "core\OutArrayStream.h"
#include "core\PatchesDB.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   struct TestObject : public ReflectionObject
   {
      DECLARE_STRUCT();

      int m_val;

      TestObject( int val = 7 ) : m_val( val ) {}
   };
   BEGIN_OBJECT( TestObject );
      PARENT( ReflectionObject );
      PROPERTY( int, m_val );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct ObjWithRefPtrMembers : public ReflectionObject
   {
      DECLARE_STRUCT();

      TRefPtr< TestObject >        m_obj1;
      TRefPtr< TestObject >        m_obj2;

      ObjWithRefPtrMembers( TestObject* obj1 = NULL, TestObject* obj2 = NULL )
         : m_obj1( obj1 )
         , m_obj2( obj2 )
      {
      }
   };

   BEGIN_OBJECT( ObjWithRefPtrMembers );
      PARENT( ReflectionObject );
      PROPERTY( TRefPtr< TestObject >, m_obj1 );
      PROPERTY( TRefPtr< TestObject >, m_obj2 );
   END_OBJECT();
}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( TestObject );
DEFINE_TYPE_ID( TRefPtr< TestObject > );
DEFINE_TYPE_ID( ObjWithRefPtrMembers );

///////////////////////////////////////////////////////////////////////////////

TEST( RefPtr, basicReferencesCounting )
{
   TestObject managedObject;

   CPPUNIT_ASSERT_EQUAL( 1, managedObject.getReferencesCount() );

   {
      TRefPtr<TestObject> ptr1( &managedObject );
      CPPUNIT_ASSERT_EQUAL( 2, managedObject.getReferencesCount() );

      {
         TRefPtr<TestObject> copiedRefPtr( ptr1 );
         CPPUNIT_ASSERT_EQUAL( 3, managedObject.getReferencesCount() );

         {
            TRefPtr<TestObject> assignedRefPtr_1;
            assignedRefPtr_1 = ptr1;
            CPPUNIT_ASSERT_EQUAL( 4, managedObject.getReferencesCount() );

            {
               TRefPtr<TestObject> assignedRefPtr_2;
               assignedRefPtr_2 = &managedObject;
               CPPUNIT_ASSERT_EQUAL( 5, managedObject.getReferencesCount() );
            }

            CPPUNIT_ASSERT_EQUAL( 4, managedObject.getReferencesCount() );
         }

         CPPUNIT_ASSERT_EQUAL( 3, managedObject.getReferencesCount() );
      }

      CPPUNIT_ASSERT_EQUAL( 2, managedObject.getReferencesCount() );
   }

   CPPUNIT_ASSERT_EQUAL( 1, managedObject.getReferencesCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RefPtr, classMember )
{
   TestObject obj1;
   TestObject obj2;

   CPPUNIT_ASSERT_EQUAL( 1, obj1.getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( 1, obj2.getReferencesCount() );
   {
      ObjWithRefPtrMembers objWithMembers( &obj1, &obj2 );

      CPPUNIT_ASSERT_EQUAL( 2, obj1.getReferencesCount() );
      CPPUNIT_ASSERT_EQUAL( 2, obj2.getReferencesCount() );
      {
         ObjWithRefPtrMembers copiedObjWithMembers( objWithMembers );

         CPPUNIT_ASSERT_EQUAL( 3, obj1.getReferencesCount() );
         CPPUNIT_ASSERT_EQUAL( 3, obj2.getReferencesCount() );
      }

      CPPUNIT_ASSERT_EQUAL( 2, obj1.getReferencesCount() );
      CPPUNIT_ASSERT_EQUAL( 2, obj2.getReferencesCount() );
   }

   CPPUNIT_ASSERT_EQUAL( 1, obj1.getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( 1, obj2.getReferencesCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RefPtr, dataAccess )
{
   TestObject obj( 7 );

   TRefPtr< TestObject > ptr( &obj );
   CPPUNIT_ASSERT_EQUAL( 7, ptr->m_val );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RefPtr, serialization )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< TestObject >( "TestObject", new TSerializableTypeInstantiator< TestObject >() );
   typesRegistry.addSerializableType< ObjWithRefPtrMembers >( "ObjWithRefPtrMembers", new TSerializableTypeInstantiator< ObjWithRefPtrMembers >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );

   // serialize the original object
   TestObject obj1( 7 );
   TestObject obj2( 13 );
   ObjWithRefPtrMembers* obj3 = new ObjWithRefPtrMembers( &obj1, &obj2 );
   ReflectionSaver saver( outStream );

   saver.save( obj3 );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   ObjWithRefPtrMembers* restoredObject = loader.getNextObject< ObjWithRefPtrMembers >();

   CPPUNIT_ASSERT( restoredObject != NULL );

   CPPUNIT_ASSERT( restoredObject->m_obj1 != NULL );
   CPPUNIT_ASSERT( restoredObject->m_obj2 != NULL );

   CPPUNIT_ASSERT_EQUAL( obj1.getReferencesCount(), restoredObject->m_obj1->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( obj2.getReferencesCount(), restoredObject->m_obj2->getReferencesCount() );

   CPPUNIT_ASSERT_EQUAL( 7, restoredObject->m_obj1->m_val );
   CPPUNIT_ASSERT_EQUAL( 13, restoredObject->m_obj2->m_val );

   // cleanup
   restoredObject->m_obj1->removeReference();
   restoredObject->m_obj2->removeReference();
   delete restoredObject;
   delete obj3;
}

///////////////////////////////////////////////////////////////////////////////