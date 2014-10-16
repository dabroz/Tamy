#include "Blender1DParamEditor.h"
#include <QtWidgets\QDoubleSpinBox>
#include <QtWidgets\QHBoxLayout>


///////////////////////////////////////////////////////////////////////////////

Blender1DParamEditor::Blender1DParamEditor( QWidget* parent )
   : QFrame( parent )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_val = new QDoubleSpinBox( this ); 
   layout->addWidget( m_val );
   m_val->setDecimals( 6 );
   m_val->setSingleStep( 0.01 );
   m_val->setRange( -FLT_MAX, FLT_MAX );
}

///////////////////////////////////////////////////////////////////////////////

Blender1DParamEditor::~Blender1DParamEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamEditor::initialize( float parameterValue )
{
   m_val->setValue( parameterValue );
}

///////////////////////////////////////////////////////////////////////////////

float Blender1DParamEditor::getValue() const
{
   return m_val->value();
}

///////////////////////////////////////////////////////////////////////////////
