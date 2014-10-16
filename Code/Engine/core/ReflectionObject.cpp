#include "core.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionType.h"
#include "core\ReflectionPropertiesView.h"
#include "core\ReflectionProperty.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\ReflectionObjectReferencesManager.h"
#include "core\ReflectionTypeComponent.h"
#include "core\CriticalSection.h"
#include "core\ListUtils.h"
#include "core\LocalList.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( ReflectionObject )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

ReflectionObject::ReflectionObject( const char* uniqueId ) 
   : m_uniqueId( uniqueId ? uniqueId : "" ) 
   , m_referencesListener( NULL )
   , m_referencesCounterLock( new CriticalSection() )
   , m_referencesCounter( 1 )
{}

///////////////////////////////////////////////////////////////////////////////

ReflectionObject::ReflectionObject( const ReflectionObject& rhs )
   : m_uniqueId( rhs.m_uniqueId ) 
   , m_referencesListener( NULL )
   , m_referencesCounterLock( new CriticalSection() )
   , m_referencesCounter( 1 )
{
   // do not copy listeners - one has to attach them on his own
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObject::~ReflectionObject()
{
   ASSERT_MSG( m_referencesCounter <= 1, "There are still some references to the deleted object" );

   // notify the listeners
   for ( List< ReflectionObjectChangeListener* >::iterator it = m_listener.begin(); !it.isEnd(); ++it )
   {
      ReflectionObjectChangeListener* listener = *it;
      listener->onObservedObjectDeleted( this );
   }
   m_listener.clear();

   delete m_referencesCounterLock;
   m_referencesCounterLock = NULL;

   delete m_referencesListener;
   m_referencesListener = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionObject::isExactlyA( const SerializableReflectionType& referenceType ) const
{
   const SerializableReflectionType& thisType = getVirtualRTTI();
   return thisType.isExactlyA( referenceType );
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionObject::isA( const SerializableReflectionType& referenceType ) const
{
   const SerializableReflectionType& thisType = getVirtualRTTI();
   return thisType.isA( referenceType );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::viewProperties( ReflectionPropertiesView& view )
{
   view.set( *this );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::attachListener( ReflectionObjectChangeListener& listener ) const
{
   if ( m_referencesCounter <= 0)
   {
      // the object's going out of scope - ignore this
      return;
   }

   List< ReflectionObjectChangeListener* >::iterator it = ListUtils::find( m_listener, &listener );
   if ( it.isEnd() )
   {
      m_listener.pushBack( &listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::detachListener( ReflectionObjectChangeListener& listener ) const
{
   if ( m_referencesCounter <= 0)
   {
      // the object's going out of scope - ignore this
      return;
   }

   List< ReflectionObjectChangeListener* >::iterator it = ListUtils::find( m_listener, &listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

ReflectionProperty* ReflectionObject::getProperty( const std::string& propertyName )
{
   // find the field in this or the parent types
   ReflectionTypeComponent* propertyField = NULL;
   {
      LocalList< const SerializableReflectionType* > types;
      types.pushBack( &getVirtualRTTI() );

      while( !types.empty() )
      {
         const SerializableReflectionType* analyzedType = types.front();
         types.popFront();

         propertyField = analyzedType->findMemberField( propertyName );
         if ( propertyField != NULL )
         {
            // found it !!!
            break;
         }

         // collect parent types for further analysis
         analyzedType->collectParents( types );
      }
   }

   if ( propertyField )
   {
      // <optimization.todo>: we're allocating a property just to delete it in a sec.
      // Do that in a dedicated memory pool
      ReflectionProperty* property = propertyField->createProperty( this );
      return property;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::notifyPrePropertyChange( ReflectionProperty& property )
{
   // notify the object itself
   onPrePropertyChanged( property );

   // notify the listeners
   for ( List< ReflectionObjectChangeListener* >::iterator it = m_listener.begin(); !it.isEnd(); ++it )
   {
      ReflectionObjectChangeListener* listener = *it;
      listener->onObservedPrePropertyChanged( property );
   }

   // if we're have any references, update them
   if ( m_referencesListener )
   {
      m_referencesListener->onPrePropertyChanged( property );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::notifyPropertyChange( ReflectionProperty& property )
{
   // notify the object itself
   onPropertyChanged( property );
   
   // notify the listeners
   for ( List< ReflectionObjectChangeListener* >::iterator it = m_listener.begin(); !it.isEnd(); ++it )
   {
      ReflectionObjectChangeListener* listener = *it;
      listener->onObservedPropertyChanged( property );
   }
   
   // if we're have any references, update them
   if ( m_referencesListener )
   {
      m_referencesListener->onPropertyChanged( property );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::notifyObjectLoaded()
{
   // scan for reference properties and start observing them
   registerReferences();

   // notify the implementation that it's just been loaded
   onObjectLoaded();
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::registerReferences()
{
   delete m_referencesListener;
   m_referencesListener = NULL;

   // collect all member field
   LocalList< const ReflectionTypeComponent* > memberFields;
   const SerializableReflectionType& analyzedType = getVirtualRTTI();
   analyzedType.collectMemberFields( memberFields, ReflectionTypeComponent::RTC_Reference );

   if ( !memberFields.empty() )
   {
      m_referencesListener = new ReflectionObjectReferencesManager( this, memberFields );
   }
}

///////////////////////////////////////////////////////////////////////////////

int ReflectionObject::getReferencesCount() const 
{ 
   m_referencesCounterLock->enter();
   int refCounter = m_referencesCounter;
   m_referencesCounterLock->leave();

   return refCounter; 
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::addReference() const
{
   m_referencesCounterLock->enter();
   ++m_referencesCounter;
   m_referencesCounterLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObject::removeReference() const
{
   m_referencesCounterLock->enter();
   if ( m_referencesCounter >= 1 )
   {
      --m_referencesCounter;
   }

   int refCounter = m_referencesCounter;
   m_referencesCounterLock->leave();


   if ( refCounter < 1 )
   {
      delete this;
   }
}

///////////////////////////////////////////////////////////////////////////////
