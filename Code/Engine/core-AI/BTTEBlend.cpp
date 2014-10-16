#include "core-AI\BTTEBlend.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\PoseBlendingUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTTEBlend );
   PARENT( BlendTreeTransitionEffect );
   PROPERTY_EDIT( "Duration", float, m_duration );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTTEBlend::BTTEBlend( float duration )
   : m_duration( duration )
{
}

///////////////////////////////////////////////////////////////////////////////

BTTEBlend::~BTTEBlend()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTTEBlend::activateEffect( BlendTreePlayer* player, const BlendTreeNode* sourceNode, const BlendTreeNode* targetNode ) const
{
   BlendTreeTransitionEffect::activateEffect( player, sourceNode, targetNode );

   setDuration( player, m_duration );
}

///////////////////////////////////////////////////////////////////////////////

void BTTEBlend::combinePoses( BlendTreePlayer* player, float timeDelta, Transform* generatedPose, Transform& outAccMotion, uint bonesCount ) const
{
   RuntimeDataBuffer& data = player->data();
   const BlendTreeNode* sourceNode = data[m_sourceNode];
   const BlendTreeNode* targetNode = data[m_targetNode];

   FastFloat progressPercentage = FastFloat::fromFloat( getProgressPercentage( player ) );

   // blend the poses
   Transform* sourcePose = sourceNode->getGeneratedPose( player );
   Transform* targetPose = targetNode->getGeneratedPose( player );
   PoseBlendingUtils::blend( progressPercentage, sourcePose, targetPose, bonesCount, generatedPose );

   // blend the accumulated motion
   Transform& sourceAccMotion = sourceNode->getAccumulatedMotion( player );
   Transform& targetAccMotion = targetNode->getAccumulatedMotion( player );
   PoseBlendingUtils::blend( progressPercentage, &sourceAccMotion, &targetAccMotion, 1, &outAccMotion );
}

///////////////////////////////////////////////////////////////////////////////
