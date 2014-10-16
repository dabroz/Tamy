/// @file   TamyEditor\Vec3PropertyEditor.h
/// @brief  3D vector property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class QDoubleSpinBox;
class QDoubleSpinBox;
class QDoubleSpinBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit 3D vector properties.
 */
class Vec3PropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< Vector >* m_property;

   QDoubleSpinBox*                        m_xVal;
   QDoubleSpinBox*                        m_yVal;
   QDoubleSpinBox*                        m_zVal;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   Vec3PropertyEditor( TEditableReflectionProperty< Vector >* property );
   ~Vec3PropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void xValChanged(double val);
   void yValChanged(double val);
   void zValChanged(double val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
