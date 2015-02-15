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

SkeletonMapper& SkeletonMapper::setSkeletons( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton )
{
   // clear the old mapping
   m_sourceSkeleton = sourceSkeleton;
   m_targetSkeleton = targetSkeleton;

   reset();

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::mapBone( const char* sourceBone, const char* targetBone )
{
   char sourceChainName[128];
   sprintf_s( sourceChainName, "__bone_%d", m_sourceChains.size() );
   addSourceChain( sourceChainName, sourceBone, sourceBone );

   char targetChainName[128];
   sprintf_s( targetChainName, "__bone_%d", m_targetChains.size() );
   addTargetChain( targetChainName, targetBone, targetBone );

   mapChain( sourceChainName, targetChainName );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::addSourceChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   if ( !m_sourceSkeleton )
   {
      return *this;
   }

   const int firstBoneIdx = m_sourceSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_sourceSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_sourceSkeleton, chainName, firstBoneIdx, lastBoneIdx );
      m_sourceChains.push_back( chain );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::addTargetChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   if ( !m_targetSkeleton )
   {
      return *this;
   }

   const int firstBoneIdx = m_targetSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_targetSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_targetSkeleton, chainName, firstBoneIdx, lastBoneIdx );
      m_targetChains.push_back( chain );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapper& SkeletonMapper::mapChain( const char* sourceChainName, const char* targetChainName )
{
   const int sourceChainIdx = SkeletonMapperUtils::getChainIdx( m_sourceChains, sourceChainName );
   const int targetChainIdx = SkeletonMapperUtils::getChainIdx( m_targetChains, targetChainName );
   if ( sourceChainIdx < 0 || targetChainIdx < 0 )
   {
      return *this;
   }

   if ( targetChainIdx >= ( int ) m_chainMappings.size() )
   {
      m_chainMappings.resize( targetChainIdx + 1, -1 );
   }

   m_chainMappings[targetChainIdx] = sourceChainIdx;

   return *this;
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
   Array< int > boneIndices;
   for ( int boneIdx = m_lastBoneIdx; boneIdx != m_firstBoneIdx && boneIdx >= 0; boneIdx = m_skeleton->m_boneParentIndices[boneIdx] )
   {
      boneIndices.push_back( boneIdx );
   }

   outPose[m_firstBoneIdx] = t;

   const uint chainBonesCount = boneIndices.size();
   Transform bindPoseT;
   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const int boneIdx = boneIndices[i];
      const int parentBoneIdx = m_skeleton->m_boneParentIndices[boneIdx];

      bindPoseT.set( m_skeleton->m_boneLocalMatrices[boneIdx] );
      outPose[boneIdx].setMul( bindPoseT, outPose[parentBoneIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////
