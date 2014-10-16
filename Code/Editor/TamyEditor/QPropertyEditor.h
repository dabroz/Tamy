/// @file   TamyEditor\QPropertyEditor.h
/// @brief  base class for all qt property editors
#pragma once

#include "core\ReflectionPropertyEditor.h"
#include <QtWidgets\QFrame>
#include <Qstring>


///////////////////////////////////////////////////////////////////////////////

class QPropertiesView;
class QVBoxLayout;
class TamyEditor;

///////////////////////////////////////////////////////////////////////////////

/**
 * This base class will provide all editors with a uniform look & feel.
 */
class QPropertyEditor : public QFrame, public ReflectionPropertyEditor
{
   Q_OBJECT

private:
   QVBoxLayout*      m_layout;

public:
   virtual ~QPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parent );
   void deinitialize( ReflectionObjectEditor* parent );

protected:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   QPropertyEditor( const QString& propertyLabel );

   /**
    * Called when the editor gets initialized.
    */
   virtual void onInitialize() {}

   /**
    * Adds a new widget.
    *
    * @param widget
    */
   void addWidget(QWidget* widget);

   /**
    * Adds an existing widget.
    *
    * @param widget
    */
   void removeWidget(QWidget& widget);
};

///////////////////////////////////////////////////////////////////////////////
