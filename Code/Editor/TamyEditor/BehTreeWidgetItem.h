/// @file   TamyEditor/BehTreeWidgetItem.h
/// @brief  base behavior tree widget item
#pragma once

#include <QtWidgets\QTreeWidgetItem>
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base behavior tree widget item.
 */
class BehTreeWidgetItem : public QTreeWidgetItem
{
public:
   virtual ~BehTreeWidgetItem();

   /**
    * Returns a reference to the represented BT node.
    */
   virtual BehTreeNode* getNode() = 0;

   /**
    * Adds a new node to the represented parent BT node.
    *
    * @param newNode
    */
   virtual void addNode( BehTreeNode* newNode ) = 0;

   /**
    * Inserts nodes at the specified position
    */
   virtual void insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert ) = 0;

   /**
    * Removes the represented child BT node from the represented parent BT node.
    *
    * @param removedChildItem
    */
   virtual void removeNode( BehTreeWidgetItem* removedChildItem ) = 0;

   /**
    * Clears the contents of the represented BT node.
    */
   virtual void clear() = 0;

   /**
    * Is the node capable of storing embedded nodes.
    */
   virtual bool isContainerNode() const { return true; }

   /**
    * Updates item's description.
    */
   void updateDescription();
};

///////////////////////////////////////////////////////////////////////////////
