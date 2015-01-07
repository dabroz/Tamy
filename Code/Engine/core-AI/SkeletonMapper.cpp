#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\Skeleton.h"
#include "core\Transform.h"
#include "core\ListUtils.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( SkeletonMapper, tsm, AM_BINARY );
   PROPERTY( Skeleton*, m_sourceSkeleton );
   PROPERTY( Skeleton*, m_targetSkeleton );
   PROPERTY( Array< SkeletonBoneChain* >, m_sourceChains );
   PROPERTY( Array< SkeletonBoneChain* >, m_targetChains );
   PROPERTY( Array< int >, m_chainMappings );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper::SkeletonMapper()
   : m_sourceSkeleton( NULL )
   , m_targetSkeleton( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper::~SkeletonMapper()
{
   m_sourceSkeleton = NULL;
   m_targetSkeleton = NULL;

   uint chainsCount = m_sourceChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_sourceChains[i];
   }

   chainsCount = m_targetChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_targetChains[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::reset()
{
   uint chainsCount = m_sourceChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_sourceChains[i];
   }
   m_sourceChains.clear();

   chainsCount = m_targetChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      delete m_targetChains[i];
   }
   m_targetChains.clear();

   m_chainMappings.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonBoneChain );
   PARENT( ReflectionObject );
   PROPERTY( Skeleton*, m_skeleton );
   PROPERTY( std::string, m_name );
   PROPERTY( uint, m_firstBoneIdx );
   PROPERTY( uint, m_lastBoneIdx );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonBoneChain::SkeletonBoneChain()
   : m_skeleton( NULL )
   , m_firstBoneIdx( 0 )
   , m_lastBoneIdx( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonBoneChain::SkeletonBoneChain( const Skeleton* skeleton, const std::string& chainName, uint firstBoneIdx, uint lastBoneIdx )
   : m_skeleton( skeleton )
   , m_name( chainName )
   , m_firstBoneIdx( firstBoneIdx )
   , m_lastBoneIdx( lastBoneIdx )
{
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonBoneChain::updatePose( const Transform& t, Transform* outPose ) const
{
   // TODO: optimize
   Array< uint > boneIndices;
   for ( uint boneIdx = m_lastBoneIdx; boneIdx != m_firstBoneIdx; boneIdx = m_skeleton->m_boneParentIndices[boneIdx] )
   {
      boneIndices.push_back( boneIdx );
   }

   outPose[m_firstBoneIdx] = t;

   const uint chainBonesCount = boneIndices.size();
   Transform bindPoseT;
   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const uint boneIdx = boneIndices[i];
      const uint parentBoneIdx = m_skeleton->m_boneParentIndices[boneIdx];

      bindPoseT.set( m_skeleton->m_boneLocalMatrices[boneIdx] );
      outPose[boneIdx].setMul( bindPoseT, outPose[parentBoneIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////
