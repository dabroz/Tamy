#include "SceneTreeWidget.h"
#include "core-MVC.h"
#include "core-Renderer\Camera.h"
#include "core\Dequeue.h"
#include <QtGui\QDropEvent>
#include <QtGui\QDragMoveEvent>
#include <QtWidgets\QLineEdit>
#include "SceneItemMimeData.h"
#include "SelectionManager.h"

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"

// node representations
#include "SCTIRoot.h"
#include "SCTIEntity.h"
#include "SCTIComponent.h"


///////////////////////////////////////////////////////////////////////////////

SceneTreeWidget::SceneTreeWidget( QWidget* parentWidget, Model& scene, Camera* camera, SceneNode* root )
   : TreeWidget( parentWidget, parentWidget->objectName() + "/SceneTreeWidget" )
   , m_scene( scene )
   , m_camera( camera )
   , m_prefabbingEnabled( true )
{
   m_scene.addReference();

   setDefaultDropAction( Qt::IgnoreAction );
   setSelectionMode( QAbstractItemView::SingleSelection );
   setDragDropMode( QAbstractItemView::DragDrop );
   setDropIndicatorShown( true );

   // define a directory where prefabs will be deployed
   {
      FilePath sceneDir;
      m_scene.getFilePath().extractDir( sceneDir );
      FilePath parentDir;
      sceneDir.leaveDir( 1, parentDir );
      m_prefabsDir = FilePath( parentDir.getRelativePath() + "/Prefabs/" );
   }

   QTreeWidgetItem* header = new QTreeWidgetItem();
   header->setText( 0, "Node" );
   setHeaderItem( header );


   // create factory associations
   m_itemsFactory = new GenericFactory< SceneNode, SceneTreeItem >();
   m_itemsFactory->associateAbstract< Entity, SCTIEntity >();
   m_itemsFactory->associateAbstract< Component, SCTIComponent >();

   // initialize items descriptors factory
   m_componentsDescriptionsFactory = new TypeDescFactory< Component >( "unknownTypeIcon.png" );
   m_entitiesDescriptionsFactory = new TypeDescFactory< Entity >( "unknownTypeIcon.png" );

   // setup common actions
   connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( focusOnItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( renameItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( renameNode( QTreeWidgetItem*, const QString& ) ), this, SLOT( nodeRenamed( QTreeWidgetItem*, const QString& ) ) );
   connect( this, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );

   // create the root node
   if ( root == NULL )
   {
      SCTIRoot* rootNode = new SCTIRoot( m_scene );
      addTopLevelItem( rootNode );
   }
   else
   {
      SceneTreeItem* rootNode = m_itemsFactory->create( *root );

      QString desc, group;
      QIcon icon;
      m_entitiesDescriptionsFactory->getDesc( root->getVirtualRTTI(), desc, group, icon );
      rootNode->setIcon( 0, icon );

      addTopLevelItem( rootNode );
   }

   // attach self as a listener and pull initial tree structure
   m_scene.attachListener( this );
   m_scene.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

SceneTreeWidget::~SceneTreeWidget()
{
   // stop listening to the tree
   m_scene.detachListener( this );
   m_scene.removeReference();

   delete m_itemsFactory;
   m_itemsFactory = NULL;

   delete m_componentsDescriptionsFactory;
   m_componentsDescriptionsFactory = NULL;

   delete m_entitiesDescriptionsFactory;
   m_entitiesDescriptionsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::enablePrefabbing( bool flag )
{
   m_prefabbingEnabled = flag;
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onNodeAdded( SceneNode* node )
{
   // don't show nodes spawned from prefabs
   if ( node->wasInstantiatedFromPrefab() )
   {
      return;
   }

   SceneTreeItem* parentItem = findItem( node->getParent() );
   if ( parentItem )
   {
      SceneTreeItem* newItem = m_itemsFactory->create( *node );

      QString desc, group;
      QIcon icon;
      m_entitiesDescriptionsFactory->getDesc( node->getVirtualRTTI(), desc, group, icon );
      newItem->setIcon( 0, icon );

      parentItem->addChild( newItem );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onNodeRemoved( SceneNode* node )
{
   SceneTreeItem* parentItem = findItem( node->getParent() );
   if ( parentItem )
   {
      // find the removed child item
      int childrenCount = parentItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         SceneTreeItem* childItem = static_cast< SceneTreeItem* >( parentItem->child( i ) );
         if ( childItem->getNode() == node )
         {
            parentItem->removeChild( childItem );

            delete childItem;

            // found it
            return;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onNodeChanged( SceneNode* node )
{
   SceneTreeItem* item = findItem( node );
   if ( item )
   {
      item->updateDescription();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::resetContents( Model& model )
{
   clear();
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* SceneTreeWidget::mimeData( const QList< QTreeWidgetItem* > items ) const
{
   QMimeData* data = __super::mimeData( items );
   if ( items.empty() )
   {
      return data;
   }

   SceneTreeItem* treeItem = static_cast< SceneTreeItem* >( items.first() );
   SceneItemMimeDataUtil::encode( treeItem, data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* mimeData = event->mimeData();

   bool containsRelevantData = SceneItemMimeDataUtil::checkDataPresence( mimeData );
   if ( containsRelevantData )
   {
      __super::dragMoveEvent( event );
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::dropEvent( QDropEvent* event )
{
   // identify the drop point ( category )
   QPoint pos = event->pos();
   SceneTreeItem* dropPlaceItem = static_cast< SceneTreeItem* >( itemAt( pos.rx(), pos.ry() ) );

   if ( !dropPlaceItem )
   {
      dropPlaceItem = static_cast< SceneTreeItem* >( topLevelItem(0) );
   }

   // move selected items to the target category
   SceneNode* nodeToAdd = NULL;
   {
      // decode the stream, removing nodes from their previous location and batching them for addition
      const QMimeData* mimeData = event->mimeData();
      SceneTreeItem* movedItem = SceneItemMimeDataUtil::decode( mimeData );

      if ( movedItem != dropPlaceItem )
      {
         // we don't allow dropping self onto oneself
         SceneTreeItem* sourceContainerItem = ( SceneTreeItem* )movedItem->parent();

         // add a reference so that the item doesn't get deleted
         SceneNode* movedNode = movedItem->getNode();
         movedNode->addReference();

         // put them up for further addition
         nodeToAdd = movedNode;

         // remove this node from its previous parent
         sourceContainerItem->removeNode( movedItem );
      }
   }

   if( !nodeToAdd )
   {
      event->ignore();
      return;
   }

   // find the target place and the insertion index
   SceneTreeItem* targetItem = NULL;
   {
      QAbstractItemView::DropIndicatorPosition dropPosition = dropIndicatorPosition();
      switch( dropPosition )
      {
      case QAbstractItemView::OnItem:
         {
            if ( !dropPlaceItem->isContainerNode() )
            {
               // we're dropping it onto a leaf node - so acquire its parent
               targetItem = static_cast< SceneTreeItem* >( dropPlaceItem->parent() );
            }
            else
            {
               targetItem = dropPlaceItem;
            }

            break;
         }

      case QAbstractItemView::BelowItem:
         {
            targetItem = static_cast< SceneTreeItem* >( dropPlaceItem->parent() );
            if ( !targetItem )
            {
               targetItem = dropPlaceItem;
            }

            break;
         }

      case QAbstractItemView::AboveItem:
         {
            targetItem = static_cast< SceneTreeItem* >( dropPlaceItem->parent() );
            if ( !targetItem )
            {
               targetItem = dropPlaceItem;
            }

            break;
         }

      case QAbstractItemView::OnViewport:
         {
            targetItem = static_cast< SceneTreeItem* >( topLevelItem(0) );
            break;
         }
      }
   }

   // verify that the target node can accommodate that particular type of node
   if ( targetItem->canAccommodate( nodeToAdd ) )
   {
      // insert the nodes into the specified node
      targetItem->addNode( nodeToAdd );

      event->accept();
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::renameItem( QTreeWidgetItem* item, int column )
{
   SceneTreeItem* clickedItem = static_cast< SceneTreeItem* >( item );
   renameNode( clickedItem );
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::nodeRenamed( QTreeWidgetItem* item, const QString& newName )
{
   SceneTreeItem* renamedItem = static_cast< SceneTreeItem* >( item );

   SceneNode* renamedNode = renamedItem->getNode();
   renamedNode->setSceneNodeName( newName.toStdString().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem )
{
   SceneTreeItem* sceneNodeParent = static_cast< SceneTreeItem* >( parentItem );
   SceneTreeItem* sceneNodeRemoved = static_cast< SceneTreeItem* >( removedItem );
   sceneNodeParent->removeNode( sceneNodeRemoved );
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onClearNode( QTreeWidgetItem* cleanedNode )
{
   SceneTreeItem* sceneNodeCleaned = static_cast< SceneTreeItem* >( cleanedNode );
   sceneNodeCleaned->clear();
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu )
{
   class AddResourceActionFactory : public MenuActionsFactory
   {
   private:
      SceneTreeWidget*                    m_parentTree;
      SceneTreeItem*                      m_parentItem;
      TreeWidgetDescFactory*              m_itemsFactory;

   public:
      AddResourceActionFactory( SceneTreeWidget* parentTree, SceneTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory )
         : m_parentTree( parentTree )
         , m_parentItem( parentItem )
         , m_itemsFactory( itemsFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddSceneNodeAction( icon, desc, itemTypeIdx, m_parentTree, m_parentItem, m_itemsFactory );
         return action;
      }
   };


   // "Add" submenu
   SceneTreeItem* menuNode = static_cast< SceneTreeItem* >( clickedItem );
   if ( menuNode->canStoreEntities() )
   {
      QMenu* addEntityMenu = menu.addMenu( "Add Entity" );
      AddResourceActionFactory actionsFactory( this, menuNode, m_entitiesDescriptionsFactory );
      MenuUtils::itemsListMenu( m_entitiesDescriptionsFactory, actionsFactory, addEntityMenu );
   }

   if ( menuNode->canStoreComponents() )
   {
      QMenu* addComponentMenu = menu.addMenu( "Add Component" );
      AddResourceActionFactory actionsFactory( this, menuNode, m_componentsDescriptionsFactory );
      MenuUtils::itemsListMenu( m_componentsDescriptionsFactory, actionsFactory, addComponentMenu );
   }

   if ( m_prefabbingEnabled && menuNode->canBeTurnedIntoPrefab() )
   {
      menu.addSeparator();

      Entity* entity = static_cast< Entity* >( menuNode->getNode() );
      TurnIntoPrefabAction* action = new TurnIntoPrefabAction( this, QIcon( ":/TamyEditor/Resources/Icons/Engine/PrefabIcon.png" ), entity, m_prefabsDir );
      menu.addAction( action );
   }
}

///////////////////////////////////////////////////////////////////////////////

SceneTreeItem* SceneTreeWidget::findItem( SceneNode* node )
{
   SceneTreeItem* rootItem = static_cast< SceneTreeItem* >( topLevelItem( 0 ) );

   Dequeue< SceneTreeItem* > itemsQueue;
   itemsQueue.push( rootItem );
   while( !itemsQueue.empty() )
   {
      SceneTreeItem* checkedItem = itemsQueue.pop();

      if ( node == checkedItem->getNode() )
      {
         // found it
         return checkedItem;
      }

      // collect children
      int childrenCount = checkedItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         SceneTreeItem* childItem = static_cast< SceneTreeItem* >( checkedItem->child( i ) );
         itemsQueue.push( childItem );
      }
   }

   // unfortunately we didn't find a match
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::selectItem( QTreeWidgetItem* item, int column )
{
   SceneTreeItem* sceneItem = static_cast< SceneTreeItem* >( item );
   if ( !sceneItem )
   {
      return;
   }

   SceneNode* selectedNode = sceneItem->getNode();
   if ( selectedNode )
   {
      emit onSceneTreeObjectSelected( selectedNode );
   }
   else
   {
      emit onSceneTreeSelectionCleaned();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::focusOnItem( QTreeWidgetItem* item, int column )
{
   selectItem( item, column );

   if ( !m_camera )
   {
      return;
   }

   SceneTreeItem* sceneItem = static_cast< SceneTreeItem* >( item );
   if ( !sceneItem )
   {
      return;
   }

   SceneNode* node = sceneItem->getNode();
   if ( node->isA< Entity >() )
   {
      Entity* spatial = static_cast< Entity* >( node );

      const AxisAlignedBox& bb = spatial->getBoundingVolume();

      Vector extents;
      bb.getExtents( extents );

      FastFloat radius;
      radius.setMul( extents.getMax<3>(), FastFloat::fromFloat( 1.5f ) );
      radius.setMax( radius, Float_5 );

      m_camera->lookAt( *spatial, radius );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::onSelectionChanged( SelectionManager* selectionMgr )
{
   SceneNode* node = selectionMgr->getLastPickedNode();
   if ( node )
   {
      SceneTreeItem* sceneItem = findItem( node );
      if ( sceneItem )
      {
         setItemSelected( sceneItem, true );
         setCurrentItem( sceneItem );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeWidget::addToRoot( const QPoint& pos )
{
   QMenu* popupMenu = new QMenu( this );

   onPopupMenuShown( topLevelItem(0), *popupMenu );

   // display the menu
   popupMenu->popup( pos );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddSceneNodeAction::AddSceneNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, SceneTreeWidget* parentTree, SceneTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory )
   : QAction( icon, desc, parentTree )
   , m_parentTree( parentTree )
   , m_typeIdx( typeIdx )
   , m_parentItem( parentItem )
   , m_itemsFactory( itemsFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddSceneNodeAction::onTriggered()
{
   const SerializableReflectionType* nodeType = m_itemsFactory->getClass( m_typeIdx );
   ASSERT_MSG( nodeType, "Requested a node of an undefined type" );

   SceneNode* newNode = nodeType->instantiate< SceneNode >();
   m_parentItem->addNode( newNode );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TurnIntoPrefabAction::TurnIntoPrefabAction( SceneTreeWidget* parentTree, const QIcon& icon, Entity* entity, const FilePath& prefabsDir )
   : QAction( icon, "Turn into prefab", parentTree )
   , m_entity( entity )
   , m_prefabsDir( prefabsDir )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void TurnIntoPrefabAction::onTriggered()
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   
   Filesystem& filesystem = TSingleton< Filesystem >::getInstance();
   filesystem.mkdir( m_prefabsDir );

   FilePath prefabPath( m_prefabsDir.getRelativePath() + m_entity->getSceneNodeName() + "." + Prefab::getExtension() );
   Prefab* prefab = resMgr.create< Prefab >( prefabPath );

   prefab->setEntity( m_entity );
   prefab->saveResource();

   // set the prefab on the entity
   m_entity->clear();
   m_entity->setPrefab( prefab );
}

///////////////////////////////////////////////////////////////////////////////
