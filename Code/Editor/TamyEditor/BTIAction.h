/// @file   TamyEditor/BTIAction.h
/// @brief  behavior tree editor node that represents a BehTreeAction

#include "BehTreeWidgetItem.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeAction;

///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree editor node that represents a BehTreeAction.
 */
class BTIAction : public BehTreeWidgetItem
{
private:
   BehTreeAction&       m_action;

public:
   /**
    * Constructor.
    *
    * @param action
    */
   BTIAction( BehTreeAction& action );

   // -------------------------------------------------------------------------
   // BehTreeWidgetItem implementation
   // -------------------------------------------------------------------------
   BehTreeNode* getNode();
   void addNode( BehTreeNode* newNode );
   void insertNodes( int insertionIdx, const List< BehTreeNode* >& nodesToInsert );
   void removeNode( BehTreeWidgetItem* removedChildItem );
   void clear();
   bool isContainerNode() const { return false; }
};

///////////////////////////////////////////////////////////////////////////////
