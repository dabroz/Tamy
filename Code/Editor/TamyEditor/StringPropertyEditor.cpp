#include "StringPropertyEditor.h"
#include <QtWidgets\QLineEdit>


///////////////////////////////////////////////////////////////////////////////

StringPropertyEditor::StringPropertyEditor( TEditableReflectionProperty< std::string >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

StringPropertyEditor::~StringPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StringPropertyEditor::setupUi()
{
   m_edit = new QLineEdit(this);
   addWidget(m_edit);
   
   m_edit->setText(m_property->get().c_str());

   connect(m_edit, SIGNAL( editingFinished() ), this, SLOT( valChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////

void StringPropertyEditor::valChanged()
{
   std::string newVal = m_edit->text().toStdString();
   const std::string& oldVal = m_property->get();
   if ( newVal != oldVal )
   {
      m_property->set( newVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StringPropertyEditor::onPropertyChanged()
{
   m_edit->setText(m_property->get().c_str());
}

///////////////////////////////////////////////////////////////////////////////
