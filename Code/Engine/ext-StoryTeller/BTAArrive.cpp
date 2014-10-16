#include "ext-StoryTeller\BTAArrive.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActor.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-MVC\EntityUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAArrive );
   PARENT( BehTreeAction );
   PROPERTY_EDIT( "Arrival radius", float, m_arrivalRadius );
   PROPERTY_EDIT( "Top speed", float, m_topSpeed );
   PROPERTY_EDIT_REFERENCE( "Target pos", BehTreeVarVector*, m_targetPos );
   PROPERTY_EDIT_REFERENCE( "Calculated velocity", BehTreeVarVector*, m_outVelocity );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAArrive::BTAArrive()
   : m_arrivalRadius( 3.0f )
   , m_topSpeed( 10.0f )
   , m_targetPos( NULL )
   , m_outVelocity( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAArrive::~BTAArrive()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAArrive::execute( BehaviorTreeRunner& runner ) const
{
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   const Vector& targetPos = m_targetPos->getRuntime( &runner );
   const Vector& currPos = context->m_ownerInstance->getGlobalMtx().position();
   Vector requiredDisplacement;
   requiredDisplacement.setSub( targetPos, currPos );

   const FastFloat distance = requiredDisplacement.length();
   Vector movementDir; movementDir.setNormalized( requiredDisplacement );

  
   Vector velocity;
   const FastFloat topSpeed = FastFloat::fromFloat( m_topSpeed );
   const FastFloat arrivalRadius = FastFloat::fromFloat( m_arrivalRadius );
   if ( distance < arrivalRadius )
   {
      velocity.setMul( movementDir, topSpeed * ( distance / arrivalRadius ) );
   }
   else
   {
      velocity.setMul( movementDir, topSpeed );
   }

   m_outVelocity->setRuntime( &runner, velocity );

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////
