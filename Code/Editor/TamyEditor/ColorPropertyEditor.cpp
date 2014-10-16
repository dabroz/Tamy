#include "ColorPropertyEditor.h"
#include "ColorFrame.h"
#include <QtWidgets\QPushButton>
#include <QtWidgets\QHBoxLayout>


///////////////////////////////////////////////////////////////////////////////

ColorPropertyEditor::ColorPropertyEditor( TEditableReflectionProperty< Color >* val )
   : QPropertyEditor( val->getLabel().c_str() )
   , m_val( val )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

ColorPropertyEditor::~ColorPropertyEditor()
{
   delete m_val;
   m_val = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ColorPropertyEditor::setupUi()
{
   QFrame* frame = new QFrame( this ); 
   addWidget( frame );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );

   m_colorPicker = new ColorFrame( frame, m_val->get() ); 
   {
      layout->addWidget( m_colorPicker, 1 );
      connect( m_colorPicker, SIGNAL( changed( const QColor& ) ), this, SLOT( valChanged( const QColor& ) ) );
   }

   m_selectColorBtn = new QPushButton( frame );
   {
      layout->addWidget( m_selectColorBtn, 0 );

      m_selectColorBtn->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/pickColor.png" ) );
      connect( m_selectColorBtn, SIGNAL( clicked( bool ) ), this, SLOT( pickColor( bool ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ColorPropertyEditor::valChanged( const QColor& val )
{
   m_val->set( m_colorPicker->getEngineColor() );
}

///////////////////////////////////////////////////////////////////////////////

void ColorPropertyEditor::pickColor( bool )
{
   m_colorPicker->selectColor();
}

///////////////////////////////////////////////////////////////////////////////

void ColorPropertyEditor::onPropertyChanged()
{
   m_colorPicker->setColor( m_val->get() );
}

///////////////////////////////////////////////////////////////////////////////
