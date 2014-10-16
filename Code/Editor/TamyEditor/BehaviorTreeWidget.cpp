#include "BehaviorTreeWidget.h"
#include "core-AI.h"
#include "core\Dequeue.h"
#include <QtGui\QDropEvent>
#include <QtGui\QDragMoveEvent>
#include <QtWidgets\QLineEdit>
#include <QtCore\QMimeData>

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"

// node representations
#include "BTIRoot.h"
#include "BTIAction.h"
#include "BTIDecorator.h"
#include "BTIComposite.h"


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeWidget::BehaviorTreeWidget( QWidget* parentWidget, BehaviorTree& behTree )
   : TreeWidget( parentWidget, parentWidget->objectName() + "/BehaviorTree" )
   , m_behTree( behTree )
{
   setDefaultDropAction( Qt::IgnoreAction );
   setSelectionMode( QAbstractItemView::SingleSelection );
   setDragDropMode( QAbstractItemView::DragDrop );
   setDropIndicatorShown( true );

   setColumnCount( 1 );
   QStringList headerLabels;
   headerLabels.push_back( "Name" );
   setHeaderLabels( headerLabels );

   // create the root node
   BTIRoot* rootNode = new BTIRoot( this, m_behTree );

   // create factory associations
   m_itemsFactory = new GenericFactory< BehTreeNode, BehTreeWidgetItem >();
   m_itemsFactory->associateAbstract< BehTreeAction, BTIAction >();
   m_itemsFactory->associateAbstract< BehTreeDecorator, BTIDecorator >();
   m_itemsFactory->associateAbstract< BehTreeComposite, BTIComposite >();

   // initialize items descriptors factory
   m_itemDescriptionsFactory = new TypeDescFactory< BehTreeNode >( "unknownBTIcon.png" );


   // attach self as a listener and pull initial tree structure
   m_behTree.addReference();
   m_behTree.attachListener( this );
   m_behTree.pullStructure( this );

   // setup common actions
   connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( renameItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( renameNode( QTreeWidgetItem*, const QString& ) ), this, SLOT( nodeRenamed( QTreeWidgetItem*, const QString& ) ) );
   connect( this, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeWidget::~BehaviorTreeWidget()
{
   // stop listening to the tree
   m_behTree.detachListener( this );
   m_behTree.removeReference();

   delete m_itemsFactory;
   m_itemsFactory = NULL;

   delete m_itemDescriptionsFactory;
   m_itemDescriptionsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onNodeAdded( BehTreeNode* parentNode, int insertionIdx, BehTreeNode* node )
{
   BehTreeWidgetItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      BehTreeWidgetItem* newItem = m_itemsFactory->create( *node );

      QString desc, group;
      QIcon icon;
      m_itemDescriptionsFactory->getDesc( node->getVirtualRTTI(), desc, group, icon );
      newItem->setIcon( 0, icon );

      parentItem->insertChild( insertionIdx, newItem );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onNodeRemoved( BehTreeNode* parentNode, BehTreeNode* node )
{
   BehTreeWidgetItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      // find the removed child item
      int childrenCount = parentItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         BehTreeWidgetItem* childItem = static_cast< BehTreeWidgetItem* >( parentItem->child( i ) );
         if ( childItem->getNode() == node )
         {
            parentItem->removeChild( childItem );

            // found it
            return;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onNodeChanged( BehTreeNode* node )
{
   BehTreeWidgetItem* item = findItem(node );
   if ( item )
   {
      item->updateDescription();
   }
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* BehaviorTreeWidget::mimeData( const QList< QTreeWidgetItem* > items ) const
{
   QMimeData* data = __super::mimeData( items );

   QByteArray encodedData;
   QDataStream stream( &encodedData, QIODevice::WriteOnly );

   for ( QList< QTreeWidgetItem* >::const_iterator it = items.begin(); it != items.end(); ++it )
   {
      QTreeWidgetItem* item = *it;
      BehTreeWidgetItem* behTreeItem = static_cast< BehTreeWidgetItem* >( item );
      
      uint movedItemPtr = (uint)behTreeItem;
      stream << movedItemPtr;
   }

   data->setData( "TamyEditor/BehaviorTreeWidget", encodedData );
   return data;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* mimeData = event->mimeData();
   QByteArray encodedData = mimeData->data( "TamyEditor/BehaviorTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   if ( !stream.atEnd() )
   {
      __super::dragMoveEvent( event );
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::dropEvent( QDropEvent* event )
{
   // identify the drop point ( category )
   QPoint pos = event->pos();
   BehTreeWidgetItem* dropPlaceItem = static_cast< BehTreeWidgetItem* >( itemAt( pos.rx(), pos.ry() ) );

   if ( !dropPlaceItem )
   {
      dropPlaceItem = static_cast< BehTreeWidgetItem* >( topLevelItem(0) );
   }

   // move selected items to the target category
   List< BehTreeNode* > nodesToAdd;
   {
      // decode the stream, removing nodes from their previous location and batching them for addition
      const QMimeData* mimeData = event->mimeData();
      QByteArray encodedData = mimeData->data( "TamyEditor/BehaviorTreeWidget" );
      QDataStream stream( &encodedData, QIODevice::ReadOnly );

      while ( !stream.atEnd() ) 
      {      
         uint movedItemPtr;
         stream >> movedItemPtr;

         BehTreeWidgetItem* movedItem = ( BehTreeWidgetItem* )movedItemPtr;
         if ( movedItem == dropPlaceItem )
         {
            // we don't allow dropping self onto oneself
            continue;
         }
         BehTreeWidgetItem* sourceContainerItem = ( BehTreeWidgetItem* )movedItem->parent();

         // add a reference so that the item doesn't get deleted
         BehTreeNode* movedNode = movedItem->getNode();
         movedNode->addReference();

         // put them up for further addition
         nodesToAdd.pushBack( movedNode );

         // remove this node from its previous parent
         sourceContainerItem->removeNode( movedItem );
      }
   }

   if( !nodesToAdd.empty() )
   {
      // find the target place and the insertion index
      BehTreeWidgetItem* targetItem = NULL;
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
                  targetItem = static_cast< BehTreeWidgetItem* >( dropPlaceItem->parent() );

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
               targetItem = static_cast< BehTreeWidgetItem* >( dropPlaceItem->parent() );
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
               targetItem = static_cast< BehTreeWidgetItem* >( dropPlaceItem->parent() );
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
               targetItem = static_cast< BehTreeWidgetItem* >( topLevelItem(0) );
               insertionIndex = topLevelItemCount();
               break;
            }
         }
      }

      // insert the nodes into the specified node
      targetItem->insertNodes( insertionIndex, nodesToAdd );

      // pull the contents of the dropped nodes
      for ( List< BehTreeNode* >::iterator it = nodesToAdd.begin(); !it.isEnd(); ++it )
      {
         BehTreeNode* node = *it;
         node->pullStructure( this );
      }

      event->accept();
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::renameItem( QTreeWidgetItem* item, int column )
{
   BehTreeWidgetItem* clickedItem = static_cast< BehTreeWidgetItem* >( item );
   renameNode( clickedItem );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::nodeRenamed( QTreeWidgetItem* item, const QString& newName )
{
   BehTreeWidgetItem* renamedItem = static_cast< BehTreeWidgetItem* >( item );

   BehTreeNode* renamedNode = renamedItem->getNode();
   renamedNode->setName( newName.toStdString().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem )
{
   BehTreeWidgetItem* btNodeParent = static_cast< BehTreeWidgetItem* >( parentItem );
   BehTreeWidgetItem* btNodeRemoved = static_cast< BehTreeWidgetItem* >( removedItem );
   btNodeParent->removeNode( btNodeRemoved );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onClearNode( QTreeWidgetItem* cleanedNode )
{
   BehTreeWidgetItem* btNodeCleaned = static_cast< BehTreeWidgetItem* >( cleanedNode );
   btNodeCleaned->clear();
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeWidget::onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu )
{
   class AddResourceActionFactory : public MenuActionsFactory
   {
   private:
      BehaviorTreeWidget*           m_parentTree;
      BehTreeWidgetItem*            m_parentItem;
      TreeWidgetDescFactory*        m_itemsFactory;

   public:
      AddResourceActionFactory( BehaviorTreeWidget* parentTree, BehTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory )
         : m_parentTree( parentTree )
         , m_parentItem( parentItem )
         , m_itemsFactory( itemsFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddBehTreeNodeAction( icon, desc, itemTypeIdx, m_parentTree, m_parentItem, m_itemsFactory );
         return action;
      }
   };


   // "Add" submenu
   QMenu* addMenu = menu.addMenu( "Add" );
   {
      BehTreeWidgetItem* menuNode = static_cast< BehTreeWidgetItem* >( clickedItem );
      if ( menuNode->isContainerNode() )
      {
         AddResourceActionFactory actionsFactory( this, menuNode, m_itemDescriptionsFactory );
         MenuUtils::itemsListMenu( m_itemDescriptionsFactory, actionsFactory, addMenu );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

BehTreeWidgetItem* BehaviorTreeWidget::findItem( BehTreeNode* node )
{
   BehTreeWidgetItem* rootItem = static_cast< BehTreeWidgetItem* >( topLevelItem( 0 ) );

   Dequeue< BehTreeWidgetItem* > itemsQueue;
   itemsQueue.push( rootItem );
   while( !itemsQueue.empty() )
   {
      BehTreeWidgetItem* checkedItem = itemsQueue.pop();

      if ( node == checkedItem->getNode() )
      {
         // found it
         return checkedItem;
      }

      // collect children
      int childrenCount = checkedItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         BehTreeWidgetItem* childItem = static_cast< BehTreeWidgetItem* >( checkedItem->child( i ) );
         itemsQueue.push( childItem );
      }
   }

   // unfortunately we didn't find a match
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddBehTreeNodeAction::AddBehTreeNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BehaviorTreeWidget* parentTree, BehTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory )
   : QAction( icon, desc, parentTree )
   , m_parentTree( parentTree )
   , m_typeIdx( typeIdx )
   , m_parentItem( parentItem )
   , m_itemsFactory( itemsFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBehTreeNodeAction::onTriggered()
{
   const SerializableReflectionType* nodeType = m_itemsFactory->getClass( m_typeIdx );
   ASSERT_MSG( nodeType, "Requested a node of an undefined type" );

   BehTreeNode* newNode = nodeType->instantiate< BehTreeNode >();
   m_parentItem->addNode( newNode );
}

///////////////////////////////////////////////////////////////////////////////
