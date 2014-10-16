#include "Vec3PropertyEditor.h"
#include <QtWidgets\QBoxLayout>
#include <QtWidgets\QLabel>
#include <QtWidgets\QSpinBox>


///////////////////////////////////////////////////////////////////////////////

Vec3PropertyEditor::Vec3PropertyEditor( TEditableReflectionProperty< Vector >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

Vec3PropertyEditor::~Vec3PropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Vec3PropertyEditor::setupUi()
{
   QFrame* frame = new QFrame(this); addWidget(frame);
   QHBoxLayout* layout = new QHBoxLayout(frame);
   layout->setSpacing( 0 );
   layout->setMargin( 0 );

   layout->addWidget(new QLabel("X:", this));
   m_xVal = new QDoubleSpinBox(this); layout->addWidget(m_xVal);
   m_xVal->setDecimals( 4 );
   m_xVal->setMaximumWidth( 100 );
   m_xVal->setRange( -FLT_MAX, FLT_MAX );

   layout->addWidget(new QLabel("Y:", this));
   m_yVal = new QDoubleSpinBox(this); layout->addWidget(m_yVal);
   m_yVal->setDecimals( 4 );
   m_yVal->setMaximumWidth( 100 );
   m_yVal->setRange( -FLT_MAX, FLT_MAX );

   layout->addWidget(new QLabel("Z:", this));
   m_zVal = new QDoubleSpinBox(this); layout->addWidget(m_zVal);
   m_zVal->setDecimals( 4 );
   m_zVal->setMaximumWidth( 100 );
   m_zVal->setRange( -FLT_MAX, FLT_MAX );

   const Vector& currVal = m_property->get();
   m_xVal->setValue(currVal[0]);
   m_yVal->setValue(currVal[1]);
   m_zVal->setValue(currVal[2]);

   connect(m_xVal, SIGNAL(valueChanged(double)), this, SLOT(xValChanged(double)));
   connect(m_yVal, SIGNAL(valueChanged(double)), this, SLOT(yValChanged(double)));
   connect(m_zVal, SIGNAL(valueChanged(double)), this, SLOT(zValChanged(double)));
}

///////////////////////////////////////////////////////////////////////////////

void Vec3PropertyEditor::xValChanged(double val)
{
   Vector currVal = m_property->get();
   currVal[0] = val;
   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////

void Vec3PropertyEditor::yValChanged(double val)
{
   Vector currVal = m_property->get();
   currVal[1] = val;
   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////

void Vec3PropertyEditor::zValChanged(double val)
{
   Vector currVal = m_property->get();
   currVal[2] = val;
   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////

void Vec3PropertyEditor::onPropertyChanged()
{
   const Vector& currVal = m_property->get();
   m_xVal->setValue(currVal[0]);
   m_yVal->setValue(currVal[1]);
   m_zVal->setValue(currVal[2]);
}

///////////////////////////////////////////////////////////////////////////////
