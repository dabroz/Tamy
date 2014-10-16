/// @file   TamyEditor\IntPropertyEditor.h
/// @brief  integer value property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class QSpinBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit integer value properties.
 */
class IntPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< int >*    m_property;

   QSpinBox*                              m_val;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   IntPropertyEditor( TEditableReflectionProperty< int >* property );
   ~IntPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged(int val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit unsigned integer value properties.
 */
class UIntPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< unsigned int >*    m_property;

   QSpinBox*                                       m_val;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   UIntPropertyEditor( TEditableReflectionProperty< unsigned int >* property );
   ~UIntPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged(int val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit unsigned integer value properties.
 */
class LongPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< long >*   m_property;

   QSpinBox*                              m_val;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   LongPropertyEditor( TEditableReflectionProperty< long >* property );
   ~LongPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged(int val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor widget that will edit unsigned long value properties.
 */
class ULongPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< unsigned long >*   m_property;

   QSpinBox*                                       m_val;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   ULongPropertyEditor( TEditableReflectionProperty< unsigned long >* property );
   ~ULongPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged(int val);

private:
   void setupUi();
};

///////////////////////////////////////////////////////////////////////////////
