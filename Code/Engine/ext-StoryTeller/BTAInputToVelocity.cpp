#include "ext-StoryTeller\BTAInputToVelocity.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AppFlow\UserInputController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core\Vector.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAInputToVelocity );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Velocity", BehTreeVarVector*, m_velocity );
   PROPERTY_EDIT_REFERENCE( "Rotation", BehTreeVarFloat*, m_rotation );
   PROPERTY_EDIT_REFERENCE( "Linear speed", BehTreeVarFloat*, m_velocityScale );
   PROPERTY_EDIT_REFERENCE( "Angular speed", BehTreeVarFloat*, m_angularSpeed );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAInputToVelocity::BTAInputToVelocity()
   : m_velocity( NULL )
   , m_rotation( NULL )
   , m_velocityScale( NULL )
   , m_angularSpeed( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAInputToVelocity::~BTAInputToVelocity()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAInputToVelocity::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_yaw );
}

///////////////////////////////////////////////////////////////////////////////

void BTAInputToVelocity::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data[m_yaw] = 0;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAInputToVelocity::execute( BehaviorTreeRunner& runner ) const
{     
   if ( !m_velocity || !m_rotation )
   {
      return FAILED;
   }

   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   UserInputController* inputCtrl = context->m_player.userInputController();

   // initialize velocity
   {
      Vector velocity;
      if ( inputCtrl->isKeyPressed( 'W' ) )
      {
         velocity.add( Vector_OX );
      }
      if ( inputCtrl->isKeyPressed( 'S' ) )
      {
         velocity.sub( Vector_OX );
      }
      if ( inputCtrl->isKeyPressed( 'A' ) )
      {
         velocity.sub( Vector_OY );
      }
      if ( inputCtrl->isKeyPressed( 'D' ) )
      {
         velocity.add( Vector_OY );
      }

      if ( m_velocityScale )
      {
         FastFloat speed;
         speed.setFromFloat( m_velocityScale->getRuntime( &runner ) );

         velocity.mul( speed );
      }

      m_velocity->setRuntime( &runner, velocity );
   }

   // initialize facing direction
   {
      float horizMouseSpeedMultiplier = context->m_player.renderer()->getViewportWidth();

      Vector rotVelocity;
      rotVelocity.load<2>( inputCtrl->getMouseSpeed() );
      const FastFloat rotationSpeed = rotVelocity.length();
      FastFloat rotationDir;
      rotationDir.setSign( rotVelocity.getComponent( 0 ) );

      FastFloat yaw = rotationDir * rotationSpeed;

      const float mouseSensitivity = 8;
      float dYaw = DEG2RAD( yaw.getFloat() * mouseSensitivity / horizMouseSpeedMultiplier );
      if ( m_angularSpeed )
      {
         const float speed = m_angularSpeed->getRuntime( &runner );
         dYaw *= DEG2RAD( speed );
      }

      m_rotation->setRuntime( &runner, dYaw );
   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////