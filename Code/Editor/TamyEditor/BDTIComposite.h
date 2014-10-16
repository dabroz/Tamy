/// @file   TamyEditor/BDTIComposite.h
/// @brief  blend tree editor node that represents a BlendTreeComposite

#include "BlendTreeWidgetItem.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeComposite;

///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree editor node that represents a BlendTreeComposite.
 */
class BDTIComposite : public BlendTreeWidgetItem
{
private:
   BlendTreeComposite&        m_composite;

public:
   /**
    * Constructor.
    *
    * @param action
    */
   BDTIComposite( BlendTreeComposite& composite );

   // -------------------------------------------------------------------------
   // BlendTreeWidgetItem implementation
   // -------------------------------------------------------------------------
   BlendTreeNode* getNode();
   void addNode( BlendTreeNode* newNode );
   void insertNodes( int insertionIdx, const List< BlendTreeNode* >& nodesToInsert );
   void removeNode( BlendTreeWidgetItem* removedChildItem );
   void clear();
};

///////////////////////////////////////////////////////////////////////////////
