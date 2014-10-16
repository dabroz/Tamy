#include "ReferencePropertyEditor.h"
#include "DropArea.h"
#include "ReflectionObjectPtrMimeData.h"
#include "core.h"
#include "tamyeditor.h"
#include <QtWidgets\QToolButton>
#include "ObjectNameGetter.h"


///////////////////////////////////////////////////////////////////////////////

ReferencePropertyEditor::ReferencePropertyEditor( QWidget* parentWidget, TEditableReflectionProperty< ReflectionObject* >* property )
   : ReflectionObjectPropertyEditorBase( parentWidget )
   , m_property( property )
   , m_objPtr( NULL )
   , m_referenceDropArea( NULL )
{
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

ReferencePropertyEditor::~ReferencePropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void ReferencePropertyEditor::initialize()
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   // what type does the reference support
   const SerializableReflectionType& acceptedBaseType = static_cast< const SerializableReflectionType& >( m_property->getType() );
   
   // a field that shows the resource
   m_referenceDropArea = new DropArea( this, new ReflectionObjectPtrMimeData( acceptedBaseType, m_objPtr ) );
   connect( m_referenceDropArea, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_referenceDropArea->setMinimumSize( 20, 20 );
   m_referenceDropArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_referenceDropArea );

   // a button for NULLing the resource
   QToolButton* eraseButton = new QToolButton( this );
   eraseButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
   eraseButton->setToolTip( "Reset resource value" );
   connect( eraseButton, SIGNAL( clicked( bool ) ), this, SLOT( valErased( bool ) ) );
   eraseButton->setMinimumSize( 20, 20 );
   eraseButton->setMaximumSize( 20, 20 );
   eraseButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( eraseButton );

   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void ReferencePropertyEditor::refreshPropertyName()
{
   ReflectionObject* obj = m_property->get();
   if ( obj )
   {
      QString strId = ObjectNameGetter::toString( obj );
      m_referenceDropArea->setText( strId );
   }
   else
   {
      m_referenceDropArea->setText( "<NULL>" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReferencePropertyEditor::valChanged()
{
   m_property->set( m_objPtr );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void ReferencePropertyEditor::valErased( bool ) 
{
   m_property->set( NULL );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void ReferencePropertyEditor::onPropertyChanged()
{
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////
