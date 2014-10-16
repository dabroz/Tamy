#include "ext-StoryTeller\BTAPhysicsToAnimation.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-Physics\PhysicsCharacterController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core-MVC\EntityUtils.h"
#include "core\TimeController.h"
#include "core\Math.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAPhysicsToAnimation );
   PARENT( BehTreeAction );
   PROPERTY_EDIT( "Linear velocity var", std::string, m_linearSpeedVarName );
   PROPERTY_EDIT( "Direction var", std::string, m_directionVarName );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAPhysicsToAnimation::BTAPhysicsToAnimation()
   : m_linearSpeedVarName( "linearVelocity" )
   , m_directionVarName( "direction" )
{
}

///////////////////////////////////////////////////////////////////////////////

BTAPhysicsToAnimation::~BTAPhysicsToAnimation()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAPhysicsToAnimation::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_characterController );
   data.registerVar( m_blendTreePlayer );
   data.registerVar( m_linearSpeedVar );
   data.registerVar( m_directionVar );
}

///////////////////////////////////////////////////////////////////////////////

void BTAPhysicsToAnimation::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   data[m_characterController] = EntityUtils::getFirstChild< PhysicsCharacterController >( controlledNodeInstance );
   BlendTreePlayer* blendTreePlayer = EntityUtils::getFirstChild< BlendTreePlayer >( controlledNodeInstance );
   data[m_blendTreePlayer] = blendTreePlayer;

   data[m_linearSpeedVar] = NULL;
   data[m_directionVar] = NULL;
   if ( blendTreePlayer )
   {
      BlendTree* blendTree = blendTreePlayer->getBlendTree();
      if ( blendTree )
      {
         const BTVarFloat* var = blendTree->getVariable< BTVarFloat >( m_linearSpeedVarName );
         data[m_linearSpeedVar] = var;
         if ( !var )
         {
            LOG( "BTAMoveWithVelocity: The character doesn't have a BlendTreePlayer component or the tree has no '%s' variable defined", m_linearSpeedVarName.c_str() );
         }

         var = blendTree->getVariable< BTVarFloat >( m_directionVarName );
         data[m_directionVar] = var;
         if ( !var )
         {
            LOG( "BTAMoveWithVelocity: The character doesn't have a BlendTreePlayer component or the tree has no '%s' variable defined", m_directionVarName.c_str() );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTAPhysicsToAnimation::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data[m_characterController] = NULL;
   data[m_blendTreePlayer] = NULL;
   data[m_linearSpeedVar] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAPhysicsToAnimation::execute( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   PhysicsCharacterController* characterController = data[m_characterController];
   if ( !characterController )
   {
      LOG( "BTAMoveWithVelocity: The character doesn't have a PhysicsCharacterController component" );
      return FAILED;
   }

   BlendTreePlayer* blendTreePlayer = data[m_blendTreePlayer];
   const BTVarFloat* linearSpeedVar = data[m_linearSpeedVar];

   const Vector& desiredLinearVelocity = characterController->getLinearVelocity();
   const float desiredLinearSpeed = desiredLinearVelocity.length().getFloat();

   if ( linearSpeedVar )
   {
      linearSpeedVar->setRuntime( blendTreePlayer, desiredLinearSpeed );
   }

   const BTVarFloat* directionVar = data[m_directionVar];
   if ( directionVar )
   {
      float movementDir = 0.f;
      if ( desiredLinearSpeed > 0.f )
      {
         Vector characterMovementDir;
         characterMovementDir.setNormalized( desiredLinearVelocity );

         const Vector& characterSideVec = characterController->getGlobalMtx().upVec();
         const Vector& characterFwdVec = characterController->getGlobalMtx().sideVec();

         FastFloat angle = characterFwdVec.dot( characterMovementDir );
         FastFloat side = characterSideVec.dot( characterMovementDir ) * Float_Minus1;

         angle.setAcos( angle );
         angle.setFlipSign( angle, side );

         movementDir = RAD2DEG( angle.getFloat() );
      }

      directionVar->setRuntime( blendTreePlayer, movementDir );

   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

