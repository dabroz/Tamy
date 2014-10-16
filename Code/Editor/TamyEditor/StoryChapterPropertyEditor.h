/// @file   TamyEditor/StoryChapterPropertyEditor.h
/// @brief  a property editor that will allow to drag&drop story chapters onto it
#pragma once

#include "ext-StoryTeller\StoryChapter.h"
#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class StoryBrowserTreeItem;
class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * A property editor that will allow to drag&drop story chapters onto it.
 */
class StoryChapterPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< StoryChapter* >*   m_property;

   DropArea*                                       m_chapterName;
   StoryBrowserTreeItem*                           m_droppedTreeItem;

public:
   /**
    * Constructor.
    *
    * @param property         property this editor edits
    */
   StoryChapterPropertyEditor( TEditableReflectionProperty< StoryChapter* >* property );
   ~StoryChapterPropertyEditor();

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
