#include "ext-StoryTeller\BTAGetActorInfo.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActor.h"
#include "core-Physics\PhysicsCharacterController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-MVC\EntityUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAGetActorInfo );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Actor", BehTreeVarReflectionObject*, m_actor );
   PROPERTY_EDIT_REFERENCE( "World pos", BehTreeVarVector*, m_worldPos );
   PROPERTY_EDIT_REFERENCE( "Linear velocity", BehTreeVarVector*, m_velocity );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAGetActorInfo::BTAGetActorInfo()
   : m_actor( NULL )
   , m_worldPos( NULL )
   , m_velocity( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAGetActorInfo::~BTAGetActorInfo()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAGetActorInfo::execute( BehaviorTreeRunner& runner ) const
{
   if ( !m_actor )
   {
      return FAILED;
   }
   
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   const StoryNodeInstance* actorInstance = static_cast< const StoryNodeInstance* >( m_actor->getRuntime( &runner ) );
   if ( !actorInstance )
   {
      return FAILED;
   }

   if ( m_worldPos )
   {
      m_worldPos->setRuntime( &runner, actorInstance->getGlobalMtx().position() );
   }

   if ( m_velocity )
   {
      const PhysicsCharacterController* characterController = EntityUtils::getFirstChild< PhysicsCharacterController >( actorInstance );
      if ( characterController )
      {
         m_velocity->setRuntime( &runner, characterController->getLinearVelocity() );
      }
   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////
