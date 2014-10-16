#include "core-AI\BlendTreeAnimation.h"
#include "core-AI\SnapshotAnimation.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreePlayerListener.h"
#include "core-AI\BlendTree.h"
#include "core-AI\Skeleton.h"
#include "core-AI\BlendTreeAnimationListener.h"
#include "core-AI\BlendTreeAnimationEvent.h"
#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\BlendTreeAnimationTrack.h"
#include "core\ListUtils.h"
#include "core\ArrayUtils.h"
#include "core\CollectionComparators.h"
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeAnimation );
   PARENT( BlendTreeNode );
   PROPERTY_EDIT( "Animation", SnapshotAnimation*, m_animation );
   PROPERTY_EDIT( "Loop animation", bool, m_loopAnimation );
   PROPERTY( Array< BlendTreeAnimationEvent* >, m_btaEvents );
   PROPERTY( Array< BlendTreeSyncPoint* >, m_syncPoints );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimation::BlendTreeAnimation( const char* name, SnapshotAnimation* animation )
   : BlendTreeNode( name )
   , m_animation( animation )
   , m_loopAnimation( true )
   , m_sortSyncPoints( false )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimation::~BlendTreeAnimation()
{
   m_animation = NULL;

   uint count = m_btaEvents.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeAnimationEvent* btaEvent = m_btaEvents[i];
      btaEvent->removeReference();
   }

   count = m_syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* syncPoint = m_syncPoints[i];
      syncPoint->removeReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::setAnimation( SnapshotAnimation* animation )
{
   NOTIFY_PROPERTY_CHANGE( m_animation );
   m_animation = animation;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::addEvent( BlendTreeAnimationEvent* btaEvent )
{
   // check for duplicates
   uint count = m_btaEvents.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeAnimationEvent* ownedBtaEvent = m_btaEvents[i];
      if ( ownedBtaEvent == btaEvent )
      {
         // that's a duplicate
         return;
      }
   }

   m_btaEvents.push_back( btaEvent );

   // notify listeners
   for ( List< BlendTreeAnimationListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreeAnimationListener* listener = *it;
      listener->onEventAdded( btaEvent );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::removeEvent( BlendTreeAnimationEvent* btaEvent )
{
   uint count = m_btaEvents.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeAnimationEvent* ownedBtaEvent = m_btaEvents[i];
      if ( ownedBtaEvent == btaEvent )
      {
         // found it

         // notify listeners
         for ( List< BlendTreeAnimationListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeAnimationListener* listener = *it;
            listener->onEventRemoved( btaEvent );
         }

         // delete the event
         btaEvent->removeReference();
         m_btaEvents.remove( i );

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::addSyncPoint( BlendTreeSyncPoint* syncPoint )
{
   // check for duplicates
   uint count = m_syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* ownedSyncPoint = m_syncPoints[i];
      if ( ownedSyncPoint == syncPoint )
      {
         // that's a duplicate
         return;
      }
   }

   m_syncPoints.push_back( syncPoint );

   // start listening to changes done to the event
   syncPoint->attachListener( *this );

   // notify listeners
   for ( List< BlendTreeAnimationListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreeAnimationListener* listener = *it;
      listener->onSyncPointAdded( syncPoint );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::removeSyncPoint( BlendTreeSyncPoint* syncPoint )
{
   uint count = m_syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* ownedSyncPoint = m_syncPoints[i];
      if ( ownedSyncPoint == syncPoint )
      {
         // found it

         // notify listeners
         for ( List< BlendTreeAnimationListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeAnimationListener* listener = *it;
            listener->onSyncPointRemoved( syncPoint );
         }

         // stop listening to changes done to the event
         syncPoint->detachListener( *this );

         // delete the event
         syncPoint->removeReference();
         m_syncPoints.remove( i );

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::attachEventsListener( BlendTreeAnimationListener* listener )
{
   List< BlendTreeAnimationListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::detachEventsListener( BlendTreeAnimationListener* listener )
{
   List< BlendTreeAnimationListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::pullEvents( BlendTreeAnimationListener* listener )
{
   uint count = m_btaEvents.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeAnimationEvent* btaEvent = m_btaEvents[i];
      listener->onEventAdded( btaEvent );
   }

   count = m_syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* syncPoint = m_syncPoints[i];
      listener->onSyncPointAdded( syncPoint );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::onObjectLoaded()
{
   BlendTreeNode::onObjectLoaded();

   uint count = m_syncPoints.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeSyncPoint* syncPoint = m_syncPoints[i];
      syncPoint->attachListener( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::onObservedPropertyChanged( ReflectionProperty& property )
{
   m_sortSyncPoints = true;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeNode::initializeLayout( player );

   if ( m_sortSyncPoints )
   {
      sortSyncPoints();
      m_sortSyncPoints = false;
   }

   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeSyncPointsList );

   if ( m_animation )
   {
      BlendTreeAnimationTrack* runtimeSyncPointsList = new BlendTreeAnimationTrack( m_syncPoints, m_animation->m_duration, m_loopAnimation );
      data[m_runtimeSyncPointsList] = runtimeSyncPointsList;
   }
   else
   {
      data[m_runtimeSyncPointsList] = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::deinitializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();

   BlendTreeAnimationTrack* runtimeSyncPointsList = data[m_runtimeSyncPointsList];
   delete runtimeSyncPointsList;
   data.unregisterVar( m_runtimeSyncPointsList );

   BlendTreeNode::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::onGenerateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const
{
   if ( !m_animation )
   {
      return;
   }

   RuntimeDataBuffer& data = player->data();
   BlendTreeAnimationTrack* runtimeSyncPointsList = data[m_runtimeSyncPointsList];

   if ( data[m_state] != ToSynchronize )
   {
      // the node can participate in building the tree sync profile only if it's been initially synchronized
      runtimeSyncPointsList->updateTreeProfile( outSyncData );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& treeSyncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
{
   if ( !m_animation )
   {
      return;
   }

   RuntimeDataBuffer& data = player->data();

   // update the node's runtime synchronization profile
   BlendTreeAnimationTrack* runtimeSyncPointsList = data[m_runtimeSyncPointsList];

   if ( data[m_state] == ToSynchronize )
   {
      // synchronize the node to the tree - find the first applicable sync
      // event and jump to it
      runtimeSyncPointsList->jumpToFirstMatch( treeSyncData );
   }

   runtimeSyncPointsList->synchronizeTo( treeSyncData, outNodeSyncProfile );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const
{
   if ( !m_animation )
   {
      return;
   }

   RuntimeDataBuffer& data = player->data();

   BlendTreeAnimationTrack* runtimeSyncPointsList = data[m_runtimeSyncPointsList];

   // update the time track
   const float prevTrackTime = runtimeSyncPointsList->m_trackTime;
   bool animationLooped = runtimeSyncPointsList->update( timeDelta );
   const float newTrackTime = runtimeSyncPointsList->m_trackTime;

   // sample the pose
   m_animation->samplePose( newTrackTime, outGeneratedPose, bonesCount );

   // get the accumulated motion
   {
      Transform startTransform, endTransform;
      m_animation->sampleMotion( prevTrackTime, newTrackTime, outAccMotion );
   }

   // trigger any events that happened along the way
   uint eventsCount = m_btaEvents.size();
   for ( uint i = 0; i < eventsCount; ++i )
   {
      BlendTreeAnimationEvent* btaEvent = m_btaEvents[i];
      BlendTreeEvent* btEvent = btaEvent->getBlendTreeEvent();
      if ( !btEvent )
      {
         // nothing to trigger
         continue;
      }
      float eventTime = btaEvent->getTime() * m_animation->m_duration;

      bool triggerEvent = false;
      if ( animationLooped )
      {
         triggerEvent = ( prevTrackTime <= eventTime || eventTime <= newTrackTime );
      }
      else
      {
         triggerEvent = ( prevTrackTime <= eventTime && eventTime <= newTrackTime );
      }

      if ( triggerEvent )
      {
         player->triggerEvent( btEvent );
      }
   }

   // notify listeners of a completed sync point ( if any )
   if ( runtimeSyncPointsList->m_completedSyncPoint )
   {
      for ( List< BlendTreePlayerListener* >::iterator it = player->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreePlayerListener* listener = *it;
         listener->onSyncPointReached( player, this, runtimeSyncPointsList->m_completedSyncPoint );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimation::sortSyncPoints() const
{
   ArrayUtils::quickSort< BlendTreeSyncPoint* >( m_syncPoints );
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeAnimation::getTrackTime( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   BlendTreeAnimationTrack* runtimeSyncPointsList = data[m_runtimeSyncPointsList];
   return runtimeSyncPointsList->m_trackTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<>
bool Comparator< BlendTreeSyncPoint* >::isLesser( BlendTreeSyncPoint* const & rhs, BlendTreeSyncPoint* const & lhs )
{
   return rhs->getTime() < lhs->getTime();
}

///////////////////////////////////////////////////////////////////////////////

template<>
bool Comparator< BlendTreeSyncPoint* >::isGreater( BlendTreeSyncPoint* const & rhs, BlendTreeSyncPoint* const & lhs )
{
   return rhs->getTime() > lhs->getTime();
}

///////////////////////////////////////////////////////////////////////////////

template<>
bool Comparator< BlendTreeSyncPoint* >::isEqual( BlendTreeSyncPoint* const & rhs, BlendTreeSyncPoint* const & lhs )
{
   return rhs->getTime() == lhs->getTime();
}

///////////////////////////////////////////////////////////////////////////////
