#include "core-TestFramework\TestFramework.h"
#include "core\InArrayStream.h"
#include "core\OutArrayStream.h"
#include "core\ReflectionTypeComponent.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionSaver.h"
#include "core\ReflectionLoader.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionObjectsTracker.h"
#include "core\ReflectionPropertiesView.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   // -------------------------------------------------------------------------

   struct TestClass : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_val1;
      int      m_val2;

      TestClass( int val1 = 0, int val2 = 0, const char* id = NULL ) : ReflectionObject( id ), m_val1( val1 ), m_val2( val2 ) {}
      virtual ~TestClass() {}
   };
   BEGIN_OBJECT( TestClass );
      PARENT( ReflectionObject);
      PROPERTY( int, m_val1 );
      PROPERTY( int, m_val2 );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct DerivedTestClass : public TestClass
   {
      DECLARE_STRUCT()

      int      m_val3;

      DerivedTestClass( int val1 = 0, int val2 = 0, int val3 = 0 ) : TestClass( val1, val2 ), m_val3( val3 ) {}
   };
   BEGIN_OBJECT( DerivedTestClass );
   PARENT( TestClass );
   PROPERTY( int, m_val3 );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct TestClassWithPointers : public ReflectionObject
   {
      DECLARE_STRUCT()

      int                           m_val;
      TestClassWithPointers*        m_ptr;

      TestClassWithPointers( int val = 0, TestClassWithPointers* ptr = NULL ) : m_val( val ), m_ptr( ptr ) {}
      ~TestClassWithPointers() { delete m_ptr; m_ptr = NULL; }
   };
   BEGIN_OBJECT( TestClassWithPointers );
   PROPERTY( int, m_val );
   PROPERTY( TestClassWithPointers*, m_ptr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct TestClassWithPODArray : public ReflectionObject
   {
      DECLARE_STRUCT()

      std::vector< int >                  m_arr;
   };
   BEGIN_OBJECT( TestClassWithPODArray );
   PROPERTY( std::vector< int >, m_arr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct TestClassWithPtrArray : public ReflectionObject
   {
      DECLARE_STRUCT()

      std::vector< TestClass* >           m_arr;

      ~TestClassWithPtrArray()
      {
         uint count = m_arr.size();
         for ( uint i = 0; i < count; ++i )
         {
            delete m_arr[i];
         }
         m_arr.clear();
      }
   };
   BEGIN_OBJECT( TestClassWithPtrArray );
   PROPERTY( std::vector< TestClass* >, m_arr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct TestClassWithSharedPointers : public ReflectionObject
   {
      DECLARE_STRUCT()

      TestClass*        m_ptr;

      TestClassWithSharedPointers( const char* id = NULL, TestClass* ptr = NULL ) : ReflectionObject( id ), m_ptr( ptr ) {}
      ~TestClassWithSharedPointers() { m_ptr = NULL; }
   };
   BEGIN_OBJECT( TestClassWithSharedPointers );
   PROPERTY( TestClassWithSharedPointers*, m_ptr );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct TestClassNotifiable : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_valOrig;
      int      m_valPreSave;
      int      m_valPostLoad;

      TestClassNotifiable( int val = 0 ) : m_valOrig( val ), m_valPreSave( 0 ), m_valPostLoad( 0 ) {}
      virtual ~TestClassNotifiable() {}

      // ----------------------------------------------------------------------
      // ReflectionObject implementation
      // ----------------------------------------------------------------------
      void onObjectPreSave() { m_valPreSave = 1; }
      void onObjectLoaded() { m_valPostLoad = 1; }
   };
   BEGIN_OBJECT( TestClassNotifiable );
   PROPERTY( int, m_valOrig );
   PROPERTY( int, m_valPreSave );
   PROPERTY( int, m_valPostLoad );
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

DEFINE_TYPE_ID( TestClass )
DEFINE_TYPE_ID( TestClassNotifiable )
DEFINE_TYPE_ID( TestClassWithSharedPointers )
DEFINE_TYPE_ID( TestClassWithPtrArray )
DEFINE_TYPE_ID( TestClassWithPODArray )
DEFINE_TYPE_ID( TestClassWithPointers )
DEFINE_TYPE_ID( DerivedTestClass )

///////////////////////////////////////////////////////////////////////////////

TEST( Reflection, isExactlyA )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< TestClass >( "TestClass", new TSerializableTypeInstantiator< TestClass >() );
   typesRegistry.addSerializableType< DerivedTestClass >( "DerivedTestClass", new TSerializableTypeInstantiator< DerivedTestClass >() );

   DerivedTestClass instA;
   TestClass instB;

   CPPUNIT_ASSERT( instA.isExactlyA< DerivedTestClass >() );
   CPPUNIT_ASSERT( !instA.isExactlyA< TestClass >() );
   CPPUNIT_ASSERT( !instB.isExactlyA< DerivedTestClass >() );
   CPPUNIT_ASSERT( instB.isExactlyA< TestClass >() );

   CPPUNIT_ASSERT( instA.isExactlyA( SerializableReflectionType( "DerivedTestClass" ) ) );
   CPPUNIT_ASSERT( !instA.isExactlyA( SerializableReflectionType( "TestClass" ) ) );
   CPPUNIT_ASSERT( !instB.isExactlyA( SerializableReflectionType( "DerivedTestClass" ) ) );
   CPPUNIT_ASSERT( instB.isExactlyA( SerializableReflectionType( "TestClass" ) ) );

   // cleanup - and now we don't need to delete the restored objects ourselves - the tracker will take care of that
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( Reflection, isA )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< TestClass >( "TestClass", new TSerializableTypeInstantiator< TestClass >() );
   typesRegistry.addSerializableType< DerivedTestClass >( "DerivedTestClass", new TSerializableTypeInstantiator< DerivedTestClass >() );
   typesRegistry.addSerializableType< TestClassWithPODArray >( "TestClassWithPODArray", new TSerializableTypeInstantiator< TestClassWithPODArray >() );

   DerivedTestClass instA;
   TestClass instB;
   TestClassWithPODArray instC;

   CPPUNIT_ASSERT( instA.isA< DerivedTestClass >() );
   CPPUNIT_ASSERT( instA.isA< TestClass >() );
   CPPUNIT_ASSERT( !instA.isA< TestClassWithPODArray >() );

   CPPUNIT_ASSERT( !instB.isA< DerivedTestClass >() );
   CPPUNIT_ASSERT( instB.isA< TestClass >() );
   CPPUNIT_ASSERT( !instB.isA< TestClassWithPODArray >() );

   CPPUNIT_ASSERT( !instC.isA< DerivedTestClass >() );
   CPPUNIT_ASSERT( !instC.isA< TestClass >() );
   CPPUNIT_ASSERT( instC.isA< TestClassWithPODArray >() );

   CPPUNIT_ASSERT( instA.isA( SerializableReflectionType( "DerivedTestClass" ) ) );
   CPPUNIT_ASSERT( instA.isA( SerializableReflectionType( "TestClass" ) ) );
   CPPUNIT_ASSERT( !instA.isA( SerializableReflectionType( "TestClassWithPODArray" ) ) );

   CPPUNIT_ASSERT( !instB.isA( SerializableReflectionType( "DerivedTestClass" ) ) );
   CPPUNIT_ASSERT( instB.isA( SerializableReflectionType( "TestClass" ) ) );
   CPPUNIT_ASSERT( !instB.isA( SerializableReflectionType( "TestClassWithPODArray" ) ) );

   CPPUNIT_ASSERT( !instC.isA( SerializableReflectionType( "DerivedTestClass" ) ) );
   CPPUNIT_ASSERT( !instC.isA( SerializableReflectionType( "TestClass" ) ) );
   CPPUNIT_ASSERT( instC.isA( SerializableReflectionType( "TestClassWithPODArray" ) ) );

   // cleanup - and now we don't need to delete the restored objects ourselves - the tracker will take care of that
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( Reflection, primitiveTypes )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addExternalType< int >( "int" );
   typesRegistry.addExternalType< bool >( "bool" );
   typesRegistry.addSerializableType< TestClass >( "TestClass", new TSerializableTypeInstantiator< TestClass >() );

   ReflectionType* intType = typesRegistry.find< int >();
   CPPUNIT_ASSERT( NULL != intType );

   ReflectionType* boolType = typesRegistry.find< bool >();
   CPPUNIT_ASSERT( NULL != boolType );

   ReflectionType* testClassType = typesRegistry.find< TestClass >();
   CPPUNIT_ASSERT( NULL != testClassType );

   // this one wasn't defined
   ReflectionType* testNotifiableClassType = typesRegistry.find< TestClassNotifiable >();
   CPPUNIT_ASSERT( NULL == testNotifiableClassType );

   // and neither was this one
   ReflectionType* uintType = typesRegistry.find< uint >();
   CPPUNIT_ASSERT( NULL == uintType );

   CPPUNIT_ASSERT( intType->isExactlyA( *boolType ) == false );
   CPPUNIT_ASSERT( intType->isA( *boolType ) == false );

   CPPUNIT_ASSERT( boolType->isExactlyA( *boolType ) == true );
   CPPUNIT_ASSERT( boolType->isA( *boolType ) == true );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( Reflection, casts )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< TestClass >( "TestClass", new TSerializableTypeInstantiator< TestClass >() );
   typesRegistry.addSerializableType< DerivedTestClass >( "DerivedTestClass", new TSerializableTypeInstantiator< DerivedTestClass >() );

   DerivedTestClass derrivedTestClassObj;

   TestClass* testClassPtr = DynamicCast< TestClass >( &derrivedTestClassObj );
   CPPUNIT_ASSERT( NULL != testClassPtr );

   DerivedTestClass* derrivedTestClassPtr = DynamicCast< DerivedTestClass >( &derrivedTestClassObj );
   CPPUNIT_ASSERT( NULL != derrivedTestClassPtr );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( Reflection, reflectionObjectClassCanBeReflectedUpon )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< TestClass >( "TestClass", new TSerializableTypeInstantiator< TestClass >() );

   TestClass testClassObj;
   CPPUNIT_ASSERT( testClassObj.isA< ReflectionObject >() );
   
   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////
