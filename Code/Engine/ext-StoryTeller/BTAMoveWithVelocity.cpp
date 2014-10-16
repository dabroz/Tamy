#include "ext-StoryTeller\BTAMoveWithVelocity.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-Physics\PhysicsCharacterController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core-MVC\EntityUtils.h"
#include "core\TimeController.h"
#include "core\Math.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAMoveWithVelocity );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Velocity", BehTreeVarVector*, m_velocity );
   PROPERTY_EDIT_REFERENCE( "Rotation", BehTreeVarFloat*, m_characterRotation );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAMoveWithVelocity::BTAMoveWithVelocity()
   : m_velocity( NULL )
   , m_characterRotation( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAMoveWithVelocity::~BTAMoveWithVelocity()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveWithVelocity::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_characterController );
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveWithVelocity::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data( );

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext( );
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   data[m_characterController] = EntityUtils::getFirstChild< PhysicsCharacterController >( controlledNodeInstance );
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveWithVelocity::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data( );
   data[m_characterController] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAMoveWithVelocity::execute( BehaviorTreeRunner& runner ) const
{   
   RuntimeDataBuffer& data = runner.data( );
   PhysicsCharacterController* characterController = data[m_characterController];

   if ( !m_velocity  )
   {
      LOG( "BTAMoveWithVelocity: Assign a velocity variable" );
      return FAILED;
   }

   if ( !m_characterRotation )
   {
      LOG( "BTAMoveWithVelocity: Assign a character rotation variable" );
      return FAILED;
   }

   if ( !characterController )
   {
      LOG( "BTAMoveWithVelocity: The character doesn't have a PhysicsCharacterController component" );
      return FAILED;
   }

   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   // calculate displacement
   TimeController& timeController = TSingleton< TimeController >::getInstance();

   Vector movementVelocity = m_velocity->getRuntime( &runner );
   characterController->setLinearVelocity( movementVelocity );


   // calculate new facing direction and a corresponding orientation
   {
      Matrix nodeTransform = controlledNodeInstance->getLocalMtx( );

      FastFloat dYaw; dYaw.setFromFloat( m_characterRotation->getRuntime( &runner ) );

      Vector angularVelocity; angularVelocity.set( Float_0, Float_0, dYaw );

      characterController->setAngularVelocity( angularVelocity );
   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

