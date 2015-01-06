#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"


///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperUtils::buildChainSkeleton( const Skeleton* baseSkeleton, const Array< SkeletonBoneChain* >& chainBones, Skeleton* outChainSkeleton )
{
   // clear the output skeleton
   outChainSkeleton->clear();

   // add bone names to the skeleton
   const uint chainBonesCount = chainBones.size();
   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const SkeletonBoneChain* chain = chainBones[i];
      outChainSkeleton->addBone( chain->m_name.c_str(), Transform::IDENTITY, -1, 1.0f );
   }

   Array< Transform > baseModelPose( baseSkeleton->getBoneCount() );
   baseModelPose.resize( baseSkeleton->getBoneCount(), Transform::IDENTITY );
   baseSkeleton->calculateLocalToModel( baseSkeleton->m_boneLocalMatrices.getRaw(), baseModelPose.getRaw() );

   // setup the hierarchy and the model space pose, which we'll later on use to calculate the local space transforms
   // for the bones
   Array< Transform > chainModelPose( chainBonesCount );
   chainModelPose.resize( chainBonesCount, Transform::IDENTITY );
   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      const SkeletonBoneChain* chain = chainBones[i];
      const int parentBoneIdx = baseSkeleton->m_boneParentIndices[chain->m_firstBoneIdx];
      if ( parentBoneIdx >= 0 )
      {
         outChainSkeleton->m_boneParentIndices[i] = findChainByBone( chainBones, parentBoneIdx );
      }

      chainModelPose[i] = baseModelPose[chain->m_firstBoneIdx];
   }

   // calculate the local pose of the chain skeleton 
   Array< Transform > chainLocalPose( chainBonesCount );
   chainLocalPose.resize( chainBonesCount, Transform::IDENTITY );
   outChainSkeleton->calculateModelToLocal( chainModelPose.getRaw(), chainLocalPose.getRaw() );

   for ( uint i = 0; i < chainBonesCount; ++i )
   {
      chainLocalPose[i].toMatrix( outChainSkeleton->m_boneLocalMatrices[i] );
   }

   // build the skeleton
   outChainSkeleton->buildSkeleton();
}

///////////////////////////////////////////////////////////////////////////////

int SkeletonMapperUtils::findChainByBone( const Array< SkeletonBoneChain* >& chainsCollection, uint baseSkeletonBoneIdx )
{
   const int chainsCount = chainsCollection.size();
   for ( int chainIdx = 0; chainIdx < chainsCount; ++chainIdx )
   {
      SkeletonBoneChain* chain = chainsCollection[chainIdx];
      if ( chain->m_firstBoneIdx == baseSkeletonBoneIdx )
      {
         return chainIdx;
      }

      for ( int boneIdx = chain->m_lastBoneIdx; boneIdx != chain->m_firstBoneIdx; boneIdx = chain->m_skeleton->m_boneParentIndices[boneIdx] )
      {
         if ( boneIdx == baseSkeletonBoneIdx )
         {
            return chainIdx;
         }
      }
   }

   return -1;
}

///////////////////////////////////////////////////////////////////////////////

int SkeletonMapperUtils::getChainIdx( const Array< SkeletonBoneChain* >& chainsCollection, const char* chainName )
{
   const int count = chainsCollection.size();
   for ( int i = 0; i < count; ++i )
   {
      if ( chainsCollection[i]->m_name == chainName )
      {
         return i;
      }
   }

   return -1;
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonMapperUtils::addBoneToChain( const Skeleton* baseSkeleton, uint baseSkeletonBoneIdx, const Array< bool >& mappedBones, Array< SkeletonBoneChain* >& inOutChainsCollection, std::string& outErrMsg )
{
   int idx = findChainByBone( inOutChainsCollection, baseSkeletonBoneIdx );
   if ( idx >= 0 )
   {
      // the bone's already in a chain
      return true;
   }


   // look for the first parent we can append the bone to
   for ( int boneIdx = baseSkeletonBoneIdx; boneIdx >= 0; boneIdx = baseSkeleton->m_boneParentIndices[boneIdx] )
   {
      if ( !mappedBones[boneIdx] )
      {
         // the bone isn't mapped - so keep on going
         continue;
      }

      // the bone is mapped - so it either has a chain already, or we need to create one for it
      int chainIdx = findChainByBone( inOutChainsCollection, boneIdx );
      if ( chainIdx < 0 )
      {

         // we need to create a new chain
         SkeletonBoneChain* newChain = new SkeletonBoneChain( baseSkeleton, baseSkeleton->m_boneNames[boneIdx].c_str(), boneIdx, baseSkeletonBoneIdx );
         inOutChainsCollection.push_back( newChain );

         return true;
      }
      else
      {
         // extend the existing chain to include the new bone
         SkeletonBoneChain* existingChain = inOutChainsCollection[chainIdx];

         // but before we do, we need to make sure that the chain doesn't branch, and we're trying to map
         // one of the branches, when another one is already mapped.
         // That of course is an error on the user part, not having defined a mapping for the branch bone,
         // and we need to flag it
         bool branchFound = detectBranching( baseSkeleton, existingChain->m_firstBoneIdx, existingChain->m_lastBoneIdx, baseSkeletonBoneIdx );
         if ( branchFound )
         {
            // we detected a branch - notify the user
            char tmpErrMsg[512];
            sprintf_s( tmpErrMsg, "Branch detected between existing chain %s ( %d, %d ), and a potential chain (%d, %d)",
                       existingChain->m_name.c_str(),
                       existingChain->m_firstBoneIdx,
                       existingChain->m_lastBoneIdx,
                       existingChain->m_firstBoneIdx,
                       baseSkeletonBoneIdx );

            outErrMsg = tmpErrMsg;
            return false;
         }
         else
         {
            // good to go - no branching detected
            existingChain->m_lastBoneIdx = baseSkeletonBoneIdx;
            return true;
         }
      }
   }

   // we should never end up here - if we did, then there's an error somewhere in the code or in the data set that
   // we haven't fixed yet
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonMapperUtils::detectBranching( const Skeleton* baseSkeleton, uint firstBoneIdx, uint lastBoneIdx, uint newChainLastBoneIdx )
{
   // The method is simple - start moving backwards from the 'newChainLastBoneIdx' - if we get to the 'lastBoneIdx' before
   // we get to 'firstBoneIdx', then this new bone is a continuation of the original chain.
   // However if we get to 'firstBoneIdx' first, we have a branch

   for ( int boneIdx = baseSkeleton->m_boneParentIndices[newChainLastBoneIdx]; boneIdx >= 0; boneIdx = baseSkeleton->m_boneParentIndices[boneIdx] )
   {
      if ( boneIdx == lastBoneIdx )
      {
         // the new bone is a continuation of the original chain
         return false;
      }

      if ( boneIdx == firstBoneIdx )
      {
         // there was a branch somewhere along the way
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
