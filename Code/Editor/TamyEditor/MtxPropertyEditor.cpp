#include "MtxPropertyEditor.h"
#include <QtWidgets\QGroupBox>
#include <QtWidgets\QBoxLayout>
#include <QtWidgets\QLabel>
#include <QtWidgets\QSpinBox>
#include "core\Vector.h"
#include "core\EulerAngles.h"


///////////////////////////////////////////////////////////////////////////////

MtxPropertyEditor::MtxPropertyEditor( TEditableReflectionProperty< Matrix >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

MtxPropertyEditor::~MtxPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::setupUi()
{
   QGroupBox* rotRow = new QGroupBox( "orientation", this ); addWidget(rotRow);
   setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
   setMinimumSize( 100, 50 );
   QHBoxLayout* rotRowLayout = new QHBoxLayout(rotRow);
   rotRowLayout->setSpacing( 0 );
   rotRowLayout->setMargin( 0 );

   m_yawVal = new QDoubleSpinBox(rotRow); 
   m_yawVal->setRange( -FLT_MAX, FLT_MAX );
   rotRowLayout->addWidget(new QLabel("yaw:", rotRow)); 
   rotRowLayout->addWidget(m_yawVal);

   m_pitchVal = new QDoubleSpinBox(rotRow); 
   m_pitchVal->setRange( -FLT_MAX, FLT_MAX );
   rotRowLayout->addWidget(new QLabel("pitch:", rotRow));
   rotRowLayout->addWidget(m_pitchVal);

   m_rollVal = new QDoubleSpinBox(rotRow); 
   m_rollVal->setRange( -FLT_MAX, FLT_MAX );
   rotRowLayout->addWidget(new QLabel("roll:", rotRow));
   rotRowLayout->addWidget(m_rollVal);

   QGroupBox* posRow = new QGroupBox( "translation", this ); addWidget(posRow);
   QHBoxLayout* posRowLayout = new QHBoxLayout(posRow);
   posRowLayout->setSpacing( 0 );
   posRowLayout->setMargin( 0 );
   m_xVal = new QDoubleSpinBox(posRow);
   m_xVal->setRange( -FLT_MAX, FLT_MAX );
   posRowLayout->addWidget(new QLabel("x:", posRow));
   posRowLayout->addWidget(m_xVal);
   m_yVal = new QDoubleSpinBox(posRow); 
   m_yVal->setRange( -FLT_MAX, FLT_MAX );
   posRowLayout->addWidget(new QLabel("y:", posRow));
   posRowLayout->addWidget(m_yVal);
   m_zVal = new QDoubleSpinBox(posRow); 
   m_zVal->setRange( -FLT_MAX, FLT_MAX );
   posRowLayout->addWidget(new QLabel("z:", posRow));
   posRowLayout->addWidget(m_zVal);

   // set the start value
   initializeValues();
   
   // connect the view to the model
   connect(m_yawVal, SIGNAL(valueChanged(double)), this, SLOT(yawValChanged(double)));
   connect(m_pitchVal, SIGNAL(valueChanged(double)), this, SLOT(pitchValChanged(double)));
   connect(m_rollVal, SIGNAL(valueChanged(double)), this, SLOT(rollValChanged(double)));

   connect(m_xVal, SIGNAL(valueChanged(double)), this, SLOT(xValChanged(double)));
   connect(m_yVal, SIGNAL(valueChanged(double)), this, SLOT(yValChanged(double)));
   connect(m_zVal, SIGNAL(valueChanged(double)), this, SLOT(zValChanged(double)));
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::initializeValues()
{
   // set the start value
   Matrix currVal = m_property->get();

   EulerAngles orientation;
   currVal.getRotation( orientation );
   m_yawVal->setValue( orientation.getYaw() );
   m_pitchVal->setValue( orientation.getPitch() );
   m_rollVal->setValue( orientation.getRoll() );

   Vector pos = currVal.position();
   m_xVal->setValue(pos[0]);
   m_yVal->setValue(pos[1]);
   m_zVal->setValue(pos[2]);
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::onPropertyChanged()
{
   initializeValues();
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::yawValChanged(double val)
{
   Matrix currVal = m_property->get();

   EulerAngles orientation;
   currVal.getRotation( orientation );
   orientation[EulerAngles::Yaw] = (float)val;
   
   Matrix rotMtx, transMtx, finalMtx;
   rotMtx.setRotation( orientation );
   transMtx.setTranslation( currVal.position() );
   finalMtx.setMul( rotMtx, transMtx );

   m_property->set( finalMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::pitchValChanged(double val)
{
   Matrix currVal = m_property->get();

   EulerAngles orientation;
   currVal.getRotation( orientation );
   orientation[EulerAngles::Pitch] = (float)val;

   Matrix rotMtx, transMtx, finalMtx;
   rotMtx.setRotation( orientation );
   transMtx.setTranslation( currVal.position() );
   finalMtx.setMul( rotMtx, transMtx );

   m_property->set( finalMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::rollValChanged(double val)
{
   Matrix currVal = m_property->get();

   EulerAngles orientation;
   currVal.getRotation( orientation );
   orientation[EulerAngles::Roll] = (float)val;

   Matrix rotMtx, transMtx, finalMtx;
   rotMtx.setRotation( orientation );
   transMtx.setTranslation( currVal.position() );
   finalMtx.setMul( rotMtx, transMtx );

   m_property->set( finalMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::xValChanged(double val)
{
   Matrix currVal = m_property->get();

   Vector pos = currVal.position();
   pos[0] = (float)val;

   currVal.setPosition<3>( pos );
   
   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::yValChanged(double val)
{
   Matrix currVal = m_property->get();

   Vector pos = currVal.position();
   pos[1] = (float)val;

   currVal.setPosition<3>( pos );

   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////

void MtxPropertyEditor::zValChanged(double val)
{
   Matrix currVal = m_property->get();

   Vector pos = currVal.position();
   pos[2] = (float)val;

   currVal.setPosition<3>( pos );

   m_property->set( currVal );
}

///////////////////////////////////////////////////////////////////////////////
