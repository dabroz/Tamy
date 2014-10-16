#include "ext-StoryTeller\BTATopDownCameraController.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "core\MatrixUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTATopDownCameraController );
   PARENT( BehTreeAction );
   PROPERTY_EDIT( "Hovering height", float, m_height );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTATopDownCameraController::BTATopDownCameraController()
   : m_height( 15 )
{
}

///////////////////////////////////////////////////////////////////////////////

BTATopDownCameraController::~BTATopDownCameraController()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTATopDownCameraController::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_actorInstance );
}

///////////////////////////////////////////////////////////////////////////////

void BTATopDownCameraController::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   // locate the actor's instance in the scene
   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   data[m_actorInstance] = context->m_ownerInstance;
}

///////////////////////////////////////////////////////////////////////////////

void BTATopDownCameraController::deinitialize( BehaviorTreeRunner& runner ) const
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTATopDownCameraController::execute( BehaviorTreeRunner& runner ) const
{     
   RuntimeDataBuffer& data = runner.data();
   StoryNodeInstance* actorInstance = data[m_actorInstance];

   // put the camera directly over the player
   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   Camera& camera = context->m_player.renderer()->getActiveCamera();

   const Vector& ownerPos = actorInstance->getGlobalMtx().position();

   Vector offset( 0, 0, m_height );
   Vector cameraPos;
   cameraPos.setAdd( ownerPos, offset );

   Matrix lookAtMtx;
   MatrixUtils::generateLookAtLH( cameraPos, ownerPos, Quad_0100, lookAtMtx );

   camera.setLocalMtx( lookAtMtx );

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////
