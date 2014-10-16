/// @file   TamyEditor/StoryBrowserTreeWidget.h
/// @brief  a tree widget dedicated to story trees
#pragma once

#include "TreeWidget.h"
#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\StoryListener.h"
#include "core\GenericFactory.h"
#include "StoryBrowserTreeItem.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class Story;
class Model;
class TreeWidgetDescFactory;
class QLineEdit;
class SelectionManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree widget dedicated to story trees.
 */
class StoryBrowserTreeWidget : public TreeWidget, public StoryListener
{
   Q_OBJECT

private:
   Story&                                                   m_story;
   Model&                                                   m_snapshotScene;
   GenericFactory< StoryNode, StoryBrowserTreeItem >*       m_itemsFactory;
   TreeWidgetDescFactory*                                   m_itemDescriptionsFactory;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param story
    * @param snapshotScene
    */
   StoryBrowserTreeWidget( QWidget* parentWidget, Story& story, Model& snapshotScene );
   ~StoryBrowserTreeWidget();

   /**
    * Looks for an item that corresponds to the specified story tree node.
    *
    * @param node
    */
   StoryBrowserTreeItem* findItem( StoryNode* node );

   /**
    * Deletes the story snapshot.
    */
   void clearSnapshot();

   // -------------------------------------------------------------------------
   // StoryListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( StoryNode* parentNode, int insertionIdx, StoryNode* node );
   void onNodeRemoved( StoryNode* parentNode, StoryNode* node );
   void onNodeChanged( StoryNode* node );

   // -------------------------------------------------------------------------
   // QAbstractItemView implementation
   // -------------------------------------------------------------------------
   QMimeData* mimeData( const QList< QTreeWidgetItem* > items ) const;
   void dragMoveEvent( QDragMoveEvent* event );
   void dropEvent( QDropEvent* event );

public slots:
   void renameItem( QTreeWidgetItem* item, int column );
   void nodeRenamed( QTreeWidgetItem* node, const QString& newName );
   void onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem );
   void onClearNode( QTreeWidgetItem* cleanedNode );
   void onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu );
};

///////////////////////////////////////////////////////////////////////////////

class AddStoryNodeAction : public QAction
{
   Q_OBJECT

private:
   StoryBrowserTreeWidget*             m_parentTree;
   StoryBrowserTreeItem*               m_parentItem;
   TreeWidgetDescFactory*              m_itemsFactory;
   int                                 m_typeIdx;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param desc
    * @param typeIdx
    * @param parentTree
    * @param parentNode
    * @param itemsFactory
    */
   AddStoryNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, StoryBrowserTreeWidget* parentTree, StoryBrowserTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
