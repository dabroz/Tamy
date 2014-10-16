#include "core.h"
#include "core\ReflectionPropertiesView.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionPropertiesView::ReflectionPropertiesView( bool inspectEditableOnly )
   : m_parentObject( NULL )
   , m_rootEditor( NULL )
   , m_arrayNodeCreator( new ArrayNodeCreator() )
   , m_objectNodeCreator( new ObjectNodeCreator() )
   , m_inspectEditableOnly( inspectEditableOnly )
{
}

///////////////////////////////////////////////////////////////////////////////

ReflectionPropertiesView::~ReflectionPropertiesView() 
{
   reset();

   // delete the creators
   unsigned int count = m_creators.size();
   for (unsigned int i = 0; i < count; ++i)
   {
      delete m_creators[ i ];
   }
   m_creators.clear();

   delete m_arrayNodeCreator;
   m_arrayNodeCreator = NULL;

   delete m_objectNodeCreator;
   m_objectNodeCreator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertiesView::set( ReflectionObject& parentObject )
{
   // remove the previously set editors
   reset();

   // explore the properties and create the editors tree for them
   m_rootEditor = m_objectNodeCreator->create( &parentObject );

   std::vector< ReflectionObjectEditor* > objectNodes; objectNodes.push_back( m_rootEditor );

   const ReflectionType* reflectionObjectType = TSingleton< ReflectionTypesRegistry >::getInstance().find< ReflectionObject >();
   ASSERT_MSG( reflectionObjectType != NULL, "Reflection type for ReflectionObject is not registered when it should be!" );

   std::vector< ReflectionProperty* > childrenProperties;
   while( !objectNodes.empty() )
   {
      ReflectionObjectEditor* populatedObjectEditor = objectNodes.back(); objectNodes.pop_back();
      ReflectionObject& populatedObject = populatedObjectEditor->getEditedObject();

      // collect object's properties
      childrenProperties.clear();
      const SerializableReflectionType& objectType = populatedObject.getVirtualRTTI();
      objectType.collectProperties( &populatedObject, childrenProperties );

      uint count = childrenProperties.size();
      for ( uint i = 0; i < count; ++i )
      {
         analyzeSingleProperty( childrenProperties[i], populatedObjectEditor, objectNodes );
      }
   }

   // initialize the editors tree
   m_rootEditor->initialize( this, NULL );
   onSet( m_rootEditor );

   // start listening to the object - it may be removed, at which point we want to reset
   m_parentObject = &parentObject;
   m_parentObject->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertiesView::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   m_parentObject = NULL;
   reset();
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertiesView::analyzeSingleProperty( ReflectionProperty* analyzedProperty, ReflectionObjectEditor* populatedObjectEditor, std::vector< ReflectionObjectEditor* >& outObjectNodes )
{
   // create the editors for the properties
   if ( m_inspectEditableOnly && !analyzedProperty->canBeEdited() )
   {
      // delete the unused property - in other case the editor will release it when it no longer needs it
      delete analyzedProperty;
      return;
   }

   // if the property reflects an array, create a composite property and put it up for population
   ReflectionPropertyEditor* editor = NULL;
   if ( analyzedProperty->isComposite() )
   {
      ReflectionPropertyArray* arrayProperty = static_cast< ReflectionPropertyArray* >( analyzedProperty );
      ReflectionPropertyEditorComposite* arrayEditor = m_arrayNodeCreator->create( arrayProperty );
      editor = arrayEditor;

      // get the children objects and register them for BFS to analyze them, as well as with
      // the array editor itself
      int count = arrayProperty->size();
      for ( int i = 0; i < count; ++i )
      {
         ReflectionObject* arrayElem = arrayProperty->getElement( i );
         if ( arrayElem == NULL )
         {
            // For now, if we encounter a NULL pointer, simply don't create an editor for it.
            // In the future, create a custom NULL -pointer object here
            continue;
         }

         ReflectionObjectEditor* arrayElemEditor = m_objectNodeCreator->create( arrayElem );
         outObjectNodes.push_back( arrayElemEditor );
         arrayEditor->addObjectEditor( arrayElemEditor );
      }
   }
   else
   {
      editor = create( analyzedProperty );
   }

   // store the editor in the parent composite editor
   if ( editor != NULL )
   {
      // set up the property id of the edited property - it will be used by the editor
      // to identify self in the process of property change notifications
      editor->m_propertyId = ReflectionTypeComponent::generateId( analyzedProperty->getName() );

      // add the editor to the object node
      populatedObjectEditor->addPropertyEditor( editor );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertiesView::reset()
{
   // delete the editors
   if ( m_rootEditor != NULL )
   {
      m_rootEditor->deinitialize( NULL );

      delete m_rootEditor;
      m_rootEditor = NULL;
   }

   // detach the listener from the object
   if ( m_parentObject )
   {
      m_parentObject->detachListener( *this );
      m_parentObject = NULL;
   }
}


///////////////////////////////////////////////////////////////////////////////

ReflectionPropertyEditor* ReflectionPropertiesView::create( ReflectionProperty* property )
{
   const ReflectionType* checkedClassType = property->getVirtualClass();
   if ( !checkedClassType )
   {
      return NULL;
   }

   for ( CreatorsVec::iterator it = m_creators.begin();  it != m_creators.end(); ++it )
   {
      Creator& creator = **it;
      if ( creator.doesMatch( *checkedClassType ) )
      {
         ReflectionPropertyEditor* editor = creator( property );
         return editor;
      }
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
