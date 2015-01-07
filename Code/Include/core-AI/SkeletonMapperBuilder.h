/// @file   core-AI\SkeletonMapperBuilder.h
/// @brief  a skeleton mapper builder
#pragma once

#include "core\MemoryRouter.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperBuilder
{
   DECLARE_ALLOCATOR( SkeletonMapperBuilder, AM_DEFAULT );

private:
   SkeletonMapper&            m_mapper;

public:
   /**
    * Constructor.
    *
    * @param mapper
    */
   SkeletonMapperBuilder( SkeletonMapper& mapper );

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
   // Basic mapper building API
   // -------------------------------------------------------------------------

   /**
    * Defines mapping for the specified skeletons.
    *
    * @param sourceSkeleton
    * @param targetSkeleton
    */
   SkeletonMapperBuilder& setSkeletons( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton );

   /**
    * Maps a bone from the source skeleton onto a bone from the target skeleton.
    *
    * @param sourceBone
    * @param targetBone
    */
   SkeletonMapperBuilder& mapBone( const char* sourceBone, const char* targetBone );

   /**
    * Defines a chain for the source skeleton.
    *
    * @param chainName
    * @param firstBoneName
    * @param lastBoneName
    */
   SkeletonMapperBuilder& addSourceChain( const char* chainName, const char* firstBoneName, const char* lastBoneName );

   /**
    * Defines a chain for the target skeleton.
    *
    * @param chainName
    * @param firstBoneName
    * @param lastBoneName
    */
   SkeletonMapperBuilder& addTargetChain( const char* chainName, const char* firstBoneName, const char* lastBoneName );

   /**
    * Maps one chain onto another.
    *
    * @param sourceChain
    * @param targetChain
    */
   SkeletonMapperBuilder& mapChain( const char* sourceChain, const char* targetChain );
};

///////////////////////////////////////////////////////////////////////////////
