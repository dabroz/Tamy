#include "BoolPropertyEditor.h"
#include <QtWidgets\QCheckBox>


///////////////////////////////////////////////////////////////////////////////

BoolPropertyEditor::BoolPropertyEditor( TEditableReflectionProperty< bool >* val )
: QPropertyEditor( val->getLabel().c_str())
, m_val( val )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

BoolPropertyEditor::~BoolPropertyEditor()
{
   delete m_val;
   m_val = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BoolPropertyEditor::setupUi()
{
   m_checkBox = new QCheckBox(this); addWidget(m_checkBox);
   m_checkBox->setChecked( m_val->get() );

   connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(valChanged(int)));
}

///////////////////////////////////////////////////////////////////////////////

void BoolPropertyEditor::valChanged(int val)
{
   m_val->set( val != 0 );
}

///////////////////////////////////////////////////////////////////////////////

void BoolPropertyEditor::onPropertyChanged()
{
   m_checkBox->setChecked( m_val->get() );
}

///////////////////////////////////////////////////////////////////////////////
