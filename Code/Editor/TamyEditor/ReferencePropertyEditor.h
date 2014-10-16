/// @file   TamyEditor\ReferencePropertyEditor.h
/// @brief  a property editor that edits references to ReflectionObjects
#pragma once

#include "ReflectionObjectPropertyEditorBase.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit reference properties.
 */
class ReferencePropertyEditor : public ReflectionObjectPropertyEditorBase
{
   Q_OBJECT

private:
   TEditableReflectionProperty< ReflectionObject* >*              m_property;

   ReflectionObject*                                              m_objPtr;
   DropArea*                                                      m_referenceDropArea;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param property         property this editor edits
    */
   ReferencePropertyEditor( QWidget* parentWidget, TEditableReflectionProperty< ReflectionObject* >* property );
   ~ReferencePropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionObjectPropertyEditorBase implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged();
   void valErased( bool );

private:
   void initialize();
   void refreshPropertyName();
};

///////////////////////////////////////////////////////////////////////////////
