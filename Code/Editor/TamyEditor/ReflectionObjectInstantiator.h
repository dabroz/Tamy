/// @file   TamyEditor\ReflectionObjectInstantiator.h
/// @brief  a property editor capable of instantiating a ReflectionObject
#pragma once

#include "ReflectionObjectPropertyEditorBase.h"
#include "core\ReflectionProperty.h"
#include "core\ReflectionObject.h"
#include <vector>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class QComboBox;
class QVBoxLayout;
class QPropertiesView;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit object properties ( except resources ) - will allow to instantiate
 * them in place etc.
 */
class ReflectionObjectInstantiator : public ReflectionObjectPropertyEditorBase
{
   Q_OBJECT

private:
   TEditableReflectionProperty< ReflectionObject* >*     m_property;
   QComboBox*                                            m_typeSelectionBox;
   QVBoxLayout*                                          m_layout;
   QPropertiesView*                                      m_propertiesView;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param property         property this editor edits
    */
   ReflectionObjectInstantiator( QWidget* parentWidget, TEditableReflectionProperty< ReflectionObject* >* property );
   ~ReflectionObjectInstantiator();

   // -------------------------------------------------------------------------
   // ReflectionObjectPropertyEditorBase implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged( const QString& text );

private:
   void initialize();
   void refresh();
};

///////////////////////////////////////////////////////////////////////////////
