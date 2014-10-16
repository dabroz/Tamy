#include "QReflectionObjectEditor.h"
#include "QReflectionPropertyEditorComposite.h"
#include "core/ReflectionObject.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

QReflectionObjectEditor::QReflectionObjectEditor( ReflectionObject* editedObject )
   : ReflectionObjectEditor( editedObject )
{
   m_layout = new QVBoxLayout( this );
   m_layout->setAlignment( Qt::AlignTop );
   m_layout->setSpacing(0);
   m_layout->setMargin(1);

   setFrameStyle( QFrame::StyledPanel );

   // set the caption with the edited object's class name
   QString propertyLabel( editedObject->getVirtualRTTI().m_name.c_str() );
   if ( propertyLabel.isEmpty() == false )
   {
      QLabel* label = new QLabel( propertyLabel, this );
      label->setPalette( QPalette( qRgb( 112, 96, 61 ) ) );  // <editor_settings.todo> object type name color
      label->setAutoFillBackground( true );

      label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
      m_layout->addWidget(label);
   }

}

///////////////////////////////////////////////////////////////////////////////

QReflectionObjectEditor::~QReflectionObjectEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionObjectEditor::initialize( ReflectionPropertiesView* parentView, ReflectionPropertyEditorComposite* parentNode )
{
   if ( parentNode )
   {
      QReflectionPropertyEditorComposite* qEditor  = static_cast< QReflectionPropertyEditorComposite* >( parentNode );
      qEditor->addPropertyEditor( this );
   }

   __super::initialize( parentView, parentNode );
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionObjectEditor::deinitialize( ReflectionPropertyEditorComposite* parentNode )
{
   __super::deinitialize( parentNode );
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionObjectEditor::addPropertyEditor( QWidget* editorWidget )
{
   editorWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   m_layout->addWidget(editorWidget);
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionObjectEditor::removePropertyEditor( QWidget& editorWidget )
{
   m_layout->removeWidget( &editorWidget );

}

///////////////////////////////////////////////////////////////////////////////
