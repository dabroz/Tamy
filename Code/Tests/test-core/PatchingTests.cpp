#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionTypeComponent.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionObject.h"
#include "core\PatchesDB.h"
#include "core\PatchRecord.h"

#include "core\InArrayStream.h"
#include "core\OutArrayStream.h"
#include "core\ReflectionSaver.h"
#include "core\ReflectionLoader.h"

#include "core\SerializedReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   void customMigrationFunc( SerializedReflectionObject& migratedObject )
   {
      const SerializedValue* oldValue = migratedObject.getValue( ReflectionTypeComponent::generateId( "m_val" ) );
      SerializedValue* newValue = migratedObject.getValue( ReflectionTypeComponent::generateId( "m_str" ) );
      if ( !oldValue || !newValue )
      {
         return;
      }

      int val;
      oldValue->initialize< int >( val );

      if ( val == 3 )
      {
         newValue->set< std::string >( "three" );
      }
      else
      {
         newValue->set< std::string >( "nothing" );
      }

   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Patching, classNameChange )
{
   // define patches
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   {
      {
         PatchRecord& record = patchesDB.addPatch( "", -1, "TestClass", 0 );
         record.addParent( "ReflectionObject", 0 );
         record.addField( "m_val1", SFT_Value );
      }

      {
         PatchRecord& record = patchesDB.addPatch( "TestClass", 0, "ModifiedTestClass", 1 );
      }
   }

   uint oldTypeId = ReflectionType::generateId( "TestClass" );
   uint expectedNewTypeId = ReflectionType::generateId( "ModifiedTestClass" );

   uint newTypeId = patchesDB.findLatestType( oldTypeId, 0 );
   CPPUNIT_ASSERT_EQUAL( expectedNewTypeId, newTypeId );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Patching, defaultDataMigration )
{
   // define patches
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   {
      {
         PatchRecord& record = patchesDB.addPatch( "", -1, "TestClass", 0 );
         record.addParent( "ReflectionObject", 0 );
         record.addField( "m_val1", SFT_Value );
      }

      {
         PatchRecord& record = patchesDB.addPatch( "TestClass", 0, "TestClass", 1 );
         record.changeField("m_val1", "m_val2" );
      }
   }

   // migrate data
   SerializedReflectionObject obj;
   obj.addValue( ReflectionTypeComponent::generateId( "m_val1" ) ).set< int >( 5 );

   uint typeId = ReflectionType::generateId( "TestClass" );
   patchesDB.migrateData( typeId, 0, obj );

   const SerializedValue* val1 = obj.getValue( ReflectionTypeComponent::generateId( "m_val1" ) );
   CPPUNIT_ASSERT( NULL == val1 );

   const SerializedValue* val2 = obj.getValue( ReflectionTypeComponent::generateId( "m_val2" ) );
   CPPUNIT_ASSERT( NULL != val2 );

   int val2_value;
   val2->initialize< int >( val2_value );
   CPPUNIT_ASSERT_EQUAL( 5, val2_value );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Patching, customPatchingFunctions )
{
   // define patches
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   patchesDB.clear();
   {
      // patch definitions
      {
         PatchRecord& record = patchesDB.addPatch( "", -1, "TestClass", 0 );
         record.addParent( "ReflectionObject", 0 );
         record.addField( "m_val", SFT_Value );
      }

      {
         PatchRecord& record = patchesDB.addPatch( "TestClass", 0, "TestClass", 1 );
         record.removeField( "m_val" );
         record.addField( "m_str", SFT_Value );
         record.defineMigrationFunc( &customMigrationFunc );
      }
   }

   // migrate data
   SerializedReflectionObject obj;
   obj.addValue( ReflectionTypeComponent::generateId( "m_val" ) ).set< int >( 3 );

   uint typeId = ReflectionType::generateId( "TestClass" );
   patchesDB.migrateData( typeId, 0, obj );

   const SerializedValue* val = obj.getValue( ReflectionTypeComponent::generateId( "m_val" ) );
   CPPUNIT_ASSERT( NULL == val );

   const SerializedValue* str = obj.getValue( ReflectionTypeComponent::generateId( "m_str" ) );
   CPPUNIT_ASSERT( NULL != str );

   std::string str_value;
   str->initialize< std::string >( str_value );

   CPPUNIT_ASSERT_EQUAL( std::string( "three" ), str_value );
}

///////////////////////////////////////////////////////////////////////////////
