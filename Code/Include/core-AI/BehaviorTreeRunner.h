/// @file   core-AI/BehaviorTreeRunner.h
/// @brief  runtime behavior tree player
#pragma once

#include "core\MemoryRouter.h"
#include "core-AI\BehaviorTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class BehTreeNode;
class RuntimeDataBuffer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Runtime behavior tree player.
 */
class BehaviorTreeRunner : public BehaviorTreeListener
{
   DECLARE_ALLOCATOR( BehaviorTreeRunner, AM_DEFAULT );

private:
   BehaviorTree&           m_tree;
   BehTreeNode*            m_root;
   RuntimeDataBuffer*      m_runtimeData;
   void*                   m_context;

public:
   /**
    * Constructor.
    *
    * @param tree
    * @param context       Custom data the runner will pass to running nodes.
    *                      It's a way of letting nodes communicate with the game.  
    */
   BehaviorTreeRunner( BehaviorTree& tree, void* context = NULL );
   ~BehaviorTreeRunner();

   /**
    * Returns the behavior tree instance.
    */
   inline BehaviorTree& getTree() const { return m_tree; }

   /**
    * Returns a pointer to context data.
    */
   inline void* getContext() const { return m_context; }

   /**
    * Executes the next stage of the tree.
    *
    * @returns 'true' if the tree is being executed, 'false' if the execution is complete
    */
   bool execute();

   /**
    * Restarts tree execution.
    */
   void restart();

   /**
    * Returns the runtime data buffer used during simulation.
    */
   inline RuntimeDataBuffer& data() { return *m_runtimeData; }

   // -------------------------------------------------------------------------
   // BehaviorTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BehTreeNode* parentNode, int insertionIdx, BehTreeNode* node );
   void onNodeRemoved( BehTreeNode* parentNode, BehTreeNode* node );
   void onNodeChanged( BehTreeNode* node ); 
   void onVariableAdded( BehaviorTreeVariable* var );
   void onVariableRemoved( BehaviorTreeVariable* var );
};

///////////////////////////////////////////////////////////////////////////////
