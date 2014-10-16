/// @file   TamyEditor/HexEditor.h
/// @brief  hex value editor widget
#pragma once

#include <QtWidgets\QLineEdit>
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Hex value editor widget.
 */
class HexEditor : public QLineEdit
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param initialValue
    */
   HexEditor( QWidget* parentWidget, ulong initialValue = 0 );

   /**
    * Returns the edited value.
    */
   ulong getValue() const;

public slots:
   void valChanged();
};

///////////////////////////////////////////////////////////////////////////////
