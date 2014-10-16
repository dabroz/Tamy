/// @file   core-AI/BehTreeAction.h
/// @brief  behavior tree leaf node that performs certain activities
#pragma once

#include "core-AI\BehTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Behavior tree leaf node that performs certain activities.
 */
class BehTreeAction : public BehTreeNode
{
   DECLARE_ALLOCATOR( BehTreeAction, AM_DEFAULT );
   DECLARE_CLASS();

public:
   virtual ~BehTreeAction();

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   void pullStructure( BehaviorTreeListener* listener );
   void onHostTreeSet( BehaviorTree* tree );
};

///////////////////////////////////////////////////////////////////////////////
