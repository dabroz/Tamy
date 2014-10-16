/// @file   TamyEditor\EnumPropertyEditor.h
/// @brief  enum property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "core\ReflectionEnum.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class Enum;
class QComboBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit enum properties.
 */
class EnumPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< ReflectionEnum >*    m_property;

   QComboBox*                                        m_editCombo;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   EnumPropertyEditor( TEditableReflectionProperty< ReflectionEnum >* property );
   ~EnumPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged( const QString& val );

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
