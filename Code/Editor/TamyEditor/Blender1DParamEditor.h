/// @file   TamyEditor/Blender1DParamEditor.h
/// @brief  a BlendTreeBlender1D parameter editor widget
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class QDoubleSpinBox;

///////////////////////////////////////////////////////////////////////////////

class Blender1DParamEditor : public QFrame
{
   Q_OBJECT

private:
   QDoubleSpinBox*         m_val;

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   Blender1DParamEditor( QWidget* parent );
   ~Blender1DParamEditor();

   /**
    * Initializes the edited value.
    *
    * @param parameterValue
    */
   void initialize( float parameterValue );

   /**
    * Returns the edited value.
    */
   float getValue() const;
};

///////////////////////////////////////////////////////////////////////////////
