/// @file   core-AI/BehTreeDecorator.h
/// @brief  a behavior tree decorator node
#pragma once

#include "core-AI\BehTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeDecorator : public BehTreeNode
{
   DECLARE_ALLOCATOR( BehTreeDecorator, AM_DEFAULT );
   DECLARE_CLASS();

protected:
   BehTreeNode*         m_childNode;

public:
   /**
    * Constructor.
    */
   BehTreeDecorator();
   virtual ~BehTreeDecorator();

   /**
    * Sets the decorated node
    *
    * @param child
    */
   void setDecoratedNode( BehTreeNode* child );

   /**
    * Returns a pointer to the decorated node.
    */
   inline BehTreeNode* getDecoratedNode() const { return m_childNode; }

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   void createLayout( BehaviorTreeRunner& runner ) const;
   void pullStructure( BehaviorTreeListener* listener );
   void onHostTreeSet( BehaviorTree* tree );
};

///////////////////////////////////////////////////////////////////////////////
