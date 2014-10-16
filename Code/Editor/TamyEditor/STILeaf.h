/// @file   TamyEditor/STILeaf.h
/// @brief  story tree item that represents a leaf node of a story tree

#include "StoryBrowserTreeItem.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNode;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story tree item  node that represents a leaf node of a story tree.
 */
class STILeaf : public StoryBrowserTreeItem
{
private:
   StoryNode&           m_leaf;

public:
   /**
    * Constructor.
    *
    * @param leaf
    */
   STILeaf( StoryNode& leaf );

   // -------------------------------------------------------------------------
   // StoryBrowserTreeItem implementation
   // -------------------------------------------------------------------------
   StoryNode* getNode();
   void addNode( StoryNode* newNode );
   void insertNode( int insertionIdx, StoryNode* nodeToInsert );
   void removeNode( StoryBrowserTreeItem* removedChildItem );
   void clear();
   bool isContainerNode() const { return false; }
};

///////////////////////////////////////////////////////////////////////////////
