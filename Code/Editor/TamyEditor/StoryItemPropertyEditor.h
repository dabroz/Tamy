/// @file   TamyEditor/StoryItemPropertyEditor.h
/// @brief  a property editor that will allow to drag&drop story items onto it
#pragma once

#include "ext-StoryTeller\StoryItem.h"
#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class StoryBrowserTreeItem;
class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * A property editor that will allow to drag&drop story items onto it.
 */
class StoryItemPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< StoryItem* >*      m_property;

   DropArea*                                       m_itemName;
   StoryBrowserTreeItem*                           m_droppedTreeItem;

public:
   /**
    * Constructor.
    *
    * @param property         property this editor edits
    */
   StoryItemPropertyEditor( TEditableReflectionProperty< StoryItem* >* property );
   ~StoryItemPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged();
   void valErased( bool );

protected:
   void onInitialize();

private:
   void refreshPropertyName();
};

///////////////////////////////////////////////////////////////////////////////
