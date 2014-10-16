/// @file   core-AI/BlendTreePlayerListener.h
/// @brief  a BlendTreePlayer listener, that informs of changes in the simulated tree state
#pragma once


///////////////////////////////////////////////////////////////////////////////

class BlendTreePlayer;
class BlendTreeNode;
class BlendTreeEvent;

///////////////////////////////////////////////////////////////////////////////

/**
 * A BlendTreePlayer listener, that informs of changes in the simulated tree state.
 */
class BlendTreePlayerListener
{
public:
   virtual ~BlendTreePlayerListener() {}

   /**
    * Called just before the player starts playing the scene.
    *
    * @param player
    */
   virtual void onSimulationStarted( BlendTreePlayer* player ) {}

   /**
    * Called just after the player finishes playing the scene
    */
   virtual void onSimulationFinished( BlendTreePlayer* player ) {}

   /**
    * Called when a node changes its runtime state ( gets activated or deactivated ).
    *
    * @param player
    * @param node
    */
   virtual void onNodeStateChanged( BlendTreePlayer* player, const BlendTreeNode* node ) {}

   /**
    * Called when an event is triggered by the tree.
    *
    * @param player
    * @param btEvent
    */
   virtual void onEventTriggered( BlendTreePlayer* player, const BlendTreeEvent* btEvent ) {}

   /**
    * Called when a node reaches a synchronization point.
    *
    * @param player
    * @param node
    * @param syncPoint
    */
   virtual void onSyncPointReached( BlendTreePlayer* player, const BlendTreeNode* node, const BlendTreeEvent* syncPoint ) {}
};

///////////////////////////////////////////////////////////////////////////////
