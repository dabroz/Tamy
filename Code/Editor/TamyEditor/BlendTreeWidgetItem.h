/// @file   TamyEditor/BlendTreeWidgetItem.h
/// @brief  base blend tree widget item
#pragma once

#include <QtWidgets\QTreeWidgetItem>
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base behavior tree widget item.
 */
class BlendTreeWidgetItem : public QTreeWidgetItem
{
public:
   virtual ~BlendTreeWidgetItem();

   // -------------------------------------------------------------------------
   // Static state
   // -------------------------------------------------------------------------

   /**
    * Returns a reference to the represented BT node.
    */
   virtual BlendTreeNode* getNode() = 0;

   /**
    * Adds a new node to the represented parent BT node.
    *
    * @param newNode
    */
   virtual void addNode( BlendTreeNode* newNode ) = 0;

   /**
    * Inserts nodes at the specified position
    */
   virtual void insertNodes( int insertionIdx, const List< BlendTreeNode* >& nodesToInsert ) = 0;

   /**
    * Removes the represented child BT node from the represented parent BT node.
    *
    * @param removedChildItem
    */
   virtual void removeNode( BlendTreeWidgetItem* removedChildItem ) = 0;

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

   // -------------------------------------------------------------------------
   // Runtime state
   // -------------------------------------------------------------------------
   /**
    * Updates the item so that it reflects what runtime state the node is currently in
    * in the context of the specified tree player.
    *
    * @param player
    */
   void updateRuntimeState( BlendTreePlayer* player );
};

///////////////////////////////////////////////////////////////////////////////
