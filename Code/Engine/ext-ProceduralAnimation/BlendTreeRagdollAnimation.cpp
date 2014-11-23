#include "ext-ProceduralAnimation\BlendTreeRagdollAnimation.h"
#include "ext-ProceduralAnimation\RagdollComponent.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonComponent.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeRagdollAnimation );
   PARENT( BlendTreeNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeRagdollAnimation::BlendTreeRagdollAnimation( const char* name )
   : BlendTreeNode( name )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeRagdollAnimation::~BlendTreeRagdollAnimation()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeNode::initializeLayout( player );

   RuntimeDataBuffer& data = player->data();

   // create a helper that will manage the RagdollCopmponent associated with the character
   {
      data.registerVar( m_ragdollComponentListener );

      RagdollComponentListener* ragdollListener = new RagdollComponentListener( player->m_skeleton );
      data[m_ragdollComponentListener] = ragdollListener;

      Entity* parentEntity = player->getParent();
      parentEntity->attachEntityListener( ragdollListener );
      parentEntity->pullStructure( ragdollListener );
   }

}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::deinitializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();

   // destroy the RagdollComponent manager
   {
      RagdollComponentListener* ragdollListener = data[m_ragdollComponentListener];

      Entity* parentEntity = player->getParent();
      parentEntity->detachEntityListener( ragdollListener );
      delete ragdollListener;

      data.unregisterVar( m_ragdollComponentListener );
   }

   BlendTreeNode::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const
{
   RuntimeDataBuffer& data = player->data();
   RagdollComponentListener* ragdollListener = data[m_ragdollComponentListener];
   RagdollComponent* ragdollComp = ragdollListener->m_ragdollComponent;
   const SkeletonMapper* mapper = ragdollListener->m_mapper;

   if ( !ragdollComp || !mapper )
   {
      return;
   }

   Array< Transform >& bodyTransforms = ragdollListener->m_bodyTransformsLocalSpace;
   ragdollComp->calcPoseLocalSpace( bodyTransforms );

   mapper->setPoseLocalSpace( bodyTransforms.getRaw(), outGeneratedPose );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendTreeRagdollAnimation::RagdollComponentListener::RagdollComponentListener( Skeleton* animationSkeleton )
   : m_animationSkeleton( animationSkeleton )
   , m_ragdollComponent( NULL )
   , m_mapper( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::RagdollComponentListener::onChildAttached( Entity* parentEntity, SceneNode* child )
{
   // use the ragdoll component, unless we're already using another one ( the user might have attached multiple components )
   if ( child->isExactlyA< RagdollComponent >() && m_ragdollComponent == NULL )
   {
      m_ragdollComponent = static_cast< RagdollComponent* >( child );

      const Skeleton* ragdollSkeleton = m_ragdollComponent->getSkeleton();
      if ( ragdollSkeleton )
      {
         const uint bodiesCount = ragdollSkeleton->getBoneCount();
         m_bodyTransformsLocalSpace.resize( bodiesCount, Transform::IDENTITY );
      }

      m_mapper = m_ragdollComponent->getSkeletonMapper( m_animationSkeleton );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::RagdollComponentListener::onChildDetached( Entity* parentEntity, SceneNode* child )
{
   if ( child->isExactlyA< RagdollComponent >() && m_ragdollComponent == child )
   {
      m_ragdollComponent = NULL;
      m_mapper = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
