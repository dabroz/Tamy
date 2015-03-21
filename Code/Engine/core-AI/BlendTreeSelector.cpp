#include "core-AI\BlendTreeSelector.h"
#include "core-AI\BlendTreeTransitionEffect.h"
#include "core-AI\BlendTreeSelectorSwitch.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeSelector );
   PARENT( BlendTreeComposite );
   PROPERTY_EDIT( "Effect", BlendTreeTransitionEffect*, m_effect );
   PROPERTY_EDIT( "Switch", BlendTreeSelectorSwitch*, m_btSwitch );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeSelector::BlendTreeSelector( const char* name )
   : BlendTreeComposite( name )
   , m_effect( NULL )
   , m_btSwitch( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeSelector::~BlendTreeSelector()
{
   if ( m_effect )
   {
      m_effect->removeReference();
      m_effect = NULL;
   }

   if ( m_btSwitch )
   {
      m_btSwitch->removeReference();
      m_btSwitch = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::setEffect( BlendTreeTransitionEffect* effect )
{
   NOTIFY_PROPERTY_CHANGE( m_effect );

   if ( m_effect )
   {
      m_effect->removeReference();
   }

   m_effect = effect;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::setSwitch( BlendTreeSelectorSwitch* btSwitch )
{
   NOTIFY_PROPERTY_CHANGE( m_btSwitch );

   if ( m_btSwitch )
   {
      m_btSwitch->removeReference();
   }

   m_btSwitch = btSwitch;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeComposite::initializeLayout( player );

   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_activeNode );
   data.registerVar( m_transitionTargetNode );

   data[m_activeNode] = NULL;
   data[m_transitionTargetNode] = NULL;

   if ( m_btSwitch )
   {
      m_btSwitch->initializeLayout( player );
   }

   if ( m_effect )
   {
      m_effect->initializeLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::deinitializeLayout( BlendTreePlayer* player ) const
{
   if ( m_effect )
   {
      m_effect->deinitializeLayout( player );
   }

   if ( m_btSwitch )
   {
      m_btSwitch->deinitializeLayout( player );
   }

   RuntimeDataBuffer& data = player->data();
   data.unregisterVar( m_activeNode );
   data.unregisterVar( m_transitionTargetNode );

   BlendTreeComposite::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::onActivateNode( BlendTreePlayer* player ) const
{
   if ( !m_btSwitch )
   {
      // no switch - no fun :)
      return;
   }

   // activate the default state, if there are any
   int selectedNodeIdx = m_btSwitch->getNodeIdx( player );
   selectedNodeIdx = min2< int >( selectedNodeIdx, m_nodes.size() - 1 );
   ASSERT( selectedNodeIdx >= 0 );

   const BlendTreeNode* activeNode = m_nodes[selectedNodeIdx];
   activateChildNode( player, activeNode );

   RuntimeDataBuffer& data = player->data();
   data[m_activeNode] = activeNode;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::onDeactivateNode( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   const BlendTreeNode* activeNode = data[m_activeNode];
   const BlendTreeNode* transitionTargetNode = data[m_transitionTargetNode];

   if ( activeNode )
   {
      deactivateChildNode( player, activeNode );
      data[m_activeNode] = NULL;
   }

   if ( transitionTargetNode )
   {
      deactivateChildNode( player, transitionTargetNode );
      data[m_transitionTargetNode] = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::onUpdateLogic( BlendTreePlayer* player ) const
{
   // a priority is to finish an active transition
   RuntimeDataBuffer& data = player->data();
   const BlendTreeNode* transitionTargetNode = data[m_transitionTargetNode];
   const BlendTreeNode* activeNode = data[m_activeNode];

   // determine if there's a transition to be made
   if ( !transitionTargetNode )
   {
      // no transition is active yet, but maybe we need to take one
      int selectedNodeIdx = m_btSwitch->getNodeIdx( player );
      selectedNodeIdx = min2< int >( selectedNodeIdx, m_nodes.size() - 1 );
      ASSERT( selectedNodeIdx >= 0 );

      const BlendTreeNode* targetNode = m_nodes[selectedNodeIdx];
      if ( targetNode != activeNode )
      {
         // yes - a transition is in order
         if ( m_effect )
         {
            transitionTargetNode = targetNode;
            data[m_transitionTargetNode] = transitionTargetNode;

            activateChildNode( player, transitionTargetNode );
            m_effect->activateEffect( player, activeNode, targetNode );
         }
         else
         {
            // go straight to the other node
            deactivateChildNode( player, activeNode );
            activateChildNode( player, targetNode );
            data[m_activeNode] = targetNode;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
{
   RuntimeDataBuffer& data = player->data();
   const BlendTreeNode* transitionTargetNode = data[m_transitionTargetNode];
   const BlendTreeNode* activeNode = data[m_activeNode];

   const BlendTreeNodeSyncProfile* sourceNodeSyncData = activeNode->getSyncData( player );

   if ( transitionTargetNode )
   {  
      ASSERT( m_effect );

      // merge the progress of the target node
      const BlendTreeNodeSyncProfile* targetNodeSyncData = transitionTargetNode->getSyncData( player );

      float progress = m_effect->getProgressPercentage( player );
      outNodeSyncProfile.mergeWith( sourceNodeSyncData, 1.0f - progress );
      outNodeSyncProfile.mergeWith( targetNodeSyncData, progress );
   }
   else
   {
      outNodeSyncProfile.mergeWith( sourceNodeSyncData, 1.0f );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSelector::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const
{
   if ( !m_btSwitch )
   {
      // no switch - no fun :)
      return;
   }

   // a priority is to finish an active transition
   RuntimeDataBuffer& data = player->data();
   const BlendTreeNode* transitionTargetNode = data[m_transitionTargetNode];
   const BlendTreeNode* activeNode = data[m_activeNode];

   if ( transitionTargetNode )
   {
      // run a transition
      bool hasFinished = m_effect->update( player, timeDelta, outGeneratedPoseDiffLS, outAccMotion, bonesCount );
      if ( hasFinished )
      {
         deactivateChildNode( player, activeNode );
         m_effect->deactivateEffect( player );
         data[m_activeNode] = transitionTargetNode;
         data[m_transitionTargetNode] = NULL;
      }
   }
   else
   {
      // copy the pose
      Transform* poseGeneratedByChild = activeNode->getGeneratedPose( player );
      memcpy( outGeneratedPoseDiffLS, poseGeneratedByChild, sizeof( Transform ) * bonesCount );

      outAccMotion = activeNode->getAccumulatedMotion( player );
   }
}

///////////////////////////////////////////////////////////////////////////////
