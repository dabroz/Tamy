/// @file   core-AI\SkeletonMapperRuntime.h
/// @brief  a skeleton mapper runtime
#pragma once

#include "core-MVC\Component.h"
#include "core\Array.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperRuntime
{
   DECLARE_ALLOCATOR( SkeletonMapperRuntime, AM_DEFAULT );

private:
   // static data
   const SkeletonMapper&            m_mapper;

   // runtime data
   Skeleton*                        m_sourceChainSkeleton;
   Skeleton*                        m_targetChainSkeleton;

   mutable Array< Transform >       m_tmpSourceBasePose;
   mutable Array< Transform >       m_tmpSourceChainPose;
   mutable Array< Transform >       m_tmpTargetChainPose;
   mutable Array< Transform >       m_tmpTargetBasePose;

   Array< Transform >               m_targetBindPose;
   Array< Transform >               m_targetToSource;
   Array< Transform >               m_sourceToTarget;

   Array< Transform >               m_outTargetPose;

public:
   /**
    * Constructor.
    *
    * @param mapper
    */
   SkeletonMapperRuntime( const SkeletonMapper& mapper );
   ~SkeletonMapperRuntime();

   /**
    * Calculates a pose the target skeleton should assume to look the same as the source.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void calcPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;

   /**
    * Translates a pose the target skeleton should assume to look the same as the source.
    * Returns a pointer to the pose ( which is maintained internally by this class ).
    *
    * @param sourcePose
    * @return a pointer to the array that contains the target pose
    */
   Transform* translatePose( const Transform* sourcePose );

   // -------------------------------------------------------------------------
   // Runtime management.
   // -------------------------------------------------------------------------
   /**
    * Compiles the runtime representation of the mapper.
    */
   void compileRuntime();

   /**
    * Destroys the runtime representation of the mapper.
    */
   void destroyRuntime();
};

///////////////////////////////////////////////////////////////////////////////
