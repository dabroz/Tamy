/// @file   core-AI\SkeletonMapperUtils.h
/// @brief  a set of utility methods that operate on a skeleton mapper
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class SkeletonBoneChain;
class Skeleton;
class SkeletonTree;

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
    * @param chainsCollection
    * @param chainName
    */
   static int getChainIdx( const Array< SkeletonBoneChain* >& chainsCollection, const char* chainName );
    
   /**
    * Creates a tree that corresponds to the configuration of the specified skeleton.
    */
   static SkeletonTree* buildSkeletonTree( const Skeleton* baseSkeleton );

   /**
    * Expands the specified chain until it hits a joint bone or another of the defined chains
    *
    * @param chainIdx
    * @param chainsCollection
    * @param skeletonTreeRoot
    */
   static void expandChain( uint chainIdx, const Array< SkeletonBoneChain* >& chainsCollection, const SkeletonTree* skeletonTree );

};

///////////////////////////////////////////////////////////////////////////////

struct SkeletonTreeNode
{
   DECLARE_ALLOCATOR( SkeletonTreeNode, AM_DEFAULT );

   int                                    m_boneIdx;
   List< SkeletonTreeNode* >              m_children;

   SkeletonTreeNode( int boneIdx )
      : m_boneIdx( boneIdx )
   {}
};

///////////////////////////////////////////////////////////////////////////////

class SkeletonTree
{
   DECLARE_ALLOCATOR( SkeletonTree, AM_DEFAULT );

private:
   SkeletonTreeNode*          m_root;

public:
   /**
    * Constructor.
    *
    * @param root
    */
   SkeletonTree( SkeletonTreeNode* root );
   ~SkeletonTree();

   /**
   * Returns a node that corresponds to the specified bone
   */
   const SkeletonTreeNode* getNodeForBone( uint boneIdx ) const;
};

///////////////////////////////////////////////////////////////////////////////
