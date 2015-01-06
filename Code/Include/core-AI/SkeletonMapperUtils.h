/// @file   core-AI\SkeletonMapperUtils.h
/// @brief  a set of utility methods that operate on a skeleton mapper
#pragma once

#include "core\Array.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////

class SkeletonBoneChain;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperUtils
{
public:
   /**
    * Builds a skeleton that describes the hierarchy between chains.
    *
    * @param baseSkeleton
    * @param chainBones
    * @param outChainSkeleton
    */
   static void buildChainSkeleton( const Skeleton* baseSkeleton, const Array< SkeletonBoneChain* >& chainBones, Skeleton* outChainSkeleton );

   /**
    * Returns the index of a chain that contains the specified bone from its based skeleton.
    *
    * @param chainsCollection
    * @param baseSkeletonBoneIdx
    */
   static int findChainByBone( const Array< SkeletonBoneChain* >& chainsCollection, uint baseSkeletonBoneIdx );

   /**
    * Locates a chain with the specified name in the specified collection and returns its index.
    *
    * @param chainSkeleton
    * @param chainName
    */
   static int getChainIdx( const Skeleton* chainSkeleton, const char* chainName );

   /**
    * Add a bone to an existing chain, or creates a new chain if the bone doesn't fit into any of the existing ones.
    *
    * @param baseSkeleton
    * @param baseSkeletonBoneIdx
    * @param mappedSourceBones
    * @param inOutChainsCollection
    * @param outErrMsg
    *
    * @return 'true' if the bone was added, 'false' if there were errors
    */
   static bool addBoneToChain( const Skeleton* baseSkeleton, uint baseSkeletonBoneIdx, const Array< bool >& mappedSourceBones, Array< SkeletonBoneChain* >& inOutChainsCollection, std::string& outErrMsg );

   /**
    * Detects if two chains starting at 'firstBoneIdx' are branched.
    *
    * @param baseSkeleton
    * @param firstBoneIdx
    * @param lastBoneIdx
    * @param newChainLastBoneIdx
    *
    * @return 'true' if a branch was detected, 'false' otherwise
    */
   static bool detectBranching( const Skeleton* baseSkeleton, uint firstBoneIdx, uint lastBoneIdx, uint newChainLastBoneIdx );
};

///////////////////////////////////////////////////////////////////////////////
