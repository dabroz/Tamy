/// @file   TamyEditor/BDTIGenerator.h
/// @brief  blend tree editor node that represents a leaf node that generates a pose

#include "BlendTreeWidgetItem.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * Blend tree editor node that represents a leaf node that generates a pose.
 */
class BDTIGenerator : public BlendTreeWidgetItem
{
private:
   BlendTreeNode&       m_generator;

public:
   /**
    * Constructor.
    *
    * @param generator
    */
   BDTIGenerator( BlendTreeNode& generator );

   // -------------------------------------------------------------------------
   // BehTreeWidgetItem implementation
   // -------------------------------------------------------------------------
   BlendTreeNode* getNode();
   void addNode( BlendTreeNode* newNode );
   void insertNodes( int insertionIdx, const List< BlendTreeNode* >& nodesToInsert );
   void removeNode( BlendTreeWidgetItem* removedChildItem );
   void clear();
   bool isContainerNode() const { return false; }
};

///////////////////////////////////////////////////////////////////////////////
