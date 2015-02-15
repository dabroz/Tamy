#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeListener.h"
#include "core-AI\BlendTreePlayerListener.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\Skeleton.h"
#include "core\List.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BlendTreeNode );
   PARENT( ReflectionObject );
   PROPERTY( BlendTree*, m_tree );
   PROPERTY_EDIT( "Node name", std::string, m_name );
   PROPERTY( Vector, m_positionOnLayout );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeNode::BlendTreeNode( const char* name )
   : m_tree( NULL )
   , m_name( name )
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::setHostTree( BlendTree* tree )
{
   ASSERT_MSG( ( tree != NULL && m_tree == NULL ) || ( tree == NULL && m_tree != NULL ) , "This node is already a part of a blend tree" );

   m_tree = tree;
   onHostTreeSet( tree );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::setPosition( const Vector& position )
{
   m_positionOnLayout = position;

   // since it's an editor thing only, and a very dedicated functionality at that, we won't 
   // be informing the listeners about the change
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::setName( const char* nodeName )
{
   if ( nodeName )
   {
      m_name = nodeName;
   }
   else
   {
      m_name = "";
   }

   // notify listeners
   if ( m_tree )
   {
      for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreeListener* listener = *it;
         listener->onNodeChanged( this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::onPropertyChanged( ReflectionProperty& property )
{
   // notify listeners
   for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreeListener* listener = *it;
      listener->onNodeChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_generatedPose );
   data.registerVar( m_accumulatedMotion );
   data.registerVar( m_nodeSyncData );
   data.registerVar( m_playbackSpeed );
   data.registerVar( m_state );

   uint bonesCount = player->getSourceBoneCount();
   data[m_generatedPose] = new Transform[bonesCount];

   data[m_nodeSyncData] = new BlendTreeNodeSyncProfile( this );

   data[m_playbackSpeed] = 1.0f;
   data[m_state] = Inactive;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::deinitializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();

   Transform* generatedPose = data[m_generatedPose];
   delete [] generatedPose;

   BlendTreeNodeSyncProfile* syncData = data[m_nodeSyncData];
   delete syncData;

   data.unregisterVar( m_generatedPose );
   data.unregisterVar( m_accumulatedMotion );
   data.unregisterVar( m_nodeSyncData );
   data.unregisterVar( m_playbackSpeed );
   data.unregisterVar( m_state );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::activateNode( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   if ( data[m_state] != Inactive )
   {
      // the node's already active
      return;
   }
   data[m_state] = ToSynchronize;

   // notify about the state change
   for ( List< BlendTreePlayerListener* >::iterator it = player->m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreePlayerListener* listener = *it;
      listener->onNodeStateChanged( player, this );
   }

   onActivateNode( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::deactivateNode( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   if ( data[m_state] == Inactive )
   {
      // the node's already inactive
      return;
   }
   data[m_state] = Inactive;

   // notify about the state change
   for ( List< BlendTreePlayerListener* >::iterator it = player->m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreePlayerListener* listener = *it;
      listener->onNodeStateChanged( player, this );
   }

   onDeactivateNode( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::updateLogic( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   State currState = data[m_state];
   ASSERT_MSG( currState != Inactive, "The node hasn't been activated yet" );

   onUpdateLogic( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::generateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const
{
   RuntimeDataBuffer& data = player->data();

   State currState = data[m_state];
   switch( currState )
   {
   case ToSynchronize:
      {
         // Even though the node became active, it's still out of sync.
         // We need to wait until the tree generates new sync profile to which 
         // it will be able to adjust itself.
         break;
      }

   case Active:
      {
         onGenerateTreeSyncProfile( player, outSyncData );
         break;
      }

   default:
      {
         ASSERT_MSG( false, "This method shouldn't have been called when the node is in this state" );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::synchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData ) const
{
   RuntimeDataBuffer& data = player->data();
   State currState = data[m_state];
   ASSERT_MSG( currState != Inactive, "This state hasn't been activated yet" );

   BlendTreeNodeSyncProfile* nodeSyncProfile = accessSyncData( player );

   onSynchronizeNodeToTree( player, syncData, *nodeSyncProfile );

   nodeSyncProfile->commit();
   nodeSyncProfile->applyVelocityChanges( player );

   if ( currState == ToSynchronize )
   {
      data[m_state] = Active;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::samplePose( BlendTreePlayer* player, float timeDelta ) const
{
   RuntimeDataBuffer& data = player->data();
   ASSERT_MSG( data[m_state] == Active, "The node hasn't been activated yet" );

   uint bonesCount = player->getSourceBoneCount();

   Transform* generatedPose = data[m_generatedPose];
   const float playbackSpeed = data[m_playbackSpeed];
   ASSERT_MSG( playbackSpeed > 0.0f, "Invalid playback speed value" );

   Transform& accumulatedMotion = data[m_accumulatedMotion];

   float playbackTimeDelta = timeDelta * playbackSpeed;
   onSamplePose( player, playbackTimeDelta, generatedPose, accumulatedMotion, bonesCount );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeNode::State BlendTreeNode::getState( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   State state = data[m_state];
   return state;
}

///////////////////////////////////////////////////////////////////////////////

Transform* BlendTreeNode::getGeneratedPose( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   Transform* generatedPose = data[m_generatedPose];
   return generatedPose;
}


///////////////////////////////////////////////////////////////////////////////

Transform& BlendTreeNode::getAccumulatedMotion( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   Transform& accumulatedMotion = data[m_accumulatedMotion];
   return accumulatedMotion;
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeNodeSyncProfile* BlendTreeNode::accessSyncData( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   BlendTreeNodeSyncProfile* syncData = data[m_nodeSyncData];

   // we need to reset the data, so that it can be filled with fresh info
   syncData->reset();

   return syncData;
}

///////////////////////////////////////////////////////////////////////////////

const BlendTreeNodeSyncProfile* BlendTreeNode::getSyncData( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   BlendTreeNodeSyncProfile* syncData = data[m_nodeSyncData];
   return syncData;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::setPlaybackSpeed( BlendTreePlayer* player, float speed ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_playbackSpeed] = speed;
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeNode::getPlaybackSpeed( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   float playbackSpeed = data[m_playbackSpeed];
   return playbackSpeed;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNode::pullRuntimeStructure( BlendTreePlayer* player, BlendTreePlayerListener* listener ) const
{
   listener->onNodeStateChanged( player, this );
}

///////////////////////////////////////////////////////////////////////////////
