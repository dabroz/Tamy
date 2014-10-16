#include "ext-StoryTeller\BTAMoveTo.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core\TimeController.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAMoveTo );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "World pos", BehTreeVarVector*, m_worldPos );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAMoveTo::BTAMoveTo()
   : m_worldPos( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAMoveTo::~BTAMoveTo()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveTo::createLayout( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveTo::initialize( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveTo::deinitialize( BehaviorTreeRunner& runner ) const
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAMoveTo::execute( BehaviorTreeRunner& runner ) const
{     
   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   Vector selectedWorldPos = m_worldPos->getRuntime( &runner );
   Matrix nodeTransform = controlledNodeInstance->getLocalMtx();
   const Vector& currPos = nodeTransform.position();

   Vector displacementToTarget;
   displacementToTarget.setSub( selectedWorldPos, currPos );

   FastFloat distToTarget = displacementToTarget.length();
   if ( distToTarget < Float_1e_1 )
   {
      return BehTreeNode::FINISHED;
   }
   

   Vector dirToTarget;
   dirToTarget.setNormalized( displacementToTarget );

   TimeController& timeController = TSingleton< TimeController >::getInstance();
   FastFloat timeElapsed;
   timeElapsed.setFromFloat( timeController.getTimeElapsed() );
      
   FastFloat maxDistanceCoveredPerSec;
   maxDistanceCoveredPerSec.setClamped( distToTarget, Float_0, Float_5 );

   FastFloat distanceCoveredThisFrame;
   distanceCoveredThisFrame.setMul( maxDistanceCoveredPerSec, timeElapsed );

   Vector displacement;
   displacement.setMul( dirToTarget, distanceCoveredThisFrame );

   Vector newPos;
   newPos.setAdd( nodeTransform.position(), displacement );
   nodeTransform.setPosition<3>( newPos );

   controlledNodeInstance->setLocalMtx( nodeTransform );
   return BehTreeNode::IN_PROGRESS;
}

///////////////////////////////////////////////////////////////////////////////

