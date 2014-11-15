#include "BlendTreeWidget.h"
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
#include "BDTIGenerator.h"
#include "BDTIComposite.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeWidget::BlendTreeWidget( QWidget* parentWidget, BlendTree& blendTree )
   : TreeWidget( parentWidget, parentWidget->objectName() + "/BlendTree" )
   , m_blendTree( blendTree )
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
   {
      BDTIComposite* rootNode = new BDTIComposite( m_blendTree.getRoot() );

      rootNode->setIcon( 0, QIcon( ":/TamyEditor/Resources/Icons/Engine/BlendTreeRootIcon.png" ) );
      addTopLevelItem( rootNode );
   }

   // create factory associations
   m_itemsFactory = new GenericFactory< BlendTreeNode, BlendTreeWidgetItem >();
   m_itemsFactory->associateAbstract< BlendTreeComposite, BDTIComposite >();
   m_itemsFactory->associateAbstract< BlendTreeNode, BDTIGenerator >(); // this has to come last, as it's the base class for all previously registered classes

   // initialize items descriptors factory
   m_itemDescriptionsFactory = new TypeDescFactory< BlendTreeNode >( "unknownBlendTreeIcon.png" );


   // attach self as a listener and pull initial tree structure
   m_blendTree.addReference();
   m_blendTree.attachBlendTreeListener( this );
   m_blendTree.pullStructure( this );

   // setup common actions
   connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( renameItem( QTreeWidgetItem*, int ) ) );
   connect( this, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
   connect( this, SIGNAL( renameNode( QTreeWidgetItem*, const QString& ) ), this, SLOT( nodeRenamed( QTreeWidgetItem*, const QString& ) ) );
   connect( this, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeWidget::~BlendTreeWidget()
{
   // stop listening to the tree
   m_blendTree.detachBlendTreeListener( this );
   m_blendTree.removeReference();

   delete m_itemsFactory;
   m_itemsFactory = NULL;

   delete m_itemDescriptionsFactory;
   m_itemDescriptionsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node )
{
   BlendTreeWidgetItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      BlendTreeWidgetItem* newItem = m_itemsFactory->create( *node );

      QString desc, group;
      QIcon icon;
      m_itemDescriptionsFactory->getDesc( node->getVirtualRTTI(), desc, group, icon );
      newItem->setIcon( 0, icon );

      parentItem->insertChild( insertionIdx, newItem );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node )
{
   BlendTreeWidgetItem* parentItem = findItem( parentNode );
   if ( parentItem )
   {
      // find the removed child item
      int childrenCount = parentItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         BlendTreeWidgetItem* childItem = static_cast< BlendTreeWidgetItem* >( parentItem->child( i ) );
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

void BlendTreeWidget::onNodeChanged( BlendTreeNode* node )
{
   BlendTreeWidgetItem* item = findItem(node );
   if ( item )
   {
      item->updateDescription();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onNodeStateChanged( BlendTreePlayer* player, const BlendTreeNode* node )
{
   // find the item that corresponds to the node
   BlendTreeWidgetItem* item = findItem( node );
   if ( item->parent() != NULL )
   {
      // update nodes with an exception of the root node - we don't want to illustrate the state its in
      // ( because it wears a custom icon etc. )
      item->updateRuntimeState( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* BlendTreeWidget::mimeData( const QList< QTreeWidgetItem* > items ) const
{
   QMimeData* data = __super::mimeData( items );

   QByteArray encodedData;
   QDataStream stream( &encodedData, QIODevice::WriteOnly );

   for ( QList< QTreeWidgetItem* >::const_iterator it = items.begin(); it != items.end(); ++it )
   {
      QTreeWidgetItem* item = *it;
      BlendTreeWidgetItem* behTreeItem = static_cast< BlendTreeWidgetItem* >( item );

      uint movedItemPtr = (uint)behTreeItem;
      stream << movedItemPtr;
   }

   data->setData( "TamyEditor/BlendTreeWidget", encodedData );
   return data;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* mimeData = event->mimeData();
   QByteArray encodedData = mimeData->data( "TamyEditor/BlendTreeWidget" );
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

void BlendTreeWidget::dropEvent( QDropEvent* event )
{
   // identify the drop point ( category )
   QPoint pos = event->pos();
   BlendTreeWidgetItem* dropPlaceItem = static_cast< BlendTreeWidgetItem* >( itemAt( pos.rx(), pos.ry() ) );

   if ( !dropPlaceItem )
   {
      dropPlaceItem = static_cast< BlendTreeWidgetItem* >( topLevelItem(0) );
   }

   // move selected items to the target category
   List< BlendTreeNode* > nodesToAdd;
   {
      // decode the stream, removing nodes from their previous location and batching them for addition
      const QMimeData* mimeData = event->mimeData();
      QByteArray encodedData = mimeData->data( "TamyEditor/BlendTreeWidget" );
      QDataStream stream( &encodedData, QIODevice::ReadOnly );

      while ( !stream.atEnd() ) 
      {      
         uint movedItemPtr;
         stream >> movedItemPtr;

         BlendTreeWidgetItem* movedItem = ( BlendTreeWidgetItem* )movedItemPtr;
         if ( movedItem == dropPlaceItem )
         {
            // we don't allow dropping self onto oneself
            continue;
         }
         BlendTreeWidgetItem* sourceContainerItem = ( BlendTreeWidgetItem* )movedItem->parent();

         // add a reference so that the item doesn't get deleted
         BlendTreeNode* movedNode = movedItem->getNode();
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
      BlendTreeWidgetItem* targetItem = NULL;
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
                  targetItem = static_cast< BlendTreeWidgetItem* >( dropPlaceItem->parent() );

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
               targetItem = static_cast< BlendTreeWidgetItem* >( dropPlaceItem->parent() );
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
               targetItem = static_cast< BlendTreeWidgetItem* >( dropPlaceItem->parent() );
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
               targetItem = static_cast< BlendTreeWidgetItem* >( topLevelItem(0) );
               insertionIndex = topLevelItemCount();
               break;
            }
         }
      }

      // insert the nodes into the specified node
      targetItem->insertNodes( insertionIndex, nodesToAdd );

      // pull the contents of the dropped nodes
      for ( List< BlendTreeNode* >::iterator it = nodesToAdd.begin(); !it.isEnd(); ++it )
      {
         BlendTreeNode* node = *it;
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

void BlendTreeWidget::renameItem( QTreeWidgetItem* item, int column )
{
   BlendTreeWidgetItem* clickedItem = static_cast< BlendTreeWidgetItem* >( item );
   renameNode( clickedItem );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::nodeRenamed( QTreeWidgetItem* item, const QString& newName )
{
   BlendTreeWidgetItem* renamedItem = static_cast< BlendTreeWidgetItem* >( item );

   BlendTreeNode* renamedNode = renamedItem->getNode();
   renamedNode->setName( newName.toStdString().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onRemoveNode( QTreeWidgetItem* parentItem, QTreeWidgetItem* removedItem )
{
   BlendTreeWidgetItem* btNodeParent = static_cast< BlendTreeWidgetItem* >( parentItem );
   BlendTreeWidgetItem* btNodeRemoved = static_cast< BlendTreeWidgetItem* >( removedItem );
   btNodeParent->removeNode( btNodeRemoved );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onClearNode( QTreeWidgetItem* cleanedNode )
{
   BlendTreeWidgetItem* btNodeCleaned = static_cast< BlendTreeWidgetItem* >( cleanedNode );
   btNodeCleaned->clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidget::onPopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu )
{
   class AddResourceActionFactory : public MenuActionsFactory
   {
   private:
      BlendTreeWidget*              m_parentTree;
      BlendTreeWidgetItem*          m_parentItem;
      TreeWidgetDescFactory*        m_itemsFactory;

   public:
      AddResourceActionFactory( BlendTreeWidget* parentTree, BlendTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory )
         : m_parentTree( parentTree )
         , m_parentItem( parentItem )
         , m_itemsFactory( itemsFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddBlendTreeNodeAction( icon, desc, itemTypeIdx, m_parentTree, m_parentItem, m_itemsFactory );
         return action;
      }
   };


   // "Add" submenu
   QMenu* addMenu = menu.addMenu( "Add" );
   {
      BlendTreeWidgetItem* menuNode = static_cast< BlendTreeWidgetItem* >( clickedItem );
      if ( menuNode->isContainerNode() )
      {
         AddResourceActionFactory actionsFactory( this, menuNode, m_itemDescriptionsFactory );
         MenuUtils::itemsListMenu( m_itemDescriptionsFactory, actionsFactory, addMenu );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

BlendTreeWidgetItem* BlendTreeWidget::findItem( const BlendTreeNode* node )
{
   BlendTreeWidgetItem* rootItem = static_cast< BlendTreeWidgetItem* >( topLevelItem( 0 ) );

   Dequeue< BlendTreeWidgetItem* > itemsQueue;
   itemsQueue.push( rootItem );
   while( !itemsQueue.empty() )
   {
      BlendTreeWidgetItem* checkedItem = itemsQueue.pop();

      if ( node == checkedItem->getNode() )
      {
         // found it
         return checkedItem;
      }

      // collect children
      int childrenCount = checkedItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         BlendTreeWidgetItem* childItem = static_cast< BlendTreeWidgetItem* >( checkedItem->child( i ) );
         itemsQueue.push( childItem );
      }
   }

   // unfortunately we didn't find a match
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddBlendTreeNodeAction::AddBlendTreeNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BlendTreeWidget* parentTree, BlendTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory )
   : QAction( icon, desc, parentTree )
   , m_parentTree( parentTree )
   , m_typeIdx( typeIdx )
   , m_parentItem( parentItem )
   , m_itemsFactory( itemsFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBlendTreeNodeAction::onTriggered()
{
   const SerializableReflectionType* nodeType = m_itemsFactory->getClass( m_typeIdx );
   ASSERT_MSG( nodeType, "Requested a node of an undefined type" );

   BlendTreeNode* newNode = nodeType->instantiate< BlendTreeNode >();
   m_parentItem->addNode( newNode );
}

///////////////////////////////////////////////////////////////////////////////
