#pragma once

/// @file   TamyEditor\StringPropertyEditor.h
/// @brief  string property editor

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class QLineEdit;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit string properties.
 */
class StringPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< std::string >*  m_property;

   QLineEdit*                                   m_edit;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   StringPropertyEditor( TEditableReflectionProperty< std::string >* property );
   ~StringPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged();

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
