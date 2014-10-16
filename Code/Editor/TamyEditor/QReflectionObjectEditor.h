/// @file   TamyEditor\QReflectionObjectEditor.h
/// @brief  an editor of  object nodes in a property editors tree
#pragma once

#include "core\ReflectionPropertyEditor.h"
#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////

/**
 * An editor of object nodes in a property editors tree.
 */
class QReflectionObjectEditor : public QFrame, public ReflectionObjectEditor
{
   Q_OBJECT

private:
   QVBoxLayout*      m_layout;

public:
   /**
    * Constructor.
    *
    * @param editedObject     object this editor edits
    */
   QReflectionObjectEditor( ReflectionObject* editedObject );
   ~QReflectionObjectEditor();

   // -------------------------------------------------------------------------
   // ReflectionObjectEditor implementation
   // -------------------------------------------------------------------------
   virtual void initialize( ReflectionPropertiesView* parentView, ReflectionPropertyEditorComposite* parentNode );
   virtual void deinitialize( ReflectionPropertyEditorComposite* parentNode );

   // -------------------------------------------------------------------------
   // Editor widgets management
   // -------------------------------------------------------------------------
   /**
    * This method adds a new single-property editor widget to the properties
    * editor window.
    *
    * @param editorWidget  widget of the editor we want to add
    */
   void addPropertyEditor( QWidget* editorWidget );

   /**
    * This method removes a single-property editor widget from the properties
    * editor window.
    *
    * @param editorWidget  widget of the editor we want to remove
    */
   void removePropertyEditor( QWidget& editorWidget );

public slots:
};

///////////////////////////////////////////////////////////////////////////////
