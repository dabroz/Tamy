/// @file   TamyEditor\ObjectPropertyEditor.h
/// @brief  object property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include "core\ReflectionObject.h"
#include <vector>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class QComboBox;
class QHBoxLayout;
class QPropertiesView;
class ReflectionObjectPropertyEditorBase;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit object properties ( except resources ) - will allow to instantiate
 * them in place etc.
 */
class ObjectPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< ReflectionObject* >*        m_property;
   ReflectionObjectPropertyEditorBase*                      m_editor;

public:
   /**
    * Constructor.
    *
    * @param property         property this editor edits
    */
   ObjectPropertyEditor( TEditableReflectionProperty< ReflectionObject* >* property );
   ~ObjectPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

protected:
   void onInitialize();

};

///////////////////////////////////////////////////////////////////////////////
