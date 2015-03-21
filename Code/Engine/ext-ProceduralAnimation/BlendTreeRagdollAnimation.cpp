#include "ext-ProceduralAnimation\BlendTreeRagdollAnimation.h"
#include "ext-ProceduralAnimation\RagdollComponent.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperRuntime.h"
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

void BlendTreeRagdollAnimation::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const
{
   RuntimeDataBuffer& data = player->data();
   RagdollComponentListener* ragdollListener = data[m_ragdollComponentListener];
   RagdollComponent* ragdollComp = ragdollListener->m_ragdollComponent;
   const SkeletonMapperRuntime* mapperRuntime = ragdollListener->m_mapperRuntime;

   if ( !ragdollComp || !mapperRuntime )
   {
      return;
   }

   Array< Transform >& bodyTransforms = ragdollListener->m_bodyTransformsLocalSpace;
   ragdollComp->calcPoseLocalSpace( bodyTransforms );

   mapperRuntime->calcPoseLocalSpace( bodyTransforms.getRaw(), outGeneratedPoseDiffLS );

   // we need to calculate the differences between this local pose and the bind pose
   Transform diff, boneLS;
   for ( uint i = 0; i < bonesCount; ++i )
   {
      boneLS.set( player->m_skeleton->m_boneLocalMatrices[i] );
      diff.setMulInverse( outGeneratedPoseDiffLS[i], boneLS );

      outGeneratedPoseDiffLS[i] = diff;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendTreeRagdollAnimation::RagdollComponentListener::RagdollComponentListener( Skeleton* animationSkeleton )
   : m_animationSkeleton( animationSkeleton )
   , m_ragdollComponent( NULL )
   , m_mapper( NULL )
   , m_mapperRuntime( NULL )
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

      m_mapper = m_ragdollComponent->m_mapper;
      if ( m_mapper )
      {
         m_mapperRuntime = new SkeletonMapperRuntime( *m_mapper );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeRagdollAnimation::RagdollComponentListener::onChildDetached( Entity* parentEntity, SceneNode* child )
{
   if ( child->isExactlyA< RagdollComponent >() && m_ragdollComponent == child )
   {
      m_ragdollComponent = NULL;
      m_mapper = NULL;

      delete m_mapperRuntime;
      m_mapperRuntime = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
