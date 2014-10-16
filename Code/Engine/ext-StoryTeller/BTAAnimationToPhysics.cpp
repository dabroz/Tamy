#include "ext-StoryTeller\BTAAnimationToPhysics.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-Physics\PhysicsCharacterController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-MVC\EntityUtils.h"
#include "core\TimeController.h"
#include "core\Math.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAAnimationToPhysics );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Rotation", BehTreeVarFloat*, m_characterRotation );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAAnimationToPhysics::BTAAnimationToPhysics()
   : m_characterRotation( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAAnimationToPhysics::~BTAAnimationToPhysics()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAAnimationToPhysics::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_characterController );
   data.registerVar( m_blendTreePlayer );
}

///////////////////////////////////////////////////////////////////////////////

void BTAAnimationToPhysics::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   data[m_characterController] = EntityUtils::getFirstChild< PhysicsCharacterController >( controlledNodeInstance );
   BlendTreePlayer* blendTreePlayer = EntityUtils::getFirstChild< BlendTreePlayer >( controlledNodeInstance );
   data[m_blendTreePlayer] = blendTreePlayer;
}

///////////////////////////////////////////////////////////////////////////////

void BTAAnimationToPhysics::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data[m_characterController] = NULL;
   data[m_blendTreePlayer] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAAnimationToPhysics::execute( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   PhysicsCharacterController* characterController = data[m_characterController];
   if ( !characterController )
   {
      LOG( "BTAAnimationToPhysics: The character doesn't have a PhysicsCharacterController component" );
      return FAILED;
   }

   BlendTreePlayer* blendTreePlayer = data[m_blendTreePlayer];
   if ( !blendTreePlayer )
   {
      LOG( "BTAAnimationToPhysics: The character doesn't have a BlendTreePlayer component" );
      return FAILED;
   }

   if ( !m_characterRotation )
   {
      LOG( "BTAAnimationToPhysics: Assign a character rotation variable" );
      return FAILED;
   }

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   // set the linear velocity
   {
      const Vector& translation = blendTreePlayer->m_accumulatedMotion.m_translation;
      TimeController& timeController = TSingleton< TimeController >::getInstance();

      Vector linearVelocityModelSpace;
      linearVelocityModelSpace.setMul( translation, FastFloat::fromFloat( 1.0f / timeController.getTimeElapsed() ) );

      const Matrix& characterWorldMtx = context->m_ownerInstance->getGlobalMtx();
      Quaternion modelToWorld;
      characterWorldMtx.getRotation( modelToWorld );

      Vector linearVelocityWorldSpace;
      modelToWorld.transform( linearVelocityModelSpace, linearVelocityWorldSpace );

      characterController->setLinearVelocity( linearVelocityWorldSpace );
   }

   // set the angular velocity
   {
      StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;
      Matrix nodeTransform = controlledNodeInstance->getLocalMtx();

      FastFloat dYaw; 
      dYaw.setFromFloat( m_characterRotation->getRuntime( &runner ) );

      Vector angularVelocity; 
      angularVelocity.set( Float_0, Float_0, dYaw );
      characterController->setAngularVelocity( angularVelocity );
   }


   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

