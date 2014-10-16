/// @file   TamyEditor/BTIComposite.h
/// @brief  behavior tree editor node that represents a BehTreeComposite

#include "BehTreeWidgetItem.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeComposite;

///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree editor node that represents a BehTreeComposite.
 */
class BTIComposite : public BehTreeWidgetItem
{
private:
   BehTreeComposite&       m_composite;

public:
   /**
    * Constructor.
    *
    * @param action
    */
   BTIComposite( BehTreeComposite& composite );

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
