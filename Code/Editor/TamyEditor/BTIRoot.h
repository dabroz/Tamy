/// @file   TamyEditor/BTIRoot.h
/// @brief  behavior tree editor node that represents the tree root node

#include "BehTreeWidgetItem.h"



///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;

///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree editor node that represents the tree root.
 */
class BTIRoot : public BehTreeWidgetItem
{
private:
   BehaviorTree&       m_tree;

public:
   /**
    * Constructor.
    *
    * @param parentTreeWidget
    * @param tree
    */
   BTIRoot( QTreeWidget* parentTreeWidget, BehaviorTree& tree );

   // -------------------------------------------------------------------------
   // BehTreeWidgetItem implementation
   // -------------------------------------------------------------------------
   BehTreeNode* getNode();
   void addNode( BehTreeNode* newNode );
   void insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert );
   void removeNode( BehTreeWidgetItem* removedChildItem );
   void clear();
};

///////////////////////////////////////////////////////////////////////////////
