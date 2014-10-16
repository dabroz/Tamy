/// @file   TamyEditor/BTIDecorator.h
/// @brief  behavior tree editor node that represents a BehTreeDecorator

#include "BehTreeWidgetItem.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeDecorator;

///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree editor node that represents a BehTreeDecorator.
 */
class BTIDecorator : public BehTreeWidgetItem
{
private:
   BehTreeDecorator&       m_decorator;

public:
   /**
    * Constructor.
    *
    * @param action
    */
   BTIDecorator( BehTreeDecorator& decorator );

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
