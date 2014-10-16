/// @file   TamyEditor\QReflectionPropertyEditorComposite.h
/// @brief  array container property editor ( featuring an std::vector as the array ;)
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class QToolBox;

///////////////////////////////////////////////////////////////////////////////

/**
 * Array container property editor ( featuring an std::vector as the array ;)
 */
class QReflectionPropertyEditorComposite : public QFrame, public ReflectionPropertyEditorComposite
{
   Q_OBJECT

private:
   QToolBox*      m_toolBox;

public:
   /**
    * Constructor.
    *
    * @param property   property this editor edits
    */
   QReflectionPropertyEditorComposite( ReflectionPropertyArray* property );
   ~QReflectionPropertyEditorComposite();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditorComposite implementation
   // -------------------------------------------------------------------------
   virtual void initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parentNode );
   virtual void deinitialize( ReflectionObjectEditor* parentNode );
   virtual void onPropertyChanged();

   // -------------------------------------------------------------------------
   // Editor widgets management
   // -------------------------------------------------------------------------
   /**
    * This method adds a new single-property editor widget to the properties
    * editor window.
    *
    * @param propertyName
    */
   void addPropertyEditor( QWidget* editorWidget );

   /**
    * This method removes a single-property editor widget from the properties
    * editor window.
    *
    * @param editorWidget  widget of the editor we want to remove
    */
   void removePropertyEditor( QWidget& editorWidget );

};

///////////////////////////////////////////////////////////////////////////////
