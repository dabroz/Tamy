#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionType.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\Thread.h"
#include "core\Runnable.h"
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct ReflectionObjectMock_ROT : public ReflectionObject
   {
      DECLARE_STRUCT()

      int      m_val; // static data

      // runtime data
      bool     m_preNotificationReceived;
      bool     m_postNotificationReceived;
      bool*    m_deletionListener;

      ReflectionObjectMock_ROT( int val = 0 ) 
         : m_val( val )
         , m_preNotificationReceived( 0 )
         , m_postNotificationReceived( 0 )
         , m_deletionListener( NULL ) 
      {}

      ReflectionObjectMock_ROT::~ReflectionObjectMock_ROT()
      {
         if ( m_deletionListener )
         {
            *m_deletionListener = true;
         }
      }

      void setDeletionListener( bool* wasDeleted ) 
      {
         m_deletionListener = wasDeleted;
      }

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
   BEGIN_OBJECT( ReflectionObjectMock_ROT );
      PARENT( ReflectionObject );
      PROPERTY_EDIT( "m_val", int, m_val );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct MockObjectListener : public ReflectionObjectChangeListener
   {
      bool     m_notificationReceived;

      MockObjectListener() : m_notificationReceived( false ) {}

      void onObservedPropertyChanged( ReflectionProperty& property )
      {
         m_notificationReceived = ( property.getName() == "m_val" );
      }

      void onObservedObjectDeleted( ReflectionObject* deletedObject ) {}
   };


   // -------------------------------------------------------------------------

   class ReflectionObjectMock : public ReflectionObject
   {
   };

   // -------------------------------------------------------------------------

   struct PlayWithRefCountMock : public Runnable
   {
      ReflectionObject& m_obj;


      PlayWithRefCountMock( ReflectionObject& obj )
         : m_obj( obj )
      {}

      void run()
      {
         for ( uint i = 0; i < 1000; ++i )
         {
            m_obj.addReference();

            Thread::yield();

            m_obj.removeReference();
            Thread::yield();
         }
      }
   };

   // -------------------------------------------------------------------------

   class ReferencingObjectMock : public ReflectionObject
   {
      DECLARE_CLASS()

   public:
      ReflectionObject*       m_reference;

   public:
      ReferencingObjectMock( ReflectionObject* reference = NULL )
         : m_reference( reference )
      {
         registerReferences();
      }

      void setReference( ReflectionObject* newReference )
      {
         NOTIFY_PROPERTY_CHANGE( m_reference );
         m_reference = newReference;
      }
   };
   BEGIN_OBJECT( ReferencingObjectMock );
      PARENT( ReflectionObject );
      PROPERTY_EDIT_REFERENCE( "m_reference", ReflectionObject*, m_reference );
   END_OBJECT();
}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( ReflectionObjectMock_ROT );
DEFINE_TYPE_ID( ReferencingObjectMock );

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionObject, propertyChangesObservation )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject>( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< ReflectionObjectMock_ROT >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock_ROT >() );

   ReflectionObjectMock_ROT object;
   MockObjectListener listener;
   object.attachListener( listener );

   ReflectionProperty* property = object.getProperty( "m_val" );
   object.notifyPropertyChange( *property );
   delete property;

   CPPUNIT_ASSERT( false == object.m_preNotificationReceived );
   CPPUNIT_ASSERT( true == object.m_postNotificationReceived );
   CPPUNIT_ASSERT( true == listener.m_notificationReceived );

   // cleanup
   object.detachListener( listener );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionObject, referenceCounting )
{
   bool wasDeleted = false;
   ReflectionObjectMock_ROT* object = new ReflectionObjectMock_ROT();
   object->setDeletionListener( &wasDeleted );

   CPPUNIT_ASSERT_EQUAL( 1, object->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( false, wasDeleted );

   object->addReference();
   CPPUNIT_ASSERT_EQUAL( 2, object->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( false, wasDeleted );

   object->removeReference();
   CPPUNIT_ASSERT_EQUAL( 1, object->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( false, wasDeleted );

   object->removeReference();
   CPPUNIT_ASSERT_EQUAL( true, wasDeleted );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionObject, threadSafeReferenceCounting )
{
   ReflectionObjectMock object;
   PlayWithRefCountMock oper1( object );
   PlayWithRefCountMock oper2( object );
   
   // start up threads that will play with the object's ref counter
   Thread thread1;
   Thread thread2;

   thread1.start( oper1 );
   thread2.start( oper2 );

   thread1.join();
   thread2.join();

   CPPUNIT_ASSERT_EQUAL( 1, object.getReferencesCount() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ReflectionObject, referencePropertiesAreAutomaticallyTracked )
{
   // TEST DESCRIPTION: when an object is referencing another object, it tracks
   // whether it has changed ( internally or externally ), and moreover - it tracks when
   // the referenced object was deleted.
   // So here's a simple scenario:
   //  1. we'll have two objects there that we can reference.
   //  2. we'll switch from one object to the next
   //  3. we'll delete both object ( one after another, checking how the test object reacted to those events )
   //  thus confirming that the test object reset its reference when the referenced object was destroyed

   // register necessary types
   ReflectionTypesRegistry& typesReg = TSingleton< ReflectionTypesRegistry >::getInstance();
   {
      typesReg.clear();
      typesReg.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
      typesReg.addSerializableType< ReflectionObjectMock >( "ReflectionObjectMock", new TSerializableTypeInstantiator< ReflectionObjectMock >() );
      typesReg.addSerializableType< ReferencingObjectMock >( "ReferencingObjectMock", new TSerializableTypeInstantiator< ReferencingObjectMock >() );
   }

   // we have an object our object will hold a reference to
   ReflectionObjectMock* referencedObject_1 = new ReflectionObjectMock();

   // and here's our test object
   ReferencingObjectMock* testObject = new ReferencingObjectMock( referencedObject_1 );

   // we can see that all's good here - the object we referenced is there
   CPPUNIT_ASSERT( testObject->m_reference == referencedObject_1 );

   // now let's create another object that we can reference and switch the reference from one to another
   ReflectionObjectMock* referencedObject_2 = new ReflectionObjectMock();
   testObject->setReference( referencedObject_2 );
   CPPUNIT_ASSERT( testObject->m_reference == referencedObject_2 );

   // start deleting objects

   // first the object that was previously referenced - test object should not change when we do so
   referencedObject_1->removeReference();
   CPPUNIT_ASSERT( testObject->m_reference == referencedObject_2 );

   // and now the currently referenced object - test object should reset the reference
   referencedObject_2->removeReference();
   CPPUNIT_ASSERT( testObject->m_reference == NULL );

   // cleanup
   testObject->removeReference();
   typesReg.clear();
}

///////////////////////////////////////////////////////////////////////////////
