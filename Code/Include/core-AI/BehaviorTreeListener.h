/// @file   core-AI/BehaviorTreeListener.h
/// @brief  a behavior tree listener interface
#pragma once


///////////////////////////////////////////////////////////////////////////////

class BehTreeNode;
class BehaviorTreeVariable;

///////////////////////////////////////////////////////////////////////////////

/**
 * A behavior tree listener interface.
 */
class BehaviorTreeListener
{
public:
   virtual ~BehaviorTreeListener() {}

   /**
    * Called when a new node was added to the tree.
    *
    * @param parentNode
    * @param insertionIdx
    * @param node
    */
   virtual void onNodeAdded( BehTreeNode* parentNode, int insertionIdx, BehTreeNode* node ) {}

   /**
    * Called when a node is about to be removed from the tree.
    *
    * @param parentNode
    * @param node
    */
   virtual void onNodeRemoved( BehTreeNode* parentNode, BehTreeNode* node ) {}

   /**
    * Called when something about a node changes.
    *
    * @param node
    */
   virtual void onNodeChanged( BehTreeNode* node ) {} 

   /**
    * Called when a new variable is added to the tree.
    *
    * @param var
    */
   virtual void onVariableAdded( BehaviorTreeVariable* var ) {}

   /**
    * Called when a variable is about to be removed from the tree.
    *
    * @param var
    */
   virtual void onVariableRemoved( BehaviorTreeVariable* var ) {}
};

///////////////////////////////////////////////////////////////////////////////
