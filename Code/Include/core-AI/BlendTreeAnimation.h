/// @file   core-AI/BlendTreeAnimation.h
/// @brief  This node generates a pose from the specified animation
#pragma once

#include "core-AI\BlendTreeNode.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\RuntimeData.h"
#include "core\List.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class SnapshotAnimation;
class BlendTreeAnimationListener;
class BlendTreeAnimationEvent;
class BlendTreeSyncPoint;
class BlendTreeAnimationTrack;

///////////////////////////////////////////////////////////////////////////////

/**
 * This node generates a pose from the specified animation.
 */
class BlendTreeAnimation : public BlendTreeNode, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( BlendTreeAnimation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   SnapshotAnimation*                        m_animation;
   bool                                      m_loopAnimation;
   Array< BlendTreeAnimationEvent* >         m_btaEvents;
   mutable Array< BlendTreeSyncPoint* >      m_syncPoints;              // We need to make this member mutable, because we want to
                                                                        // perform its lazy sorting, that's gonna
                                                                        // be done when the node is first being played.
                                                                        // Subsequent calls to this node will find this list
                                                                        // already ordered.

   // runtime data
   mutable bool                              m_sortSyncPoints;
   TRuntimeVar< BlendTreeAnimationTrack* >   m_runtimeSyncPointsList;

   List< BlendTreeAnimationListener* >       m_listeners;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param animation
    */
   BlendTreeAnimation( const char* name = "Animation", SnapshotAnimation* animation = NULL );
   ~BlendTreeAnimation();

   /**
    * Sets an animation clip this node should play.
    *
    * @param animation
    */
   void setAnimation( SnapshotAnimation* animation );

   /**
    * Returns a pointer to an animation clip this node plays.
    */
   inline SnapshotAnimation* getAnimation() const { return m_animation; }

   // -------------------------------------------------------------------------
   // Runtime data API
   // -------------------------------------------------------------------------

   /**
    * Returns current track time.
    *
    * @param player
    */
   float getTrackTime( BlendTreePlayer* player ) const;

   // -------------------------------------------------------------------------
   // Animation events & sync points
   // -------------------------------------------------------------------------

   /**
    * Adds a new animation event.
    *
    * @param btaEvent
    */
   void addEvent( BlendTreeAnimationEvent* btaEvent );

   /**
    * Removes an animation event.
    *
    * @param btaEvent
    */
   void removeEvent( BlendTreeAnimationEvent* btaEvent );

   /**
    * Adds a new synchronization point.
    *
    * @param syncPoint
    */
   void addSyncPoint( BlendTreeSyncPoint* syncPoint );

   /**
    * Removes a synchronization point.
    *
    * @param syncPoint
    */
   void removeSyncPoint( BlendTreeSyncPoint* syncPoint );

   /**
    * Pulls all events currently set on this node.
    *
    * @param listener
    */
   void pullEvents( BlendTreeAnimationListener* listener );

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------

   /**
    * Attaches a listener.
    *
    * @param listener
    */
   void attachEventsListener( BlendTreeAnimationListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachEventsListener( BlendTreeAnimationListener* listener );

   // -------------------------------------------------------------------------
   // BlendTreeNode implementation
   // -------------------------------------------------------------------------
   void initializeLayout( BlendTreePlayer* player ) const override;
   void deinitializeLayout( BlendTreePlayer* player ) const override;
   void onGenerateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const override;
   void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const override;
   void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const override;

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property ) override;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

private:
   void sortSyncPoints() const;
};

///////////////////////////////////////////////////////////////////////////////
