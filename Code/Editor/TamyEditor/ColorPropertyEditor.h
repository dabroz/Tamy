/// @file   TamyEditor\ColorPropertyEditor.h
/// @brief  color property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "core\Color.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class ColorFrame;
class QPushButton;
///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit color properties.
 */
class ColorPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< Color >*     m_val;

   ColorFrame*                               m_colorPicker;
   QPushButton*                              m_selectColorBtn;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   ColorPropertyEditor( TEditableReflectionProperty< Color >* property );
   ~ColorPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged( const QColor& val );
   void pickColor( bool );

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
