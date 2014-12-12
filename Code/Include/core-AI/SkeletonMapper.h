/// @file   core-AI\SkeletonMapper.h
/// @brief  a tool that maps bone transforms between two different skeletons
#pragma once

#include "core\ReflectionObject.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Transform.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Skeleton;
class SkeletonMapper;

///////////////////////////////////////////////////////////////////////////////

enum BoneLookupMethod
{
   Lookup_ByName,
   Lookup_ByDistance,
};

///////////////////////////////////////////////////////////////////////////////

class SkeletonBoneChain : public ReflectionObject
{
   DECLARE_ALLOCATOR( SkeletonBoneChain, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   const Skeleton*      m_skeleton;

   // the bones in the chain can be traced from the last bone to the first using parental relations
   // defined in the skeleton.
   // So it's:
   // m_lastBoneIdx -> m_lastBoneIdx's parent -> ... -> m_firstBoneIdx
   uint                 m_firstBoneIdx;
   uint                 m_lastBoneIdx;

public:
   /**
    * Default constructor.
    */
   SkeletonBoneChain();

   /**
    * Constructor.
    *
    * @param skeleton
    * @param firstBoneIdx
    * @param lastBoneIdx
    */
   SkeletonBoneChain( const Skeleton* skeleton, uint firstBoneIdx, uint lastBoneIdx );

   /**
    * Updates the specified pose, transforming the bones this chain consists of.
    *
    * @param t
    * @param outPose
    */
   void updatePose( const Transform& t, Transform* outPose ) const;
};

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper : public ReflectionObject
{
   DECLARE_ALLOCATOR( SkeletonMapper, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static
   const Skeleton*               m_sourceSkeleton;
   const Skeleton*               m_targetSkeleton;

private:
   // static
   Array< SkeletonBoneChain* >   m_sourceChains;
   Array< SkeletonBoneChain* >   m_targetChains;
   Array< int >                  m_chainMappings;

   Skeleton*                     m_sourceChainSkeleton;
   Skeleton*                     m_targetChainSkeleton;

   mutable Array< Transform >    m_tmpSourceBasePose;
   mutable Array< Transform >    m_tmpSourceChainPose;
   mutable Array< Transform >    m_tmpTargetChainPose;
   mutable Array< Transform >    m_tmpTargetBasePose;

   Array< Transform >            m_targetBindPose;
   Array< Transform >            m_targetToSource;
   Array< Transform >            m_sourceToTarget;

public:
   /**
    * Constructor.
    */
   SkeletonMapper();
   ~SkeletonMapper();

   /**
    * Defines mapping for the specified skeletons.
    *
    * @param sourceSkeleton
    * @param targetSkeleton
    */
   SkeletonMapper& defineMapping( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton );

   /**
    * Maps a bone from the source skeleton onto a bone from the target skeleton.
    *
    * @param sourceBone
    * @param targetBone
    */
   SkeletonMapper& mapBone( const char* sourceBone, const char* targetBone );

   /**
    * Calculates a pose the target skeleton should assume to look the same as the source.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void calcPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;

   // -------------------------------------------------------------------------
   // Mapping definitions
   // -------------------------------------------------------------------------

   /**
    * Defines a chain for the source skeleton.
    *
    * @param chainName
    * @param firstBoneName
    * @param lastBoneName
    */
   SkeletonMapper& addSourceChain( const char* chainName, const char* firstBoneName, const char* lastBoneName );

   /**
    * Defines a chain for the target skeleton.
    *
    * @param chainName
    * @param firstBoneName
    * @param lastBoneName
    */
   SkeletonMapper& addTargetChain( const char* chainName, const char* firstBoneName, const char* lastBoneName );

   /**
    * Maps one chain onto another.
    */
   SkeletonMapper& mapChain( const char* sourceChain, const char* targetChain );

   /**
    * Builds the mapper.
    */
   void buildMapper();

private:
   /**
    * Locates a chain with the specified name in the specified collection and returns its index.
    *
    * @param chainSkeleton
    * @param chainName
    */
   int getChainIdx( const Skeleton* chainSkeleton, const char* chainName ) const;

   /**
    * Returns the index of a chain that contains the specified bone from its based skeleton.
    *
    * @param chainsCollection
    * @param baseSkeletonBoneIdx
    */
   int findChainByBone( const Array< SkeletonBoneChain* >& chainsCollection, uint baseSkeletonBoneIdx ) const;

   /**
    * Builds a chain.
    *
    * @param baseSkeleton
    * @param chainBones
    * @param outChainSkeleton
    */
   void buildChain( const Skeleton* baseSkeleton, const Array< SkeletonBoneChain* >& chainBones, Skeleton* outChainSkeleton ) const;
};

///////////////////////////////////////////////////////////////////////////////
