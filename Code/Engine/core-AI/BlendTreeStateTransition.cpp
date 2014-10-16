#include "core-AI\BlendTreeStateTransition.h"
#include "core-AI\BlendTreeTransitionEffect.h"
#include "core-AI\BlendTreeTransitionTrigger.h"
#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\Skeleton.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BlendTreeStateTransition );
   PARENT( ReflectionObject );
   PROPERTY_EDIT( "name", std::string, m_name );
   PROPERTY_EDIT( "Effect", BlendTreeTransitionEffect*, m_effect );
   PROPERTY_EDIT( "Transition trigger", BlendTreeTransitionTrigger*, m_transitionTrigger );
   PROPERTY( BlendTreeNode*, m_startState );
   PROPERTY( BlendTreeNode*, m_endState );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateTransition::BlendTreeStateTransition( const char* name )
   : m_name( name )
   , m_effect( NULL )
   , m_transitionTrigger( NULL )
   , m_startState( NULL )
   , m_endState( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateTransition::~BlendTreeStateTransition()
{
   if ( m_transitionTrigger )
   {
      m_transitionTrigger->removeReference();
      m_transitionTrigger = NULL;
   }

   if ( m_effect )
   {
      m_effect->removeReference();
      m_effect = NULL;
   }

   m_startState = NULL;
   m_endState = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::setConnection( BlendTreeNode* startState, BlendTreeNode* endState )
{
   m_startState = startState;
   m_endState = endState;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::setEffect( BlendTreeTransitionEffect* effect )
{
   NOTIFY_PROPERTY_CHANGE( m_effect );
   if ( m_effect )
   {
      m_effect->removeReference(); 
   }

   m_effect = effect;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::setTransitionTrigger( BlendTreeTransitionTrigger* transitionTrigger )
{
   NOTIFY_PROPERTY_CHANGE( m_transitionTrigger );
   if ( m_transitionTrigger )
   {
      m_transitionTrigger->removeReference(); 
   }

   m_transitionTrigger = transitionTrigger;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::initializeLayout( BlendTreePlayer* player ) const
{
   // initialize the pose storage
   uint bonesCount = player->getBoneCount();
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_generatedPose );
   data.registerVar( m_accumulatedMotion );

   data[m_generatedPose] = new Transform[bonesCount];

   // initialize the children
   if ( m_effect )
   {
      m_effect->initializeLayout( player );
   }

   if ( m_transitionTrigger )
   {
      m_transitionTrigger->initializeLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::deinitializeLayout( BlendTreePlayer* player ) const
{
   // deinitialize the children
   if ( m_transitionTrigger )
   {
      m_transitionTrigger->deinitializeLayout( player );
   }

   if ( m_effect )
   {
      m_effect->deinitializeLayout( player );
   }

   // deinitialize the pose storage
   RuntimeDataBuffer& data = player->data();
   Transform* generatedPose = data[m_generatedPose];
   delete [] generatedPose;
   data.unregisterVar( m_generatedPose );
   data.unregisterVar( m_accumulatedMotion );
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeStateTransition::testActivation( BlendTreePlayer* player ) const
{
   bool result = false;  // no trigger defined - activation denied

   if ( m_transitionTrigger )
   {
      result = m_transitionTrigger->testActivation( player );
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::activate( BlendTreePlayer* player, const BlendTreeStateMachine* hostMachine ) const
{
   ASSERT( m_endState );
   hostMachine->activateChildNode( player, m_endState );

   if ( m_effect )
   {
      m_effect->activateEffect( player, m_startState, m_endState );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::deactivate( BlendTreePlayer* player, const BlendTreeStateMachine* hostMachine ) const
{
   ASSERT( m_startState );
   hostMachine->deactivateChildNode( player, m_startState );

   if ( m_effect )
   {
      m_effect->deactivateEffect( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateTransition::interrupt( BlendTreePlayer* player, const BlendTreeStateMachine* hostMachine ) const
{
   ASSERT( m_startState );
   ASSERT( m_endState );
   hostMachine->deactivateChildNode( player, m_startState );
   hostMachine->deactivateChildNode( player, m_endState );

   if ( m_effect )
   {
      m_effect->deactivateEffect( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeStateTransition::update( BlendTreePlayer* player, float timeDelta ) const
{
   uint bonesCount = player->getBoneCount();

   RuntimeDataBuffer& data = player->data();
   Transform* outGeneratedPose = data[m_generatedPose];
   Transform& accumulatedMotion = data[m_accumulatedMotion];

   if ( m_effect )
   {
      // sample the transition
      bool isTransitionOver = m_effect->update( player, timeDelta, outGeneratedPose, accumulatedMotion, bonesCount );
      return isTransitionOver;
   }
   else
   {
      // there's no effect, so immediately transition to the next state and sample its pose
      Transform* generatedPose = m_endState->getGeneratedPose( player );
      memcpy( outGeneratedPose, generatedPose, sizeof( Transform ) * bonesCount );

      accumulatedMotion = m_endState->getAccumulatedMotion( player );

      return true;
   }
}

///////////////////////////////////////////////////////////////////////////////

Transform* BlendTreeStateTransition::getGeneratedPose( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   Transform* generatedPose = data[m_generatedPose];
   return generatedPose;
}

///////////////////////////////////////////////////////////////////////////////

Transform& BlendTreeStateTransition::getAccumulatedMotion( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   Transform& accMotion = data[m_accumulatedMotion];
   return accMotion;
}

///////////////////////////////////////////////////////////////////////////////
