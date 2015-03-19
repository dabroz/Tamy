/// @file   ext-ProceduralAnimation\BlendTreeRagdollAnimation.h
/// @brief  a generator that copies the current ragdoll pose as an animation pose
#pragma once

#include "core-AI\BlendTreeNode.h"
#include "core-MVC\EntityListener.h"
#include "core\RuntimeData.h"
#include "core\Transform.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class RagdollComponent;
class Skeleton;
class SkeletonMapper;
class SkeletonMapperRuntime;

///////////////////////////////////////////////////////////////////////////////

/**
 * This node generates a pose from the pose of the ragdoll associated with the character using the RagdollComponent.
 */
class BlendTreeRagdollAnimation : public BlendTreeNode
{
   DECLARE_ALLOCATOR( BlendTreeRagdollAnimation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   struct RagdollComponentListener : public EntityListener
   {
      DECLARE_ALLOCATOR( RagdollComponentListener, AM_DEFAULT );

   public:
      Skeleton*                                 m_animationSkeleton;
      RagdollComponent*                         m_ragdollComponent;
      const SkeletonMapper*                     m_mapper;
      SkeletonMapperRuntime*                    m_mapperRuntime;

      Array< Transform >                        m_bodyTransformsLocalSpace;

   public:
      RagdollComponentListener( Skeleton* animationSkeleton );

      // -------------------------------------------------------------------------
      // EntityListener implementation
      // -------------------------------------------------------------------------
      void onChildAttached( Entity* parentEntity, SceneNode* child ) override;
      void onChildDetached( Entity* parentEntity, SceneNode* child ) override;
   };

private:
   TRuntimeVar< RagdollComponentListener* >     m_ragdollComponentListener;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeRagdollAnimation( const char* name = "RagdollAnimation" );
   ~BlendTreeRagdollAnimation();

   // -------------------------------------------------------------------------
   // BlendTreeNode implementation
   // -------------------------------------------------------------------------
   void initializeLayout( BlendTreePlayer* player ) const override;
   void deinitializeLayout( BlendTreePlayer* player ) const override;
   void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const override;
};

///////////////////////////////////////////////////////////////////////////////
