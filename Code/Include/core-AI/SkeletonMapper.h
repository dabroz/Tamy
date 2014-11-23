/// @file   core-AI\SkeletonMapper.h
/// @brief  a tool that maps bone transforms between two different skeletons
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class Skeleton;
struct Transform;

///////////////////////////////////////////////////////////////////////////////

enum BoneLookupMethod
{
   Lookup_ByName,
   Lookup_ByDistance,
};

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper : public ReflectionObject
{
   DECLARE_ALLOCATOR( SkeletonMapper, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static
   const Skeleton*      m_sourceSkeleton;
   const Skeleton*      m_targetSkeleton;

private:
   // static
   Array< uint >        m_mapping;

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
    * @param lookupMethod
    */
   void defineMapping( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton, BoneLookupMethod lookupMethod );

   /**
    * Applies a pose of the source skeleton to the target skeleton.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void setPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;

private:
   void defineIdentityMapping();
   void mapByDistance();
   void mapByName();
   void buildMappingTable( const Array< int >& sourceToTargetMapping, uint mappedBonesCount );
};

///////////////////////////////////////////////////////////////////////////////
