#include "ext-StoryTeller\BTAPursue.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActor.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-MVC\EntityUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAPursue );
PARENT( BehTreeAction );
   PROPERTY_EDIT( "Arrival radius", float, m_arrivalRadius );
   PROPERTY_EDIT( "Top speed", float, m_topSpeed );
   PROPERTY_EDIT_REFERENCE( "Target pos", BehTreeVarVector*, m_targetPos );
   PROPERTY_EDIT_REFERENCE( "Target velocity", BehTreeVarVector*, m_targetVelocity );
   PROPERTY_EDIT_REFERENCE( "Calculated velocity", BehTreeVarVector*, m_outVelocity );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAPursue::BTAPursue()
   : m_arrivalRadius( 3.0f )
   , m_topSpeed( 10.0f )
   , m_targetPos( NULL )
   , m_targetVelocity( NULL )
   , m_outVelocity( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAPursue::~BTAPursue()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAPursue::execute( BehaviorTreeRunner& runner ) const
{
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   // common data used for calculation
   const FastFloat topSpeed = FastFloat::fromFloat( m_topSpeed );
   const Vector& currPos = context->m_ownerInstance->getGlobalMtx().position();

   // predict the target's position
   Vector predictedTargetPos;
   {
      const Vector& targetVel = m_targetVelocity->getRuntime( &runner );
      const FastFloat targetSpeed = targetVel.length();
      const Vector& targetPos = m_targetPos->getRuntime( &runner );

      if ( targetSpeed > Float_1e_3 )
      {
         Vector displacementToTarget;
         displacementToTarget.setSub( targetPos, currPos );

         const FastFloat distanceToTarget = displacementToTarget.length();
         const FastFloat timeToTarget = distanceToTarget / topSpeed;
         const FastFloat predictedDisplacement = targetSpeed * timeToTarget;

         Vector targetMovementDir; targetMovementDir.setNormalized( targetVel );
         predictedTargetPos.setMulAdd( targetMovementDir, predictedDisplacement, targetPos );
      }
      else
      {
         predictedTargetPos = targetPos;
      }
   }
   
   // calculate the arrival steering velocity
   {
      Vector displacementToTarget;
      displacementToTarget.setSub( predictedTargetPos, currPos );

      const FastFloat distanceToTarget = displacementToTarget.length();

      Vector movementDir; movementDir.setNormalized( displacementToTarget );

      Vector velocity;
      const FastFloat topSpeed = FastFloat::fromFloat( m_topSpeed );
      const FastFloat arrivalRadius = FastFloat::fromFloat( m_arrivalRadius );
      if ( distanceToTarget < arrivalRadius )
      {
         velocity.setMul( movementDir, topSpeed * ( distanceToTarget / arrivalRadius ) );
      }
      else
      {
         velocity.setMul( movementDir, topSpeed );
      }

      m_outVelocity->setRuntime( &runner, velocity );
   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////
