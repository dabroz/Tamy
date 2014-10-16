#include "StoryBrowserTreeWidget.h"
#include "ext-StoryTeller.h"
#include "core\Dequeue.h"
#include <QtGui\QDropEvent>
#include <QtGui\QDragMoveEvent>
#include <QtWidgets\QLineEdit>
#include "StoryTreeItemMimeData.h"

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"

// node representations
#include "STIRoot.h"
#include "STIChapter.h"
#include "STILeaf.h"


///////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeWidget::StoryBrowserTreeWidget( QWidget* parentWidget, Story& story, Model& snapshotScene )
   : TreeWidget( parentWidget, parentWidget->objectName() + "/Story" )
   , m_story( story )
   , m_snapshotScene( snapshotScene )
{
   m_snapshotScene.addReference();

   setDefaultDropAction( Qt::IgnoreAction );
   setSelectionMode( QAbstractItemView::SingleSelection );
   setDragDropMode( QAbstractItemView::DragDrop );
   setDropIndicatorShown( true );
   setColumnCount(2);

   QTreeWidgetItem* header = new QTreeWidgetItem();
   header->setText( 0, "Name" );
   header->setText( 1, "Preview" );
   setHeaderItem( header );

   // create the root node
   STIRoot* rootNode = new STIRoot( this, m_story );
   rootNode->initialize( snapshotScene, NULL );

   // create factory associations
   m_itemsFactory = new GenericFactory< StoryNode, StoryBrowserTreeItem >();
   m_itemsFactory->associate< StoryChapter, STIChapter >();
   m_itemsFactory->associateAbstract< StoryItem, STILeaf >();
   m_itemsFactory->associateAbstract< StoryActor, STILeaf >();

   // initialize items descriptors factory
   m_itemDescriptionsFactory = new TypeDescFactory< StoryNode >( "unknownStoryIcon.png" );


   // attach self as a listener and pull initial tree structure
   m_story.addReference();
   m_story.attachListener( this );
   m_story.pullStructure( this );

   // setup common actions
   connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( renameItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( renameNode( QTreeWidgetItem*, const QString& ) ), this, SLOT( nodeRenamed( QTreeWidgetItem*, const QString& ) ) );
   connect( this, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeWidget::~StoryBrowserTreeWidget()
{
   // stop listening to the tree
   m_story.detachListener( this );
   m_story.removeReference();

   delete m_itemsFactory;
   m_itemsFactory = NULL;

   delete m_itemDescriptionsFactory;
   m_itemDescriptionsFactory = NULL;

   // remove a reference to the snapshot scene
   m_snapshotScene.removeReference();
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::clearSnapshot()
{
   // deinitialize all items
   LocalStack< StoryBrowserTreeItem* > items;
   items.push( static_cast< StoryBrowserTreeItem* >( this->topLevelItem(0) ) );
   while( !items.empty() )
   {
      StoryBrowserTreeItem* item = items.pop();
      item->clearSnapshot();

      int count = item->childCount();
      for ( int i = 0; i < count; ++i )
      {
         StoryBrowserTreeItem* childItem = static_cast< StoryBrowserTreeItem* >( item->child(i) );
         items.push( childItem );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onNodeAdded( StoryNode* parentNode, int insertionIdx, StoryNode* node )
{
   StoryBrowserTreeItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      StoryBrowserTreeItem* newItem = m_itemsFactory->create( *node );
     
      QString desc, group;
      QIcon icon;
      m_itemDescriptionsFactory->getDesc( node->getVirtualRTTI(), desc, group, icon );
      newItem->setIcon( 0, icon );

      parentItem->insertChild( insertionIdx, newItem );

      // initialize new item
      newItem->initialize( m_snapshotScene, parentItem );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onNodeRemoved( StoryNode* parentNode, StoryNode* node )
{
   StoryBrowserTreeItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      // find the removed child item
      int childrenCount = parentItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         StoryBrowserTreeItem* childItem = static_cast< StoryBrowserTreeItem* >( parentItem->child( i ) );
         if ( childItem->getNode() == node )
         {
            parentItem->removeChild( childItem );

            childItem->clearSnapshot();
            delete childItem;

            // found it
            return;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onNodeChanged( StoryNode* node )
{
   StoryBrowserTreeItem* item = findItem(node );
   if ( item )
   {
      item->updateDescription();
   }
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* StoryBrowserTreeWidget::mimeData( const QList< QTreeWidgetItem* > items ) const
{
   QMimeData* data = __super::mimeData( items );
   if ( items.empty() )
   {
      return data;
   }

   StoryBrowserTreeItem* treeItem = static_cast< StoryBrowserTreeItem* >( items.first() );
   StoryTreeItemMimeDataUtil::encode( treeItem, data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* mimeData = event->mimeData();

   bool containsRelevantData = StoryTreeItemMimeDataUtil::checkDataPresence( mimeData );
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

void StoryBrowserTreeWidget::dropEvent( QDropEvent* event )
{
   // identify the drop point ( category )
   QPoint pos = event->pos();
   StoryBrowserTreeItem* dropPlaceItem = static_cast< StoryBrowserTreeItem* >( itemAt( pos.rx(), pos.ry() ) );

   if ( !dropPlaceItem )
   {
      dropPlaceItem = static_cast< StoryBrowserTreeItem* >( topLevelItem(0) );
   }

   // move selected items to the target category
   StoryNode* nodeToAdd = NULL;
   {
      // decode the stream, removing nodes from their previous location and batching them for addition
      const QMimeData* mimeData = event->mimeData();
      StoryBrowserTreeItem* movedItem = StoryTreeItemMimeDataUtil::decode( mimeData );

      if ( movedItem != dropPlaceItem )
      {
         // we don't allow dropping self onto oneself
         StoryBrowserTreeItem* sourceContainerItem = ( StoryBrowserTreeItem* )movedItem->parent();

         // add a reference so that the item doesn't get deleted
         StoryNode* movedNode = movedItem->getNode();
         movedNode->addReference();

         // put them up for further addition
         nodeToAdd = movedNode;

         // remove this node from its previous parent
         sourceContainerItem->removeNode( movedItem );
      }
   }

   if( nodeToAdd )
   {
      // find the target place and the insertion index
      StoryBrowserTreeItem* targetItem = NULL;
      int insertionIndex = 0;
      {
         QAbstractItemView::DropIndicatorPosition dropPosition = dropIndicatorPosition();
         switch( dropPosition )
         {
         case QAbstractItemView::OnItem:
            {
               if ( !dropPlaceItem->isContainerNode() )
               {
                  // we're dropping it onto a leaf node - so acquire its parent
                  targetItem = static_cast< StoryBrowserTreeItem* >( dropPlaceItem->parent() );

                  // and insert it behind it
                  insertionIndex = targetItem->indexOfChild( dropPlaceItem ) + 1;
               }
               else
               {
                  targetItem = dropPlaceItem;
                  insertionIndex = targetItem->childCount();
               }

               break;
            }

         case QAbstractItemView::BelowItem:
            {
               targetItem = static_cast< StoryBrowserTreeItem* >( dropPlaceItem->parent() );
               if ( targetItem == NULL )
               {
                  targetItem = dropPlaceItem;
                  insertionIndex = 0;
               }
               else
               {
                  insertionIndex = targetItem->indexOfChild( dropPlaceItem ) + 1;
               }
               break;
            }

         case QAbstractItemView::AboveItem:
            {
               targetItem = static_cast< StoryBrowserTreeItem* >( dropPlaceItem->parent() );
               if ( targetItem == NULL )
               {
                  targetItem = dropPlaceItem;
                  insertionIndex = targetItem->childCount();
               }
               else
               {
                  insertionIndex = targetItem->indexOfChild( dropPlaceItem );
               }
               break;
            }

         case QAbstractItemView::OnViewport:
            {
               targetItem = static_cast< StoryBrowserTreeItem* >( topLevelItem(0) );
               insertionIndex = topLevelItemCount();
               break;
            }
         }
      }

      // insert the nodes into the specified node
      targetItem->insertNode( insertionIndex, nodeToAdd );

      // pull the contents of the dropped nodes
      nodeToAdd->pullStructure( this );

      event->accept();
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::renameItem( QTreeWidgetItem* item, int column )
{
   StoryBrowserTreeItem* clickedItem = static_cast< StoryBrowserTreeItem* >( item );
   renameNode( clickedItem );
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::nodeRenamed( QTreeWidgetItem* item, const QString& newName )
{
   StoryBrowserTreeItem* renamedItem = static_cast< StoryBrowserTreeItem* >( item );

   StoryNode* renamedNode = renamedItem->getNode();
   renamedNode->setName( newName.toStdString().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem )
{
   StoryBrowserTreeItem* btNodeParent = static_cast< StoryBrowserTreeItem* >( parentItem );
   StoryBrowserTreeItem* btNodeRemoved = static_cast< StoryBrowserTreeItem* >( removedItem );
   btNodeParent->removeNode( btNodeRemoved );
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onClearNode( QTreeWidgetItem* cleanedNode )
{
   StoryBrowserTreeItem* btNodeCleaned = static_cast< StoryBrowserTreeItem* >( cleanedNode );
   btNodeCleaned->clear();
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeWidget::onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu )
{
   class AddResourceActionFactory : public MenuActionsFactory
   {
   private:
      StoryBrowserTreeWidget*             m_parentTree;
      StoryBrowserTreeItem*               m_parentItem;
      TreeWidgetDescFactory*              m_itemsFactory;

   public:
      AddResourceActionFactory( StoryBrowserTreeWidget* parentTree, StoryBrowserTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory )
         : m_parentTree( parentTree )
         , m_parentItem( parentItem )
         , m_itemsFactory( itemsFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddStoryNodeAction( icon, desc, itemTypeIdx, m_parentTree, m_parentItem, m_itemsFactory );
         return action;
      }
   };


   // "Add" submenu
   QMenu* addMenu = menu.addMenu( "Add" );
   {
      StoryBrowserTreeItem* menuNode = static_cast< StoryBrowserTreeItem* >( clickedItem );
      if ( menuNode->isContainerNode() )
      {
         AddResourceActionFactory actionsFactory( this, menuNode, m_itemDescriptionsFactory );
         MenuUtils::itemsListMenu( m_itemDescriptionsFactory, actionsFactory, addMenu );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeItem* StoryBrowserTreeWidget::findItem( StoryNode* node )
{
   StoryBrowserTreeItem* rootItem = static_cast< StoryBrowserTreeItem* >( topLevelItem( 0 ) );

   Dequeue< StoryBrowserTreeItem* > itemsQueue;
   itemsQueue.push( rootItem );
   while( !itemsQueue.empty() )
   {
      StoryBrowserTreeItem* checkedItem = itemsQueue.pop();

      if ( node == checkedItem->getNode() )
      {
         // found it
         return checkedItem;
      }

      // collect children
      int childrenCount = checkedItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         StoryBrowserTreeItem* childItem = static_cast< StoryBrowserTreeItem* >( checkedItem->child( i ) );
         itemsQueue.push( childItem );
      }
   }

   // unfortunately we didn't find a match
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddStoryNodeAction::AddStoryNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, StoryBrowserTreeWidget* parentTree, StoryBrowserTreeItem* parentItem, TreeWidgetDescFactory* itemsFactory )
   : QAction( icon, desc, parentTree )
   , m_parentTree( parentTree )
   , m_typeIdx( typeIdx )
   , m_parentItem( parentItem )
   , m_itemsFactory( itemsFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddStoryNodeAction::onTriggered()
{
   const SerializableReflectionType* nodeType = m_itemsFactory->getClass( m_typeIdx );
   ASSERT_MSG( nodeType, "Requested a node of an undefined type" );

   StoryNode* newNode = nodeType->instantiate< StoryNode >();
   m_parentItem->addNode( newNode );
}

///////////////////////////////////////////////////////////////////////////////
