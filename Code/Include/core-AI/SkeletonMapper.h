/// @file   core-AI\SkeletonMapper.h
/// @brief  a tool that maps bone transforms between two different skeletons
#pragma once

#include "core\ReflectionObject.h"
#include "core\Resource.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Transform.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Skeleton;
class SkeletonMapper;

///////////////////////////////////////////////////////////////////////////////

class SkeletonBoneChain : public ReflectionObject
{
   DECLARE_ALLOCATOR( SkeletonBoneChain, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   const Skeleton*      m_skeleton;

   std::string          m_name;

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
    * @param chainName
    * @param firstBoneIdx
    * @param lastBoneIdx
    */
   SkeletonBoneChain( const Skeleton* skeleton, const std::string& chainName, uint firstBoneIdx, uint lastBoneIdx );

   /**
    * Updates the specified pose, transforming the bones this chain consists of.
    *
    * @param t
    * @param outPose
    */
   void updatePose( const Transform& t, Transform* outPose ) const;
};

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper : public Resource
{
   DECLARE_ALLOCATOR( SkeletonMapper, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   // static
   const Skeleton*               m_sourceSkeleton;
   const Skeleton*               m_targetSkeleton;

private:
   // static
   Array< SkeletonBoneChain* >   m_sourceChains;
   Array< SkeletonBoneChain* >   m_targetChains;
   Array< int >                  m_chainMappings;

   // runtime
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
    * Resets the mapping.
    */
   void reset();

   /**
    * Calculates a pose the target skeleton should assume to look the same as the source.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void calcPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;


   // -------------------------------------------------------------------------
   // Getters/setters
   // -------------------------------------------------------------------------

   /**
    * Returns the number of chains in the source skeleton.
    */
   inline uint getSourceChainsCount() const {
      return m_sourceChains.size();
   }

   /**
    * Returns the number of chains in the target skeleton.
    */
   inline uint getTargetChainsCount() const {
      return m_targetChains.size();
   }

   /**
    * Returns a chain from the source skeleton.
    *
    * @param idx
    */
   inline const SkeletonBoneChain* getSourceChain( uint idx ) const {
      return m_sourceChains[idx];
   }

   /**
    * Returns a chain from the target skeleton.
    *
    * @param idx
    */
   inline const SkeletonBoneChain* getTargetChain( uint idx ) const {
      return m_targetChains[idx];
   }

   /**
    * Returns the number of chain mappings.
    */
   inline uint getMappingsCount() const {
      return m_chainMappings.size();
   }

   /**
    * Returns the index of a source chain that maps onto the specified
    * target chain.
    *
    * @param targetChainIdx
    * @return -1 if such mapping is not defined, >=0 if it exists
    */
   inline int getMappingForChain( uint targetChainIdx ) const {
      return m_chainMappings[targetChainIdx];
   }

   /**
    * Returns the skeleton that shows the hierarchy of source chains.
    */
   inline const Skeleton* getSourceChainSkeleton() const {
      return m_sourceChainSkeleton;
   }

   /**
   * Returns the skeleton that shows the hierarchy of target chains.
   */
   inline const Skeleton* getTargetChainSkeleton() const {
      return m_targetChainSkeleton;
   }

   // -------------------------------------------------------------------------
   // Mapper construction methods
   // -------------------------------------------------------------------------

   /**
    * Builds a mapper using bone names.
    *
    * @param sourceSkeleton
    * @param targetSkeleton
    */
   bool buildMapperUsingBoneNames( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton, std::string& outErrorMsg );

   // -------------------------------------------------------------------------
   // Mapping definitions
   // -------------------------------------------------------------------------

   /**
    * Defines mapping for the specified skeletons.
    *
    * @param sourceSkeleton
    * @param targetSkeleton
    */
   SkeletonMapper& setSkeletons( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton );

   /**
   * Maps a bone from the source skeleton onto a bone from the target skeleton.
   *
   * @param sourceBone
   * @param targetBone
   */
   SkeletonMapper& mapBone( const char* sourceBone, const char* targetBone );

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

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void onResourceLoaded( ResourcesManager& mgr ) override;
};

///////////////////////////////////////////////////////////////////////////////
