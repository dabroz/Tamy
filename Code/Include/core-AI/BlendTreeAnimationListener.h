/// @file   core-AI/BlendTreeAnimationListener.h
/// @brief  a listener that notifies about changes in consistency of events in an animation node
#pragma once


///////////////////////////////////////////////////////////////////////////////

class BlendTreeAnimationEvent;
class BlendTreeSyncPoint;

///////////////////////////////////////////////////////////////////////////////

/**
 * A listener that notifies about changes in consistency of events in an animation node.
 */
class BlendTreeAnimationListener
{
public:
   virtual ~BlendTreeAnimationListener() {}

   /**
    * Called when a new event is added to the observed animation node.
    *
    * @param btaEvent
    */
   virtual void onEventAdded( BlendTreeAnimationEvent* btaEvent ) = 0;

   /**
    * Called when an event is removed from the observed animation node.
    *
    * @param btaEvent
    */
   virtual void onEventRemoved( BlendTreeAnimationEvent* btaEvent ) = 0;

   /**
    * Called when a new sync point is added to the observed animation node.
    *
    * @param syncPoint
    */
   virtual void onSyncPointAdded( BlendTreeSyncPoint* syncPoint ) = 0;

   /**
    * Called when a sync point is removed from the observed animation node.
    *
    * @param syncPoint
    */
   virtual void onSyncPointRemoved( BlendTreeSyncPoint* syncPoint ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
