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

SkeletonTree* SkeletonMapperUtils::buildSkeletonTree( const Skeleton* baseSkeleton )
{
   // build a tree based on the skeleton that we can later on use to define skeletons.
   // We need to take this step, because we need information which bone has what children,
   // as opposed to which node is parented by what bone - the information that the Skeleton class gives us

   SkeletonTreeNode* root = new SkeletonTreeNode( -1 );
   List< SkeletonTreeNode* > nodesToAnalyze;

   nodesToAnalyze.pushBack( root );
   const uint boneCount = baseSkeleton->getBoneCount();
   while ( !nodesToAnalyze.empty() )
   {
      SkeletonTreeNode* node = nodesToAnalyze.front();
      nodesToAnalyze.popFront();

      for ( uint i = 0; i < boneCount; ++i )
      {
         if ( baseSkeleton->m_boneParentIndices[i] == node->m_boneIdx )
         {
            SkeletonTreeNode* childNode = new SkeletonTreeNode( i );
            node->m_children.pushBack( childNode );
            nodesToAnalyze.pushBack( childNode );
         }
      }
   }

   return new SkeletonTree( root );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperUtils::expandChain( uint chainIdx, const Array< SkeletonBoneChain* >& chainsCollection, const SkeletonTree* skeletonTree )
{
   SkeletonBoneChain* chainBeingExpanded = chainsCollection[chainIdx];

   const SkeletonTreeNode* boneNode = skeletonTree->getNodeForBone( chainBeingExpanded->m_lastBoneIdx );
   while ( boneNode->m_children.size() == 1 )
   {
      boneNode = boneNode->m_children.front();

      // verify that that bone doesn't belong to any of the chains
      const int existingChainIdx = findChainByBone( chainsCollection, boneNode->m_boneIdx );
      if ( existingChainIdx >= 0 )
      {
         // that bone already belongs to another chain - we reached the end of our chain
         break;
      }

      // expand the chain and move to the next bone
      chainBeingExpanded->m_lastBoneIdx = boneNode->m_boneIdx;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SkeletonTree::SkeletonTree( SkeletonTreeNode* root )
   : m_root( root )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonTree::~SkeletonTree()
{
   if ( !m_root )
   {
      return;
   }

   List< SkeletonTreeNode* > nodesQueue;
   nodesQueue.pushBack( m_root );

   while ( !nodesQueue.empty() )
   {
      SkeletonTreeNode* node = nodesQueue.front();
      nodesQueue.popFront();

      for ( List< SkeletonTreeNode* >::iterator it = node->m_children.begin(); !it.isEnd(); ++it )
      {
         nodesQueue.pushBack( *it );
      }

      delete node;
   }

   m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

const SkeletonTreeNode* SkeletonTree::getNodeForBone( uint boneIdx ) const
{
   List< SkeletonTreeNode* > nodesQueue;
   nodesQueue.pushBack( m_root );

   while ( !nodesQueue.empty() )
   {
      SkeletonTreeNode* node = nodesQueue.front();
      nodesQueue.popFront();

      if ( node->m_boneIdx == boneIdx )
      {
         return node;
      }

      for ( List< SkeletonTreeNode* >::iterator it = node->m_children.begin(); !it.isEnd(); ++it )
      {
         nodesQueue.pushBack( *it );
      }

   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
