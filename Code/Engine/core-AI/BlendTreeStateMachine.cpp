#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeStateTransition.h"
#include "core-AI\BlendTreeTransitionEffect.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\Skeleton.h"
#include "core-AI\BlendTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeStateMachine );
   PARENT( BlendTreeComposite );
   PROPERTY( Array< BlendTreeStateTransition* >, m_transitions );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachine::BlendTreeStateMachine( const char* name )
   : BlendTreeComposite( name )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachine::~BlendTreeStateMachine()
{
   uint count = m_transitions.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeStateTransition* transition = m_transitions[i];
      transition->removeReference();
   }
   m_transitions.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::pullStructure( BlendTreeListener* listener )
{
   BlendTreeComposite::pullStructure( listener );

   uint count = m_transitions.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeStateTransition* transition = m_transitions[i];
      listener->onTransitionAdded( this, transition );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeComposite::initializeLayout( player );

   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_activeState );
   data.registerVar( m_activeTransition );

   data[m_activeState] = m_nodes.empty() ? NULL : m_nodes[0];
   data[m_activeTransition] = NULL;

   // pass the call down to the transitions
   uint count = m_transitions.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeStateTransition* transition = m_transitions[i];
      if ( transition )
      {
         transition->initializeLayout( player );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::deinitializeLayout( BlendTreePlayer* player ) const
{
   // pass the call down to the transitions
   uint count = m_transitions.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeStateTransition* transition = m_transitions[i];
      if ( transition )
      {
         transition->deinitializeLayout( player );
      }
   }

   RuntimeDataBuffer& data = player->data();
   data.unregisterVar( m_activeState );
   data.unregisterVar( m_activeTransition );

   BlendTreeComposite::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::addTransition( BlendTreeStateTransition* transition )
{
   if ( transition )
   {
      m_transitions.push_back( transition );

      // notify listeners
      for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreeListener* listener = *it;
         listener->onTransitionAdded( this, transition );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::removeTransition( BlendTreeStateTransition* transition )
{
   if ( !transition )
   {
      return;
   }

   uint count = m_transitions.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_transitions[i] == transition )
      {
         // notify listeners
         for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeListener* listener = *it;
            listener->onTransitionRemoved( this, transition );
         }

         // remove the transition
         transition->removeReference();
         m_transitions.remove( i );

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onNodeRemoved( BlendTreeNode* node, uint idx )
{
   // remove all transitions this node was involved in
   uint count = m_transitions.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      BlendTreeStateTransition* transition = m_transitions[i];
      if ( transition->m_startState == node || transition->m_endState == node )
      {
         // notify listeners
         for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
         {
            BlendTreeListener* listener = *it;
            listener->onTransitionRemoved( this, transition );
         }

         // remove the transition
         transition->removeReference();
         m_transitions.remove( i );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onActivateNode( BlendTreePlayer* player ) const
{
   // activate the default state, if there are any
   if ( !m_nodes.empty() )
   {
      const BlendTreeNode* activeState = m_nodes[0];
      activateChildNode( player, activeState );

      RuntimeDataBuffer& data = player->data();
      data[m_activeState] = activeState;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onDeactivateNode( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   const BlendTreeStateTransition* activeTransition = data[m_activeTransition];
   const BlendTreeNode* activeState = data[m_activeState];

   if ( activeTransition != NULL )
   {
      // deactivate both states involved in the transition
      ASSERT( activeState == activeTransition->m_startState );

      activeTransition->interrupt( player, this );
   }
   else if ( activeState )
   {
      // deactivate the active state
      deactivateChildNode( player, activeState );
   }

   // cleanup temporary data
   data[m_activeState] = NULL;
   data[m_activeTransition] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onUpdateLogic( BlendTreePlayer* player ) const
{
   // a priority is to finish an active transition
   RuntimeDataBuffer& data = player->data();
   const BlendTreeStateTransition* activeTransition = data[m_activeTransition];
   const BlendTreeNode* activeState = data[m_activeState];

   // check if there's a transition to process
   if ( !activeTransition )
   {

      // test all transitions to see if one would be interested in being activated
      uint count = m_transitions.size();
      for ( uint i = 0; i < count; ++i )
      {
         BlendTreeStateTransition* transition = m_transitions[i];
         if ( transition->m_startState != activeState )
         {
            continue;
         }

         bool toBeActivated = transition->testActivation( player );
         if ( toBeActivated )
         {
            // activate the end state
            transition->activate( player, this );

            // memorize active transition
            activeTransition = transition;
            data[m_activeTransition] = activeTransition;
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
{
   RuntimeDataBuffer& data = player->data();

   const BlendTreeStateTransition* activeTransition = data[m_activeTransition];
   if ( activeTransition )
   {
      // this code was previously a part of BlendTreeStateTransition class, but I inlined it
      // for readability - it's better this way, since we're setting the playback speeds afterwards,
      // and it's good to see that both paths are actually doing the same
      const BlendTreeNodeSyncProfile* startStateSyncData = activeTransition->m_startState->getSyncData( player );
      const BlendTreeNodeSyncProfile* endStateSyncData = activeTransition->m_endState->getSyncData( player );

      float progress = 1.0f;
      if ( activeTransition->m_effect )
      {
         progress = activeTransition->m_effect->getProgressPercentage( player );
      }

      outNodeSyncProfile.mergeWith( startStateSyncData, 1.0f - progress );
      outNodeSyncProfile.mergeWith( endStateSyncData, progress );
   }
   else
   {
      const BlendTreeNode* activeState = data[m_activeState];
      if ( activeState )
      {
         // store the progress in the node's runtime synchronization profile
         const BlendTreeNodeSyncProfile* childSyncData = activeState->getSyncData( player );
         outNodeSyncProfile.mergeWith( childSyncData, 1.0f );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachine::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const
{
   // a priority is to finish an active transition
   RuntimeDataBuffer& data = player->data();
   const BlendTreeStateTransition* activeTransition = data[m_activeTransition];
   if ( activeTransition )
   {
      // run the active transition
      bool hasFinished = activeTransition->update( player, timeDelta );

      // copy the generated pose
      Transform* generatedPose = activeTransition->getGeneratedPose( player );
      memcpy( outGeneratedPoseDiffLS, generatedPose, sizeof( Transform ) * bonesCount );

      outAccMotion = activeTransition->getAccumulatedMotion( player );

      // check if the transition has finished
      if ( hasFinished )
      {
         // deactivate the start state
         activeTransition->deactivate( player, this );

         // cleanup
         data[m_activeState] = activeTransition->m_endState;
         data[m_activeTransition] = NULL;
      }
   }
   else
   {
      // there's no active transition - sample the active state
      const BlendTreeNode* activeState = data[m_activeState];
      if ( activeState )
      {
         // copy the generated pose
         Transform* generatedPose = activeState->getGeneratedPose( player );
         memcpy( outGeneratedPoseDiffLS, generatedPose, sizeof( Transform ) * bonesCount );

         outAccMotion = activeState->getAccumulatedMotion( player );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
