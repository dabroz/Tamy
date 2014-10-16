#include "core\ReflectionObjectReferencesManager.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionTypeComponent.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionObjectReferencesManager::ReflectionObjectReferencesManager( ReflectionObject* host, const List< const ReflectionTypeComponent* >& memberFields  )
   : m_host( host )
   , m_memberFields( memberFields )
{
   for ( List< const ReflectionTypeComponent* >::iterator it = m_memberFields.begin(); !it.isEnd(); ++it )
   {
      const ReflectionTypeComponent* field = *it;
      ReflectionObject* object = *( (ReflectionObject**)field->getPtrToField( m_host ) );
      if ( object )
      {
         object->attachListener( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObjectReferencesManager::~ReflectionObjectReferencesManager()
{
   for ( List< const ReflectionTypeComponent* >::iterator it = m_memberFields.begin(); !it.isEnd(); ++it )
   {
      const ReflectionTypeComponent* field = *it;
      ReflectionObject* object = *( (ReflectionObject**)field->getPtrToField( m_host ) );
      if ( object )
      {
         object->detachListener( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectReferencesManager::onPrePropertyChanged( ReflectionProperty& property )
{
   uint fieldId = property.getId();

   for ( List< const ReflectionTypeComponent* >::iterator it = m_memberFields.begin(); !it.isEnd(); ++it )
   {
      const ReflectionTypeComponent* field = *it;
      if ( field->m_id == fieldId )
      {
         // found the field - stop observing the object it holds
         ReflectionObject* object = *( (ReflectionObject**)field->getPtrToField( m_host ) );
         if ( object )
         {
            object->detachListener( *this );
         }

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectReferencesManager::onPropertyChanged( ReflectionProperty& property )
{
   uint fieldId = property.getId();

   for ( List< const ReflectionTypeComponent* >::iterator it = m_memberFields.begin(); !it.isEnd(); ++it )
   {
      const ReflectionTypeComponent* field = *it;
      if ( field->m_id == fieldId )
      {
         // found the field - start observing the object it holds
         ReflectionObject* object = *( (ReflectionObject**)field->getPtrToField( m_host ) );
         if ( object )
         {
            object->attachListener( *this );
         }

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectReferencesManager::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   for ( List< const ReflectionTypeComponent* >::iterator it = m_memberFields.begin(); !it.isEnd(); ++it )
   {
      const ReflectionTypeComponent* field = *it;
      ReflectionObject** object = (ReflectionObject**)field->getPtrToField( m_host );
      if ( *object == deletedObject )
      {
         // reset a reference to this 
         ReflectionProperty* property = field->createProperty( m_host );
         m_host->notifyPrePropertyChange( *property );

         *object = NULL;

         m_host->notifyPropertyChange( *property );
         delete property;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
