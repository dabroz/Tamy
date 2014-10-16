/// @file   core-AI/BehTreeComposite.h
/// @brief  a composite behavior tree node
#pragma once

#include "core-AI\BehTreeNode.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;

///////////////////////////////////////////////////////////////////////////////

/**
 * A composite behavior tree node.
 */
class BehTreeComposite : public BehTreeNode
{
   DECLARE_ALLOCATOR( BehTreeComposite, AM_DEFAULT );
   DECLARE_CLASS();

protected:
   Array< BehTreeNode* >      m_nodes;

public:
   virtual ~BehTreeComposite();

   /**
    * Adds a new child node to the tree.
    */
   void add( BehTreeNode* node );

   /**
    * Inserts a node at the specified position.
    *
    * @param index
    * @param node
    */
   void insert( int index, BehTreeNode* node );

   /**
    * Removes a child node from the tree.
    */
   void remove( BehTreeNode* node );

   /**
    * Clears the contents of this node.
    */
   void clear();

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void pullStructure( BehaviorTreeListener* listener );
   void onHostTreeSet( BehaviorTree* tree );
};

///////////////////////////////////////////////////////////////////////////////
