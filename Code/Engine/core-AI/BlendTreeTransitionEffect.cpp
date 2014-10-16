#include "core-AI\BlendTreeTransitionEffect.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BlendTreeTransitionEffect );
   PARENT( ReflectionObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeTransitionEffect::BlendTreeTransitionEffect()
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeTransitionEffect::~BlendTreeTransitionEffect()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionEffect::setDuration( BlendTreePlayer* player, float duration ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_duration] = duration;
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeTransitionEffect::getProgressPercentage( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   float percent = data[m_progress]/data[m_duration];
   return percent;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionEffect::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_duration );
   data.registerVar( m_progress );
   data.registerVar( m_sourceNode );
   data.registerVar( m_targetNode );

   data[m_duration] = 0.0f;
   data[m_progress] = 0.0f;
   data[m_sourceNode] = NULL;
   data[m_targetNode] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionEffect::deinitializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.unregisterVar( m_duration );
   data.unregisterVar( m_progress );
   data.unregisterVar( m_sourceNode );
   data.unregisterVar( m_targetNode );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionEffect::activateEffect( BlendTreePlayer* player, const BlendTreeNode* sourceNode, const BlendTreeNode* targetNode ) const
{
   RuntimeDataBuffer& data = player->data();

   data[m_progress] = 0.0f;
   data[m_sourceNode] = sourceNode;
   data[m_targetNode] = targetNode;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionEffect::deactivateEffect( BlendTreePlayer* player ) const
{
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeTransitionEffect::update( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const
{
   RuntimeDataBuffer& data = player->data();
   const float duration = data[m_duration];
   float progress = data[m_progress];
   progress += timeDelta;
   data[m_progress] = progress;

   combinePoses( player, timeDelta, outGeneratedPose, outAccMotion, bonesCount );

   return progress >= duration; 
}

///////////////////////////////////////////////////////////////////////////////

