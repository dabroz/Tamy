/// @file   TamyEditor/BehaviorTreeWidget.h
/// @brief  a tree widget dedicated to behavior trees
#pragma once

#include "TreeWidget.h"
#include "core-AI\BehTreeNode.h"
#include "core-AI\BehaviorTreeListener.h"
#include "core\GenericFactory.h"
#include "BehTreeWidgetItem.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class TreeWidgetDescFactory;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree widget dedicated to behavior trees.
 */
class BehaviorTreeWidget : public TreeWidget, public BehaviorTreeListener
{
   Q_OBJECT

private:
   BehaviorTree&                                            m_behTree;
   GenericFactory< BehTreeNode, BehTreeWidgetItem >*        m_itemsFactory;
   TreeWidgetDescFactory*                                   m_itemDescriptionsFactory;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param behTree
    */
   BehaviorTreeWidget( QWidget* parentWidget, BehaviorTree& behTree );
   ~BehaviorTreeWidget();

   /**
    * Looks for an item that corresponds to the specified behavior tree node.
    *
    * @param node
    */
   BehTreeWidgetItem* findItem( BehTreeNode* node );

   // -------------------------------------------------------------------------
   // BehaviorTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BehTreeNode* parentNode, int insertionIdx, BehTreeNode* node );
   void onNodeRemoved( BehTreeNode* parentNode, BehTreeNode* node );
   void onNodeChanged( BehTreeNode* node );

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

class AddBehTreeNodeAction : public QAction
{
   Q_OBJECT

private:
   BehaviorTreeWidget*                 m_parentTree;
   BehTreeWidgetItem*                  m_parentItem;
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
   AddBehTreeNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BehaviorTreeWidget* parentTree, BehTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
