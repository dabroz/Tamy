#pragma once

/// @file   TamyEditor\TreeWidget.h
/// @brief  A tree widget with some basic common functionality

#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class QMenu;
class QPoint;
class QTreeWidgetItem;
class TypeSelectionAction;
class TreeWidgetDescFactory;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree widget with some basic common functionality.
 */
class TreeWidget : public QTreeWidget
{
   Q_OBJECT

private:
   QTreeWidgetItem*        m_popupMenuItem;

   // renaming related data
   QTreeWidgetItem*        m_renamedItem;
   QLineEdit*              m_renameEditor;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param objName       object name
    */
   TreeWidget( QWidget* parent, const QString& objName );

   /**
    * Gives access to an item for which a popup menu was created.
    */
   inline QTreeWidgetItem* getPopupMenuItem() { return m_popupMenuItem; }

   /**
    * Initializes the node renaming procedure.
    *
    * @param node
    */
   void renameNode( QTreeWidgetItem* node );

signals:

   /**
    * Removes a child node form a parent node
    *
    * @param parent
    * @param child
    */
   void removeNode( QTreeWidgetItem* parent, QTreeWidgetItem* child );

   /**
    * Removes all children from the specified node
    *
    * @param node
    */
   void clearNode( QTreeWidgetItem* node );

   /**
    * Emitted when the user renames an item.
    *
    * @param node
    * @param newName
    */
   void renameNode( QTreeWidgetItem* node, const QString& newName );

   /**
    * Called when a pop-up menu is displayed.
    *
    * @param parent
    * @param menu
    */
   void popupMenuShown( QTreeWidgetItem* parent, QMenu& menu );

public slots:
   void showPopupMenu( const QPoint& pos );
   void expandNode();
   void collapseNode();
   void removeNode();
   void renameNode();
   void clearNode();
   void itemRenamed();

private:
   void deepCollapse( QTreeWidgetItem* root, bool expand );
};

///////////////////////////////////////////////////////////////////////////////
