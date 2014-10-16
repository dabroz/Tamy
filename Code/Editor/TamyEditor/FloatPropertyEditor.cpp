#include "FloatPropertyEditor.h"
#include <QtWidgets\QSpinBox>


///////////////////////////////////////////////////////////////////////////////

FloatPropertyEditor::FloatPropertyEditor( TEditableReflectionProperty< float >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

FloatPropertyEditor::~FloatPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void FloatPropertyEditor::setupUi()
{
   m_val = new QDoubleSpinBox(this); addWidget(m_val);
   m_val->setSingleStep( 0.1 );
   m_val->setDecimals( 6 );
   m_val->setRange( -FLT_MAX, FLT_MAX );
   m_val->setValue( m_property->get() );

   connect(m_val, SIGNAL(valueChanged(double)), this, SLOT(valChanged(double)));
}

///////////////////////////////////////////////////////////////////////////////

void FloatPropertyEditor::valChanged(double val)
{
   m_property->set( val );
}

///////////////////////////////////////////////////////////////////////////////

void FloatPropertyEditor::onPropertyChanged()
{
   m_val->setValue( m_property->get() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DoublePropertyEditor::DoublePropertyEditor( TEditableReflectionProperty< double >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

DoublePropertyEditor::~DoublePropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DoublePropertyEditor::setupUi()
{
   m_val = new QDoubleSpinBox(this); addWidget(m_val);
   m_val->setSingleStep( 0.1 );
   m_val->setRange( -FLT_MAX, FLT_MAX );
   m_val->setValue( m_property->get() );

   connect(m_val, SIGNAL(valueChanged(double)), this, SLOT(valChanged(double)));
}

///////////////////////////////////////////////////////////////////////////////

void DoublePropertyEditor::valChanged(double val)
{
   m_property->set( val );
}

///////////////////////////////////////////////////////////////////////////////

void DoublePropertyEditor::onPropertyChanged()
{
   m_val->setValue( m_property->get() );
}

///////////////////////////////////////////////////////////////////////////////
