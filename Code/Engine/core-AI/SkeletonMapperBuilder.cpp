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

bool SkeletonMapperBuilder::buildMapperUsingBoneNames( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton, std::string& outErrorMsg )
{
   if ( !sourceSkeleton || !targetSkeleton )
   {
      outErrorMsg = "Skeletons we're supposed to define a mapping for are not specified.";
      return false;
   }

   m_mapper.reset();

   const uint sourceBonesCount = sourceSkeleton->getBoneCount();
   const uint targetBonesCount = targetSkeleton->getBoneCount();

   // find common bones in both skeletons
   Array< bool > mappedSourceBones( sourceBonesCount ); mappedSourceBones.resize( sourceBonesCount, false );
   Array< bool > mappedTargetBones( targetBonesCount ); mappedTargetBones.resize( targetBonesCount, false );
   {
      List< std::string > commonBones;
      for ( uint i = 0; i < sourceBonesCount; ++i )
      {
         const std::string& boneName = sourceSkeleton->m_boneNames[i];
         if ( targetSkeleton->getBoneIndex( boneName.c_str() ) >= 0 )
         {
            commonBones.pushBack( boneName );
         }
      }

      for ( List< std::string >::iterator it = commonBones.begin(); !it.isEnd(); ++it )
      {
         const std::string& boneName = *it;

         const int sourceBoneIdx = sourceSkeleton->getBoneIndex( boneName.c_str() );
         if ( sourceBoneIdx >= 0 )
         {
            mappedSourceBones[sourceBoneIdx] = true;
         }

         const int targetBoneIdx = targetSkeleton->getBoneIndex( boneName.c_str() );
         if ( targetBoneIdx >= 0 )
         {
            mappedTargetBones[targetBoneIdx] = true;
         }
      }
   }

   // setup skeletons
   m_mapper.m_sourceSkeleton = sourceSkeleton;
   m_mapper.m_targetSkeleton = targetSkeleton;


   // Define chains - we're gonna step through all bones in each skeleton.
   // Whenever we find a bone that's not mapped, we're gonna try finding a chain that already contains its parent
   // If we don't find such a chain, we're gonna create a new one, and at the end, we're gonna try merging 
   // all chains.
   for ( uint boneIdx = 0; boneIdx < sourceBonesCount; ++boneIdx )
   {
      bool result = SkeletonMapperUtils::addBoneToChain( sourceSkeleton, boneIdx, mappedSourceBones, m_mapper.m_sourceChains, outErrorMsg );
      if ( !result )
      {
         m_mapper.reset();
         return false;
      }
   }

   for ( uint boneIdx = 0; boneIdx < targetBonesCount; ++boneIdx )
   {
      bool result = SkeletonMapperUtils::addBoneToChain( targetSkeleton, boneIdx, mappedTargetBones, m_mapper.m_targetChains, outErrorMsg );
      if ( !result )
      {
         m_mapper.reset();
         return false;
      }
   }

   // define mappings
   const uint targetMappingsCount = m_mapper.m_targetChains.size();
   m_mapper.m_chainMappings.resize( targetMappingsCount, -1 );
   for ( uint targetChainIdx = 0; targetChainIdx < targetMappingsCount; ++targetChainIdx )
   {
      int sourceChainIdx = SkeletonMapperUtils::getChainIdx( m_mapper.m_sourceChains, m_mapper.m_targetChains[targetChainIdx]->m_name.c_str() );
      m_mapper.m_chainMappings[targetChainIdx] = sourceChainIdx;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
