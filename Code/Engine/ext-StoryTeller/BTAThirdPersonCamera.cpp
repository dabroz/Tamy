#include "ext-StoryTeller\BTAThirdPersonCamera.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "core\MatrixUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAThirdPersonCamera );
   PARENT( BehTreeAction );
   PROPERTY_EDIT( "Camera pos. offset", Vector, m_cameraPosOffset );
   PROPERTY_EDIT( "Camera look-at offset", Vector, m_cameraLookAtOffset );
   PROPERTY_EDIT( "Arrival slowdown radius", float, m_slowdownRadius );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAThirdPersonCamera::BTAThirdPersonCamera()
   : m_slowdownRadius( 1.0f )
{
}

///////////////////////////////////////////////////////////////////////////////

BTAThirdPersonCamera::~BTAThirdPersonCamera()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAThirdPersonCamera::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_actorInstance );
   data.registerVar( m_cameraVelocity );
}

///////////////////////////////////////////////////////////////////////////////

void BTAThirdPersonCamera::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   // locate the actor's instance in the scene
   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   data[m_actorInstance] = context->m_ownerInstance;
   data[m_cameraVelocity].setZero();
}

///////////////////////////////////////////////////////////////////////////////

void BTAThirdPersonCamera::deinitialize( BehaviorTreeRunner& runner ) const
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAThirdPersonCamera::execute( BehaviorTreeRunner& runner ) const
{     
   RuntimeDataBuffer& data = runner.data();
   StoryNodeInstance* actorInstance = data[m_actorInstance];

   // put the camera directly over the player
   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   Camera& camera = context->m_player.renderer()->getActiveCamera();

   const Matrix& ownerMtx = actorInstance->getGlobalMtx();
   const Vector& actorPos = ownerMtx.position();
   
   Vector desiredCamPos;
   Vector desiredLookAtPos;
   {
      // plot a new position for the camera
      const Vector& actorFacing = ownerMtx.sideVec();
      Quaternion rotQ;
      rotQ.setFromShortestRotation( Vector_NEG_OX, actorFacing );

      Vector disp;
      rotQ.transform( m_cameraPosOffset, disp );
      desiredCamPos.setAdd( actorPos, disp );

      Vector rotatedLookAtOffset;
      rotQ.transform( m_cameraLookAtOffset, rotatedLookAtOffset );
      desiredLookAtPos.setAdd( actorPos, rotatedLookAtOffset );
   }


   // inertially slide the camera into the desired spot
   Vector newCamPos;
   Vector newLookAtPos;
   {
      newCamPos = desiredCamPos;
      newLookAtPos = desiredLookAtPos;
      /*
      const Matrix& camWorldMtx = camera.getGlobalMtx();

      Vector displacementToTargetPos;
      displacementToTargetPos.setSub( desiredCamPos, camWorldMtx.position() );

      const FastFloat distToTargetPos = displacementToTargetPos.length();
      const FastFloat slowdownRadius = FastFloat::fromFloat( m_slowdownRadius );
      Vector desiredVelocity;
      if ( distToTargetPos < slowdownRadius )
      {
         FastFloat slowdownFactor; 
         slowdownFactor.setDiv( distToTargetPos, slowdownRadius );
         desiredVelocity.setMul( displacementToTargetPos, slowdownFactor ); 
      }
      else
      {
         desiredVelocity = displacementToTargetPos;
      }

      newCamPos.setAdd( camWorldMtx.position(), desiredVelocity );
      newLookAtPos = desiredLookAtPos;
      */
   }

   Matrix newCamMtx;
   MatrixUtils::generateLookAtLH( newCamPos, newLookAtPos, Vector_OZ, newCamMtx );

   camera.setLocalMtx( newCamMtx );

   return IN_PROGRESS;
}

///////////////////////////////////////////////////////////////////////////////
