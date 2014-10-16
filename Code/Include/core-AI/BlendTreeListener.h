/// @file   core-AI/BlendTreeListener.h
/// @brief  a blend tree listener interface
#pragma once


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreeStateTransition;
class BlendTreeStateMachine;
class BlendTreeEvent;
class BlendTreeVariable;

///////////////////////////////////////////////////////////////////////////////

/**
 * A blend tree listener interface.
 */
class BlendTreeListener
{
public:
   virtual ~BlendTreeListener() {}

   /**
    * Called when a new node was added to the tree.
    *
    * @param parentNode
    * @param insertionIdx
    * @param node
    */
   virtual void onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node ) {}

   /**
    * Called when a node is about to be removed from the tree.
    *
    * @param parentNode
    * @param node
    */
   virtual void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node ) {}

   /**
    * Called when something about a node changes.
    *
    * @param node
    */
   virtual void onNodeChanged( BlendTreeNode* node ) {}

   /**
    * Called when a new transition is added to a state machine.
    *
    * @param parentStateMachine
    * @param transition
    */
   virtual void onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition ) {}

   /**
    * Called when a transition is removed from a state machine.
    *
    * @param parentStateMachine
    * @param transition
    */
   virtual void onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition ) {}

   /**
    * Called when a new event is added to the blend tree.
    *
    * @param btEvent
    */
   virtual void onEventAdded( BlendTreeEvent* btEvent ) {}

   /**
    * Called when an event is removed to the blend tree.
    *
    * @param btEvent
    */
   virtual void onEventRemoved( BlendTreeEvent* btEvent ) {}

   /**
    * Called when a new variable is added to the blend tree.
    *
    * @param btVariable
    */
   virtual void onVariableAdded( BlendTreeVariable* btVariable ) {}

   /**
    * Called when a variable is removed to the blend tree.
    *
    * @param btVariable
    */
   virtual void onVariableRemoved( BlendTreeVariable* btVariable ) {}
};

///////////////////////////////////////////////////////////////////////////////
