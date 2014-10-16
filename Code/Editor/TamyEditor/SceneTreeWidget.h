/// @file   TamyEditor/SceneTreeWidget.h
/// @brief  A tree that shows the structure of a scene
#pragma once

#include "TreeWidget.h"
#include "core-MVC\ModelView.h"
#include "core-MVC\SceneNode.h"
#include "core\GenericFactory.h"
#include "core\FilePath.h"
#include "SceneTreeItem.h"
#include "SelectionManagerListener.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class Camera;
class Model;
class TreeWidgetDescFactory;
class QLineEdit;
class SelectionManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree widget dedicated to story trees.
 */
class SceneTreeWidget : public TreeWidget, public ModelView, public SelectionManagerListener
{
   Q_OBJECT

private:
   Model&                                                   m_scene;
   Camera*                                                  m_camera;

   FilePath                                                 m_prefabsDir;
   bool                                                     m_prefabbingEnabled;

   GenericFactory< SceneNode, SceneTreeItem >*              m_itemsFactory;
   TreeWidgetDescFactory*                                   m_componentsDescriptionsFactory;
   TreeWidgetDescFactory*                                   m_entitiesDescriptionsFactory;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param story
    * @param camera
    * @param root             a different node that will be considered the hierarchy root
    */
   SceneTreeWidget( QWidget* parentWidget, Model& scene, Camera* camera, SceneNode* root = NULL );
   ~SceneTreeWidget();

   /**
    * Looks for an item that corresponds to the specified story tree node.
    *
    * @param node
    */
   SceneTreeItem* findItem( SceneNode* node );

   /**
    * Shows a pop-up menu that allows to select and add a node to the root of the scene.
    *
    * @param pos     where the menu should be displayed ( in global screen coordinates )
    */
   void addToRoot( const QPoint& pos );

   /**
    * Enables/disables the ability to turn entities into Prefabs.
    *
    * @param flag
    */
   void enablePrefabbing( bool flag );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& model );

   // -------------------------------------------------------------------------
   // SelectionManagerListener implementation
   // -------------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );

   // -------------------------------------------------------------------------
   // QAbstractItemView implementation
   // -------------------------------------------------------------------------
   QMimeData* mimeData( const QList< QTreeWidgetItem* > items ) const;
   void dragMoveEvent( QDragMoveEvent* event );
   void dropEvent( QDropEvent* event );

signals:
   /**
    * Emitted when a node in the tree is selected.
    *
    * @param node
    */
   void onSceneTreeObjectSelected( SceneNode* node );

   /**
    * Emitted when the selection changes so that no node is selected any longer.
    */
   void onSceneTreeSelectionCleaned();

public slots:
   void selectItem( QTreeWidgetItem* item, int column );
   void focusOnItem( QTreeWidgetItem* item, int column );
   void renameItem( QTreeWidgetItem* item, int column );
   void nodeRenamed( QTreeWidgetItem* node, const QString& newName );
   void onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem );
   void onClearNode( QTreeWidgetItem* cleanedNode );
   void onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu );
};

///////////////////////////////////////////////////////////////////////////////

class AddSceneNodeAction : public QAction
{
   Q_OBJECT

private:
   SceneTreeWidget*                    m_parentTree;
   SceneTreeItem*                      m_parentItem;
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
   AddSceneNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, SceneTreeWidget* parentTree, SceneTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class TurnIntoPrefabAction : public QAction
{
   Q_OBJECT

private:
   Entity*                                m_entity;
   FilePath                               m_prefabsDir;

public:
   /**
    * Constructor.
    *
    * @param parentTree
    * @param icon
    * @param entity
    * @param prefabsDir
    */
   TurnIntoPrefabAction( SceneTreeWidget* parentTree, const QIcon& icon, Entity* entity, const FilePath& prefabsDir );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
