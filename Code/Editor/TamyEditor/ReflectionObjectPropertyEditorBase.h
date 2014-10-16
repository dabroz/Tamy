/// @file   TamyEditor/ReflectionObjectPropertyEditorBase.h
/// @brief  a base for the ReflectionObject editors
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class ReflectionObjectPropertyEditorBase : public QFrame
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param widget
    */
   ReflectionObjectPropertyEditorBase( QWidget* widget );
   virtual ~ReflectionObjectPropertyEditorBase();

   /**
    * Called by the ObjectPropertyEditor when the property it edits changes outside the property editor.
    */
   virtual void onPropertyChanged() = 0;
};

///////////////////////////////////////////////////////////////////////////////
