#include "ext-StoryTeller\BTASlideTo.h"
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

BEGIN_OBJECT( BTASlideTo );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Destination", BehTreeVarVector*, m_destination );
   PROPERTY_EDIT( "Duration", float, m_slideDuration );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTASlideTo::BTASlideTo()
   : m_destination( NULL )
   , m_slideDuration( 1.0f )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTASlideTo::~BTASlideTo()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTASlideTo::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_slideVec );
   data.registerVar( m_remainingDuration );
}

///////////////////////////////////////////////////////////////////////////////

void BTASlideTo::initialize( BehaviorTreeRunner& runner ) const
{
   if ( !m_destination )
   {
      LOG( "BTASlideRepetively: Assign a destination variable" );
      return;
   }

   RuntimeDataBuffer& data = runner.data();
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;
   data[m_remainingDuration] = max2( m_slideDuration, 0.1f );

   // calculate displacement
   const Vector& currPos = controlledNodeInstance->getGlobalMtx().position();
   const Vector& destination = m_destination->getRuntime( &runner );

   Vector slideVec;
   slideVec.setSub( destination, currPos );
   data[m_slideVec] = slideVec;
}

///////////////////////////////////////////////////////////////////////////////

void BTASlideTo::deinitialize( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTASlideTo::execute( BehaviorTreeRunner& runner ) const
{
   if ( !m_destination )
   {
      return FAILED;
   }

   RuntimeDataBuffer& data = runner.data();
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   PhysicsCharacterController* characterController = EntityUtils::getFirstChild< PhysicsCharacterController >( controlledNodeInstance );
 
   if ( !characterController )
   {
      LOG( "BTAMoveWithVelocity: The character doesn't have a PhysicsCharacterController component" );
      return FAILED;
   }

   // get the time delta
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   const float timeDelta = min2( timeController.getTimeElapsed(), data[m_remainingDuration] );
   data[m_remainingDuration] -= timeDelta;

   const float slideDuration = max2( m_slideDuration, 0.1f );
   const FastFloat slideDistanceFactor = FastFloat::fromFloat( 1.f / m_slideDuration );

   Vector slideOffset;
   slideOffset.setMul( data[m_slideVec], slideDistanceFactor );

   characterController->setLinearVelocity( slideOffset );

   // the action never ends
   return data[m_remainingDuration] > 0.f ? IN_PROGRESS : FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

