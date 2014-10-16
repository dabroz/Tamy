#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionEnum.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionSaver.h"
#include "core\ReflectionLoader.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionPropertyEditor.h"
#include "core\ReflectionPropertiesView.h"
#include "core\Resource.h"
#include "core\InArrayStream.h"
#include "core\OutArrayStream.h"
#include "core\PatchesDB.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

enum WeekEnd
{
   Sunday = 1,
   Saturday = 7
};
template<> void TReflectionEnum< WeekEnd >::registerEnumerators()
{
   registerEnumerator( Sunday, "Sunday" );
   registerEnumerator( Saturday, "Saturday" );
}

// ----------------------------------------------------------------------------

enum WeekDays
{
   Monday,
   Tuesday,
   Wensday,
   Thursday,
   Friday
};
template<> void TReflectionEnum< WeekDays >::registerEnumerators()
{
   registerEnumerator( Monday, "Monday" );
   registerEnumerator( Tuesday, "Tuesday" );
   registerEnumerator( Wensday, "Wensday" );
   registerEnumerator( Thursday, "Thursday" );
   registerEnumerator( Friday, "Friday" );
}
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( WeekDays );

// ----------------------------------------------------------------------------

namespace // anonymous
{
   struct TestEnumClass : public ReflectionObject
   {
      DECLARE_STRUCT()

      WeekDays      m_val;

      TestEnumClass( WeekDays val = Monday ) : m_val( val ) {}
   };
   BEGIN_OBJECT( TestEnumClass );
      PROPERTY_EDIT( "val", WeekDays, m_val );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class PropertiesViewMock : public ReflectionPropertiesView
   {
   };

   // -------------------------------------------------------------------------

   class EditorMock : public ReflectionPropertyEditor
   {
   private:
      TEditableReflectionProperty< ReflectionEnum >*         m_property;

   public:
      EditorMock( TEditableReflectionProperty< ReflectionEnum >* property ) 
         : m_property( property )
      {
      }

      ~EditorMock()
      {
         delete m_property;
      }

      void initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parentEditor ) {}

      void deinitialize( ReflectionObjectEditor* parentEditor ) {}

      void onPropertyChanged() {}

      void setValue( ReflectionEnum val )
      {
         m_property->set( val );
      }
   };

}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( WeekEnd )
DEFINE_TYPE_ID( WeekDays )
DEFINE_TYPE_ID( TestEnumClass )

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionEnum, simpleEnum )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addEnumType< WeekEnd >( "WeekEnd" );

   TReflectionEnum< WeekEnd >* weekendEnum = dynamic_cast< TReflectionEnum< WeekEnd >* >( typesRegistry.find< WeekEnd>() );
   CPPUNIT_ASSERT( NULL != weekendEnum );

   const std::string &str = weekendEnum->from( Saturday );
   CPPUNIT_ASSERT_EQUAL( std::string( "Saturday" ), str );

   WeekEnd w;
   weekendEnum->to( "Sunday", w );
   CPPUNIT_ASSERT_EQUAL( w, Sunday );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionEnum, valuesEnumeration )
{
   std::vector< std::string > values;

   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addEnumType< WeekEnd >( "WeekEnd" );
   typesRegistry.addEnumType< WeekDays >( "WeekDays" );

   {
      TReflectionEnum< WeekEnd >* weekendEnum = dynamic_cast< TReflectionEnum< WeekEnd >* >( typesRegistry.find< WeekEnd>() );
      CPPUNIT_ASSERT( NULL != weekendEnum );

      weekendEnum->getEnumerators( values );
      CPPUNIT_ASSERT_EQUAL( (unsigned int)2, values.size() );
      CPPUNIT_ASSERT_EQUAL( std::string( "Sunday" ), values[0] );
      CPPUNIT_ASSERT_EQUAL( std::string( "Saturday" ), values[1] );

      values.clear();
   }
   
   {
      TReflectionEnum< WeekDays >* weekdaysEnum = dynamic_cast< TReflectionEnum< WeekDays >* >( typesRegistry.find< WeekDays>() );
      CPPUNIT_ASSERT( NULL != weekdaysEnum );

      weekdaysEnum->getEnumerators( values );
      CPPUNIT_ASSERT_EQUAL( (unsigned int)5, values.size() );
      CPPUNIT_ASSERT_EQUAL( std::string( "Monday" ), values[0] );
      CPPUNIT_ASSERT_EQUAL( std::string( "Tuesday" ), values[1] );
      CPPUNIT_ASSERT_EQUAL( std::string( "Wensday" ), values[2] );
      CPPUNIT_ASSERT_EQUAL( std::string( "Thursday" ), values[3] );
      CPPUNIT_ASSERT_EQUAL( std::string( "Friday" ), values[4] );
   }

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionEnum, enumMembers )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addEnumType< WeekDays >( "WeekDays" );
   typesRegistry.addSerializableType< Resource >( "Resource", NULL );
   typesRegistry.addSerializableType< TestEnumClass >( "TestEnumClass", new TSerializableTypeInstantiator< TestEnumClass >() );

   // setup patches DB
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   typesRegistry.build( patchesDB );

   // prepare the serializers
   Array< byte > memBuf;
   InArrayStream inStream( memBuf );
   OutArrayStream outStream( memBuf );
   ReflectionSaver saver( outStream );

   // serialize the object
   TestEnumClass obj( Wensday ); 
   saver.save( &obj );
   saver.flush();

   // restore the object
   ReflectionLoader loader;
   loader.deserialize( inStream );
   TestEnumClass* restoredObject = loader.getNextObject< TestEnumClass >();
   CPPUNIT_ASSERT( restoredObject != NULL );
   CPPUNIT_ASSERT_EQUAL( Wensday, restoredObject->m_val );

   // cleanup
   delete restoredObject;
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionEnum, enumProperties )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addEnumType< WeekDays >( "WeekDays" );
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< TestEnumClass >( "TestEnumClass", new TSerializableTypeInstantiator< TestEnumClass >() );

   // create the properties viewer
   PropertiesViewMock propertiesView;
   propertiesView.defineEnums< EditorMock >();

   // create an object and view its properties
   TestEnumClass obj;

   CPPUNIT_ASSERT( NULL == propertiesView.getRootEditor() );

   obj.viewProperties( propertiesView );
   CPPUNIT_ASSERT( NULL != propertiesView.getRootEditor() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, propertiesView.getRootEditor()->getPropertiesCount() );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionEnum, castTests )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addEnumType< WeekDays >( "WeekDays" );
   typesRegistry.addEnumType< WeekEnd >( "WeekEnd" );
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );

   ReflectionType& genericEnumType = typesRegistry.getGenericEnumType();
   TReflectionEnum< WeekEnd >* weekendEnum = dynamic_cast< TReflectionEnum< WeekEnd >* >( typesRegistry.find< WeekEnd>() );
   TReflectionEnum< WeekDays >* weekdaysEnum = dynamic_cast< TReflectionEnum< WeekDays >* >( typesRegistry.find< WeekDays>() );

   // isA tests
   {
      CPPUNIT_ASSERT( weekendEnum->isA( genericEnumType ) );
      CPPUNIT_ASSERT( weekdaysEnum->isA( genericEnumType ) );

      CPPUNIT_ASSERT( !genericEnumType.isA( *weekendEnum ) );
      CPPUNIT_ASSERT( !genericEnumType.isA( *weekdaysEnum ) );

      CPPUNIT_ASSERT( !weekendEnum->isA( *weekdaysEnum ) );
      CPPUNIT_ASSERT( !weekdaysEnum->isA( *weekendEnum ) );

      CPPUNIT_ASSERT( weekendEnum->isA( *weekendEnum ) );
      CPPUNIT_ASSERT( weekdaysEnum->isA( *weekdaysEnum ) );
   }

   // isExactlyA tests
   {
      CPPUNIT_ASSERT( !weekendEnum->isExactlyA( genericEnumType ) );
      CPPUNIT_ASSERT( !weekdaysEnum->isExactlyA( genericEnumType ) );

      CPPUNIT_ASSERT( !genericEnumType.isExactlyA( *weekendEnum ) );
      CPPUNIT_ASSERT( !genericEnumType.isExactlyA( *weekdaysEnum ) );

      CPPUNIT_ASSERT( !weekendEnum->isExactlyA( *weekdaysEnum ) );
      CPPUNIT_ASSERT( !weekdaysEnum->isExactlyA( *weekendEnum ) );

      CPPUNIT_ASSERT( weekendEnum->isExactlyA( *weekendEnum ) );
      CPPUNIT_ASSERT( weekdaysEnum->isExactlyA( *weekdaysEnum ) );
   }

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////
