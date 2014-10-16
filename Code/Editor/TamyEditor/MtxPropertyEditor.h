/// @file   TamyEditor\MtxPropertyEditor.h
/// @brief  matrix property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class QDoubleSpinBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit matrix properties.
 */
class MtxPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< Matrix >*    m_property;

   QDoubleSpinBox*                           m_yawVal;
   QDoubleSpinBox*                           m_pitchVal;
   QDoubleSpinBox*                           m_rollVal;
   QDoubleSpinBox*                           m_xVal;
   QDoubleSpinBox*                           m_yVal;
   QDoubleSpinBox*                           m_zVal;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   MtxPropertyEditor( TEditableReflectionProperty< Matrix >* property );
   ~MtxPropertyEditor();

protected:
   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void yawValChanged(double val);
   void pitchValChanged(double val);
   void rollValChanged(double val);

   void xValChanged(double val);
   void yValChanged(double val);
   void zValChanged(double val);

private:
   void setupUi();
   void initializeValues();

};

///////////////////////////////////////////////////////////////////////////////
