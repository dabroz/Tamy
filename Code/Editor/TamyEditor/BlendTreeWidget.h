/// @file   TamyEditor/BlendTreeWidget.h
/// @brief  a tree widget dedicated to blend trees
#pragma once

#include "TreeWidget.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreeListener.h"
#include "core-AI\BlendTreePlayerListener.h"
#include "core\GenericFactory.h"
#include "BlendTreeWidgetItem.h"
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class TreeWidgetDescFactory;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree widget dedicated to blend trees.
 */
class BlendTreeWidget : public TreeWidget, public BlendTreeListener, public BlendTreePlayerListener
{
   Q_OBJECT

private:
   BlendTree&                                               m_blendTree;
   GenericFactory< BlendTreeNode, BlendTreeWidgetItem >*    m_itemsFactory;
   TreeWidgetDescFactory*                                   m_itemDescriptionsFactory;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param blendTree
    */
   BlendTreeWidget( QWidget* parentWidget, BlendTree& blendTree );
   ~BlendTreeWidget();

   /**
    * Looks for an item that corresponds to the specified behavior tree node.
    *
    * @param node
    */
   BlendTreeWidgetItem* findItem( const BlendTreeNode* node );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node );
   void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node );
   void onNodeChanged( BlendTreeNode* node );
   void onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition );
   void onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition );

   // -------------------------------------------------------------------------
   // BlendTreePlayerListener implementation
   // -------------------------------------------------------------------------
   void onNodeStateChanged( BlendTreePlayer* player, const BlendTreeNode* node );

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

class AddBlendTreeNodeAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeWidget*                    m_parentTree;
   BlendTreeWidgetItem*                m_parentItem;
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
   AddBlendTreeNodeAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BlendTreeWidget* parentTree, BlendTreeWidgetItem* parentItem, TreeWidgetDescFactory* itemsFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
