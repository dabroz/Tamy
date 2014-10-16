/// @file   TamyEditor/StoryEditor.h
/// @brief  a story resource editor
#pragma once

#include "ResourceEditor.h"
#include "SelectionManagerListener.h"
#include "core\TimeDependent.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class Story;
class StoryPlayer;
class StoryChapter;
class Model;
class StoryBrowserTreeWidget;
class StoryBrowserTreeItem;
class QPropertiesView;
class QTreeWidgetItem;
class QVBoxLayout;
class TamySceneWidget;
class QMenu;
class QAction;
class QSplitter;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////

class StoryEditor : public ResourceEditor, public SelectionManagerListener, public TimeDependent
{
   Q_OBJECT

private:
   enum TabIds
   {
      StoryEditorTab_ChapterGraph,
      StoryEditorTab_Scene,
   };

private:
   Story&                        m_story;

   StoryPlayer*                  m_storyPlayer;
   bool                          m_playing;

   QSplitter*                    m_mainSplitter;
   StoryBrowserTreeWidget*       m_treeWidget;
   QPropertiesView*              m_nodeProperties;
   QVBoxLayout*                  m_chapterGraphEditorLayout;
   QTabWidget*                   m_workAreaTabsManager;
   TamySceneWidget*              m_sceneWidget;

   StoryBrowserTreeItem*         m_selectedItem;
   StoryChapter*                 m_selectedChapter;

public:
   /**
    * Constructor.
    *
    * @param behTree
    */
   StoryEditor( Story& behTree );
   ~StoryEditor();

   // -------------------------------------------------------------------------
   // SelectionManagerListener implementation
   // -------------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

public slots:
   void onSave();
   void onPlay( bool scenePlayed );
   void onItemClicked( QTreeWidgetItem* item, int column );
   void onStoryTreePopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void editItem( QTreeWidgetItem* item );

   void updatePreviewItemSelection( QTreeWidgetItem* item );

   void updateStoryPlayerStatus();

   void freezePreview( bool freeze );
};

///////////////////////////////////////////////////////////////////////////////

class GenerateSceneSnapshotAction : public QAction
{
   Q_OBJECT

private:
   Model*                              m_scene;
   StoryBrowserTreeItem*               m_item;

public:
   /**
    * Constructor.
    *
    * @parent
    * @param scene
    * @param item
    */
   GenerateSceneSnapshotAction( QObject* parent, Model* scene, StoryBrowserTreeItem* item );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
