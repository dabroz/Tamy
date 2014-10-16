/// @file   TamyEditor\BoolPropertyEditor.h
/// @brief  bool property editor
#pragma once


#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class QCheckBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit boolean properties.
 */
class BoolPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< bool >*    m_val;

   QCheckBox*                              m_checkBox;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   BoolPropertyEditor( TEditableReflectionProperty< bool >* property );
   ~BoolPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged(int val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
