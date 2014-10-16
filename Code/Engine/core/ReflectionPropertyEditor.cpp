#include "core.h"
#include "core\ReflectionPropertyEditor.h"
#include "core\ReflectionProperty.h"
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionPropertyEditorComposite::ReflectionPropertyEditorComposite( ReflectionPropertyArray* arrayProperty )
   : m_arrayProperty( arrayProperty )
{
}

///////////////////////////////////////////////////////////////////////////////

ReflectionPropertyEditorComposite::~ReflectionPropertyEditorComposite()
{
   uint count = m_editors.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_editors[i];
   }
   m_editors.clear();

   delete m_arrayProperty;
   m_arrayProperty = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertyEditorComposite::initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parentEditor )
{
   unsigned int count = m_editors.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ReflectionObjectEditor* editor = m_editors[i];
      editor->initialize( parentView, this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertyEditorComposite::deinitialize( ReflectionObjectEditor* parentEditor )
{
   unsigned int count = m_editors.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ReflectionObjectEditor* editor = m_editors[i];

      // deinitialize only if the derived view still exists
      // (which it may not if it's actually undergoing the destruction process)
      editor->deinitialize( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionPropertyEditorComposite::onPropertyChanged()
{
   // nothing to do in the default implementation
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ReflectionObjectEditor::ReflectionObjectEditor( ReflectionObject* editedObject )
   : m_editedObject( editedObject )
{
   // attach the listener
   m_editedObject->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObjectEditor::~ReflectionObjectEditor()
{
   // detach the listener
   if ( m_editedObject )
   {
      m_editedObject->detachListener( *this );
   }

   // destroy the editors
   uint count = m_editors.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_editors[i];
   }
   m_editors.clear();

   // don't delete the edited object - after all we were just editing it, it's still out there,
   // doing its fine job - leave it to  it ;)
   m_editedObject = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectEditor::initialize( ReflectionPropertiesView* parentView, ReflectionPropertyEditorComposite* parentComposite )
{
   unsigned int count = m_editors.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ReflectionPropertyEditor* editor = m_editors[i];
      editor->initialize( parentView, this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectEditor::deinitialize( ReflectionPropertyEditorComposite* parentComposite )
{
   unsigned int count = m_editors.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ReflectionPropertyEditor* editor = m_editors[i];
      editor->deinitialize( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   uint changedPropertyId = ReflectionTypeComponent::generateId( property.getName() );

   unsigned int count = m_editors.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ReflectionPropertyEditor* editor = m_editors[i];
      if ( editor->m_propertyId == changedPropertyId )
      {
         // found the property editor that corresponds to the changed property
         editor->onPropertyChanged();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectEditor::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   m_editedObject = NULL;
}

///////////////////////////////////////////////////////////////////////////////
