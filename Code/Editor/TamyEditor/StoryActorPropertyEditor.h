/// @file   TamyEditor/StoryActorPropertyEditor.h
/// @brief  a property editor that will allow to drag&drop story actor onto it
#pragma once

#include "ext-StoryTeller\StoryActor.h"
#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

class StoryBrowserTreeItem;
class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * A property editor that will allow to drag&drop story actor onto it.
 */
class StoryActorPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< StoryActor* >*     m_property;

   DropArea*                                       m_actorName;
   StoryBrowserTreeItem*                           m_droppedTreeItem;

public:
   /**
    * Constructor.
    *
    * @param property         property this editor edits
    */
   StoryActorPropertyEditor( TEditableReflectionProperty< StoryActor* >* property );
   ~StoryActorPropertyEditor();

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
