/// @file   core-AI\SkeletonMapper.h
/// @brief  a tool that maps bone transforms between two different skeletons
#pragma once

#include "core\ReflectionObject.h"
#include "core\Array.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

class Skeleton;

///////////////////////////////////////////////////////////////////////////////

enum BoneLookupMethod
{
   Lookup_ByName,
   Lookup_ByDistance,
};

///////////////////////////////////////////////////////////////////////////////

struct BoneChain : public ReflectionObject
{
   DECLARE_ALLOCATOR( BoneChain, AM_DEFAULT );
   DECLARE_STRUCT();

   enum Type
   {
      OneToOne,
      OneToMany,
      ManyToOne,
      ManyToMany,
   };

   Type                          m_type;
   Array< uint >                 m_sourceBones;
   Array< uint >                 m_targetBones;
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
   Array< BoneChain >             m_boneChains;

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
    * Calculates a pose the target skeleton should assume to look the same as the source.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void calcPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;

private:
   void defineIdentityMapping();
   void mapByDistance();
   void mapByName();
   void buildMappingTable( const Array< int >& sourceToTargetMapping, uint mappedBonesCount );
   
   typedef void( SkeletonMapper::*MappingFunc )( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const;
   void mapOneToOne( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const;
   void mapOneToMany( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const;
   void mapManyToOne( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const;
   void mapManyToMany( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const;
};

///////////////////////////////////////////////////////////////////////////////
