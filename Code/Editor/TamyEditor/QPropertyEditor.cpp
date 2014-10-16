#include "QPropertyEditor.h"
#include "QReflectionObjectEditor.h"
#include <QtWidgets\QBoxLayout>
#include <QtWidgets\QFrame>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

QPropertyEditor::QPropertyEditor( const QString& propertyLabel )
{
   m_layout = new QVBoxLayout( this );
   m_layout->setAlignment( Qt::AlignTop );
   m_layout->setSpacing(1);
   m_layout->setMargin(2);


   if( propertyLabel.isEmpty() == false )
   {
      QLabel* label = new QLabel( propertyLabel, this );
      label->setPalette( QPalette( qRgb( 191, 191, 191 ) ) ); // <editor_settings.todo> property name color
      label->setAutoFillBackground(true);

      label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
      m_layout->addWidget( label );
   }
}

///////////////////////////////////////////////////////////////////////////////

QPropertyEditor::~QPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void QPropertyEditor::initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parent )
{
   QReflectionObjectEditor* parentEd = static_cast< QReflectionObjectEditor* >( parent );
   parentEd->addPropertyEditor( this );
   onInitialize();
}

///////////////////////////////////////////////////////////////////////////////

void QPropertyEditor::deinitialize( ReflectionObjectEditor* parent )
{
   QReflectionObjectEditor* parentEd = static_cast< QReflectionObjectEditor* >( parent );
   parentEd->removePropertyEditor(*this);
}

///////////////////////////////////////////////////////////////////////////////

void QPropertyEditor::addWidget( QWidget* widget )
{
   widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   m_layout->addWidget(widget);
}

///////////////////////////////////////////////////////////////////////////////

void QPropertyEditor::removeWidget( QWidget& widget )
{
   m_layout->removeWidget( &widget );
}

///////////////////////////////////////////////////////////////////////////////
