#include "core-AI\SkeletonMapperBuilder.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\Skeleton.h"


///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder::SkeletonMapperBuilder( SkeletonMapper& mapper )
   : m_mapper( mapper )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder& SkeletonMapperBuilder::setSkeletons( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton )
{
   // clear the old mapping
   m_mapper.m_sourceSkeleton = sourceSkeleton;
   m_mapper.m_targetSkeleton = targetSkeleton;

   m_mapper.reset();

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder& SkeletonMapperBuilder::mapBone( const char* sourceBone, const char* targetBone )
{
   char sourceChainName[128];
   sprintf_s( sourceChainName, "__bone_%d", m_mapper.m_sourceChains.size() );
   addSourceChain( sourceChainName, sourceBone, sourceBone );

   char targetChainName[128];
   sprintf_s( targetChainName, "__bone_%d", m_mapper.m_targetChains.size() );
   addTargetChain( targetChainName, targetBone, targetBone );

   mapChain( sourceChainName, targetChainName );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder& SkeletonMapperBuilder::addSourceChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   const int firstBoneIdx = m_mapper.m_sourceSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_mapper.m_sourceSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_mapper.m_sourceSkeleton, chainName, firstBoneIdx, lastBoneIdx );
      m_mapper.m_sourceChains.push_back( chain );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder& SkeletonMapperBuilder::addTargetChain( const char* chainName, const char* firstBoneName, const char* lastBoneName )
{
   const int firstBoneIdx = m_mapper.m_targetSkeleton->getBoneIndex( firstBoneName );
   const int lastBoneIdx = m_mapper.m_targetSkeleton->getBoneIndex( lastBoneName );

   if ( firstBoneIdx >= 0 && lastBoneIdx >= 0 )
   {
      SkeletonBoneChain* chain = new SkeletonBoneChain( m_mapper.m_targetSkeleton, chainName, firstBoneIdx, lastBoneIdx );
      m_mapper.m_targetChains.push_back( chain );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperBuilder& SkeletonMapperBuilder::mapChain( const char* sourceChainName, const char* targetChainName )
{
   const int sourceChainIdx = SkeletonMapperUtils::getChainIdx( m_mapper.m_sourceChains, sourceChainName );
   const int targetChainIdx = SkeletonMapperUtils::getChainIdx( m_mapper.m_targetChains, targetChainName );
   if ( sourceChainIdx < 0 || targetChainIdx < 0 )
   {
      return *this;
   }

   if ( targetChainIdx >= ( int ) m_mapper.m_chainMappings.size() )
   {
      m_mapper.m_chainMappings.resize( targetChainIdx + 1, -1 );
   }

   m_mapper.m_chainMappings[targetChainIdx] = sourceChainIdx;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////
