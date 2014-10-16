#include "ObjectPropertyEditor.h"
#include "ReferencePropertyEditor.h"
#include "ReflectionObjectInstantiator.h"
#include <QtWidgets\QVBoxLayout>


///////////////////////////////////////////////////////////////////////////////

ObjectPropertyEditor::ObjectPropertyEditor( TEditableReflectionProperty< ReflectionObject* >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
{
}

///////////////////////////////////////////////////////////////////////////////

ObjectPropertyEditor::~ObjectPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void ObjectPropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QVBoxLayout* layout = new QVBoxLayout( frame );
   layout->setSpacing(0);
   layout->setMargin(0);
   frame->setLayout( layout );
   addWidget( frame );

   if ( m_property->isReference() )
   {
      m_editor = new ReferencePropertyEditor( this, m_property );
   }
   else
   {
      m_editor = new ReflectionObjectInstantiator( this, m_property );
   }
   layout->addWidget( m_editor );
}

///////////////////////////////////////////////////////////////////////////////

void ObjectPropertyEditor::onPropertyChanged()
{
   m_editor->onPropertyChanged();
}

///////////////////////////////////////////////////////////////////////////////
