#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionProperty.h"
#include "core\ReflectionType.h"
#include "core\ReflectionPropertyEditor.h"
#include "core\ReflectionPropertiesView.h"
#include "core-TestFramework\MatrixWriter.h"
#include "core\ReflectionObject.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class PropertiesViewMock : public ReflectionPropertiesView
   {
   public:
      ReflectionPropertyEditor* mockCreate( ReflectionProperty* property )
      {
         return create( property );
      }
   };

   // -------------------------------------------------------------------------

   template<typename T>
   class EditorMock : public ReflectionPropertyEditor
   {
   private:
      TEditableReflectionProperty< T >*         m_property;

   public:
      EditorMock( TEditableReflectionProperty< T >* property ) 
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

      void setValue( const T& val )
      {
         m_property->set( val );
      }
   };

   // -------------------------------------------------------------------------

   struct ReflectionObjectMock : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_val; // static data

      // runtime data
      bool     m_preNotificationReceived;
      bool     m_postNotificationReceived;

      ReflectionObjectMock( int val = 0 ) : m_val( val ), m_preNotificationReceived( 0 ), m_postNotificationReceived( 0 ) {}

      void onPrePropertyChanged( ReflectionProperty& property )
      {
         m_preNotificationReceived = true;
      }

      void onPropertyChanged( ReflectionProperty& property )
      {
         m_postNotificationReceived = true;
      }

      bool wasNotified() const 
      {
         return m_preNotificationReceived && m_postNotificationReceived;
      }
   };
   BEGIN_OBJECT( ReflectionObjectMock );
      PARENT( ReflectionObject );
      PROPERTY_EDIT( "m_val", int, m_val );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct MockObjWithTwoVectors : public ReflectionObject
   {
      DECLARE_STRUCT()

      Vector      m_position;
      Vector      m_speed;

   };
   BEGIN_OBJECT( MockObjWithTwoVectors );
      PARENT( ReflectionObject );
      PROPERTY_EDIT( "m_position", Vector, m_position );
      PROPERTY_EDIT( "m_speed", Vector, m_speed );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct MockObjWithArray : public ReflectionObject
   {
      DECLARE_STRUCT()

      std::vector< ReflectionObjectMock* >      m_arr;

      ~MockObjWithArray()
      {
         for ( uint i = 0; i < m_arr.size(); ++i )
         {
            delete m_arr[i];
         }
         m_arr.clear();
      }

   };
   BEGIN_OBJECT( MockObjWithArray );
      PARENT( ReflectionObject );
      PROPERTY_EDIT( "m_arr", std::vector< ReflectionObjectMock* >, m_arr );
   END_OBJECT();

} // anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( ReflectionObjectMock );
DEFINE_TYPE_ID( MockObjWithTwoVectors );
DEFINE_TYPE_ID( MockObjWithArray );

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionPropertiesView, editors )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addExternalType< Vector >( "Vector" );
   typesRegistry.addSerializableType< ReflectionObject>( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< MockObjWithTwoVectors >( "MockObjWithTwoVectors", new TSerializableTypeInstantiator< MockObjWithTwoVectors >() );


   MockObjWithTwoVectors object;
   PropertiesViewMock view;
   view.associate< Vector, EditorMock< Vector > >();

   view.set( object );
   ReflectionObjectEditor* rootEditor = view.getRootEditor();
   CPPUNIT_ASSERT( NULL != rootEditor );

   CPPUNIT_ASSERT_EQUAL( (uint)2, rootEditor->getPropertiesCount() );

   view.reset();
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionProperties, dynamicEditorCreation )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addExternalType< int >( "int" );
   typesRegistry.addExternalType< float >( "float" );
   typesRegistry.addSerializableType< ReflectionObject>( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< ReflectionObjectMock >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock >() );

   PropertiesViewMock factory;
   factory.associate< int, EditorMock< int > >();
   factory.associate< float, EditorMock< float > >();
   factory.associateAbstract< ReflectionObjectMock*, EditorMock< ReflectionObjectMock* > >();

   int intVal;
   float floatVal;
   ReflectionObjectMock hostObject;
   ReflectionObjectMock* ptr = new ReflectionObjectMock();

   ReflectionPropertyEditor* editor = factory.mockCreate( new TReflectionProperty<int>( &hostObject, &intVal ) );
   CPPUNIT_ASSERT(NULL != editor);
   CPPUNIT_ASSERT(NULL != dynamic_cast<EditorMock<int>*> (editor));
   delete editor;

   editor = factory.mockCreate( new TReflectionProperty<float>( &hostObject, &floatVal ) );
   CPPUNIT_ASSERT(NULL != editor);
   CPPUNIT_ASSERT(NULL != dynamic_cast<EditorMock<float>*> (editor));
   delete editor;

   editor = factory.mockCreate( new TReflectionProperty< ReflectionObjectMock* >( &hostObject, &ptr ) );
   CPPUNIT_ASSERT(NULL != editor);
   CPPUNIT_ASSERT(NULL != dynamic_cast< EditorMock< ReflectionObjectMock* >* > (editor));
   delete editor;
   delete ptr;
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionProperties, access )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addExternalType< int >( "int" );
   typesRegistry.addSerializableType< ReflectionObject>( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< ReflectionObjectMock >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock >() );

   // create the properties viewer
   PropertiesViewMock propertiesView;
   propertiesView.associate< int, EditorMock< int > >();

   // create an object and view its properties
   ReflectionObjectMock obj;

   CPPUNIT_ASSERT( NULL == propertiesView.getRootEditor() );

   obj.viewProperties( propertiesView );

   CPPUNIT_ASSERT( NULL != propertiesView.getRootEditor() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, propertiesView.getRootEditor()->getPropertiesCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionProperties, observation )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addExternalType< int >( "int" );
   typesRegistry.addSerializableType< ReflectionObject>( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< ReflectionObjectMock >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock >() );

   // create the properties viewer
   PropertiesViewMock propertiesView;
   propertiesView.associate< int, EditorMock< int > >();

   // create an object and view its properties
   ReflectionObjectMock obj;
   obj.viewProperties( propertiesView );

   EditorMock< int >* editor = static_cast< EditorMock< int >* >( propertiesView.getRootEditor()->getPropertyEditor( 0 ) );
   CPPUNIT_ASSERT( editor != NULL );
   editor->setValue( 12 );

   CPPUNIT_ASSERT( obj.wasNotified() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionProperties, vectorsOfObjects )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addExternalType< int >( "int" );
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< ReflectionObjectMock >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock >() );
   typesRegistry.addSerializableType< MockObjWithArray >( "MockObjWithArray", new TSerializableTypeInstantiator< MockObjWithArray >() );

   // create an object and view its properties
   MockObjWithArray object;
   object.m_arr.push_back( new ReflectionObjectMock( 1 ) );
   object.m_arr.push_back( new ReflectionObjectMock( 2 ) );
  
   // create the properties viewer
   PropertiesViewMock propertiesView;
   propertiesView.associate< int, EditorMock< int > >();
   propertiesView.set( object );


   // top level
   ReflectionObjectEditor* objectEditor = propertiesView.getRootEditor();
   CPPUNIT_ASSERT( NULL != objectEditor );
   CPPUNIT_ASSERT_EQUAL( (uint)1, objectEditor->getPropertiesCount() );
   {
      // array property
      ReflectionPropertyEditorComposite* arrayEditor = dynamic_cast< ReflectionPropertyEditorComposite* >( objectEditor->getPropertyEditor(0) );
      CPPUNIT_ASSERT( NULL != arrayEditor );
      CPPUNIT_ASSERT_EQUAL( (uint)2, arrayEditor->getEditorsCount() );

      {
         // first object
         ReflectionObjectEditor* arrayElemEditor = arrayEditor->getEditor(0);
         CPPUNIT_ASSERT( NULL != arrayElemEditor );
         CPPUNIT_ASSERT_EQUAL( (uint)1, arrayElemEditor->getPropertiesCount() );

         {
            // and its property
            EditorMock< int >* arrayElemPropertyEditor = dynamic_cast< EditorMock< int >* >( arrayElemEditor->getPropertyEditor(0) );
            CPPUNIT_ASSERT( NULL != arrayElemPropertyEditor );
            arrayElemPropertyEditor->setValue( 10 );
            CPPUNIT_ASSERT( object.m_arr[0]->wasNotified() == true );
            CPPUNIT_ASSERT( object.m_arr[1]->wasNotified() == false );
         }
      }

      {
         // second object
         ReflectionObjectEditor* arrayElemEditor = arrayEditor->getEditor(1);
         CPPUNIT_ASSERT( NULL != arrayElemEditor );
         CPPUNIT_ASSERT_EQUAL( (uint)1, arrayElemEditor->getPropertiesCount() );

         {
            // and its property
            EditorMock< int >* arrayElemPropertyEditor = dynamic_cast< EditorMock< int >* >( arrayElemEditor->getPropertyEditor(0) );
            CPPUNIT_ASSERT( NULL != arrayElemPropertyEditor );
            arrayElemPropertyEditor->setValue( 10 );
            CPPUNIT_ASSERT( object.m_arr[0]->wasNotified() == true );
            CPPUNIT_ASSERT( object.m_arr[1]->wasNotified() == true );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
