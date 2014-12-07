#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BoneChain );
   PARENT( ReflectionObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonMapper );
   PARENT( ReflectionObject );
   PROPERTY( Skeleton*, m_sourceSkeleton );
   PROPERTY( Skeleton*, m_targetSkeleton );
   PROPERTY( Array< BoneChain >, m_boneChains );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( BoneLookupMethod );
   ENUM_VAL( Lookup_ByName );
   ENUM_VAL( Lookup_ByDistance );
END_ENUM();

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
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::defineMapping( const Skeleton* sourceSkeleton, const Skeleton* targetSkeleton, BoneLookupMethod lookupMethod )
{
   if ( m_sourceSkeleton == sourceSkeleton && m_targetSkeleton == targetSkeleton )
   {
      // nothing changes
      return;
   }
   
   // clear the old mapping
   m_sourceSkeleton = sourceSkeleton;
   m_targetSkeleton = targetSkeleton;
   m_boneChains.clear();

   if ( !m_sourceSkeleton || !m_targetSkeleton )
   {
      // we require two skeletons to define a mapping
      return;
   }

   if ( m_sourceSkeleton == m_targetSkeleton )
   {
      defineIdentityMapping();
   }
   else
   {
      switch ( lookupMethod )
      {
         case Lookup_ByDistance:
         {
            mapByDistance();
            break;
         }

         case Lookup_ByName:
         {
            mapByName();
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::defineIdentityMapping()
{
   const uint bonesCount = m_sourceSkeleton->getBoneCount();
   Array<int> tmpMapping( bonesCount );
   tmpMapping.resize( bonesCount, -1 );
   for ( uint i = 0; i < bonesCount; ++i )
   {
      tmpMapping[i] = i;
   }

   buildMappingTable( tmpMapping, bonesCount );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapByDistance()
{
   const uint sourceBonesCount = m_sourceSkeleton->getBoneCount();
   const uint targetBonesCount = m_targetSkeleton->getBoneCount();

   Array<int> tmpMapping( sourceBonesCount );
   tmpMapping.resize( sourceBonesCount, -1 );

   Array< Transform > sourceModelPose( sourceBonesCount ); sourceModelPose.resize( sourceBonesCount, Transform::IDENTITY );
   Array< Transform > targetModelPose( targetBonesCount ); targetModelPose.resize( targetBonesCount, Transform::IDENTITY );
   m_sourceSkeleton->calculateLocalToModel( m_sourceSkeleton->m_boneLocalMatrices.getRaw(), sourceModelPose.getRaw() );
   m_targetSkeleton->calculateLocalToModel( m_sourceSkeleton->m_boneLocalMatrices.getRaw(), targetModelPose.getRaw() );

   Array< bool > boneMapped( targetBonesCount );
   boneMapped.resize( targetBonesCount, false );

   int bestMatchIdx;
   Vector tmpDispl, sDir, tDir;
   FastFloat distSq, bestWeight;

   uint mappedBonesCount = 0;
   for ( uint sourceBoneIdx = 0; sourceBoneIdx < sourceBonesCount; ++sourceBoneIdx )
   {
      const Transform& sourceTransform = sourceModelPose[sourceBoneIdx];

      bestWeight = Float_1;
      bestMatchIdx = -1;
      for ( uint targetBoneIdx = 0; targetBoneIdx < targetBonesCount; ++targetBoneIdx )
      {
         if ( boneMapped[targetBoneIdx] )
         {
            continue;
         }

         const Transform& targetTransform = targetModelPose[targetBoneIdx];
         tmpDispl.setSub( sourceTransform.m_translation, targetTransform.m_translation );
         distSq = tmpDispl.lengthSq();

         if ( distSq < bestWeight )
         {
            bestMatchIdx = targetBoneIdx;
            bestWeight = distSq;
         }
      }

      if ( bestMatchIdx >= 0 )
      {
         boneMapped[bestMatchIdx] = true;
         tmpMapping[sourceBoneIdx] = bestMatchIdx;
         ++mappedBonesCount;
      }
      else
      {
         // couldn't find a match for this bone
      }
   }

   buildMappingTable( tmpMapping, mappedBonesCount );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapByName()
{
   const uint sourceBonesCount = m_sourceSkeleton->getBoneCount();
   const uint targetBonesCount = m_targetSkeleton->getBoneCount();

   Array<int> tmpMapping( sourceBonesCount );
   tmpMapping.resize( sourceBonesCount, -1 );

   Array< bool > boneMapped( targetBonesCount );
   boneMapped.resize( targetBonesCount, false );

   uint mappedBonesCount = 0;
   for ( uint sourceBoneIdx = 0; sourceBoneIdx < sourceBonesCount; ++sourceBoneIdx )
   {
      const std::string& sourceName = m_sourceSkeleton->m_boneNames[sourceBoneIdx];

      for ( uint targetBoneIdx = 0; targetBoneIdx < targetBonesCount; ++targetBoneIdx )
      {
         if ( boneMapped[targetBoneIdx] )
         {
            continue;
         }

         const std::string& targetName = m_targetSkeleton->m_boneNames[targetBoneIdx];
         if ( sourceName == targetName )
         {
            // found a mapping
            boneMapped[targetBoneIdx] = true;
            tmpMapping[sourceBoneIdx] = targetBoneIdx;
            ++mappedBonesCount;
            break;
         }
      }
   }

   buildMappingTable( tmpMapping, mappedBonesCount );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::buildMappingTable( const Array< int >& sourceToTargetMapping, uint mappedBonesCount )
{
   const uint targetSkeletonBoneCount = m_targetSkeleton->getBoneCount();
   Array< bool > mappedTargetBones( targetSkeletonBoneCount );
   mappedTargetBones.resize( targetSkeletonBoneCount, false );

   const uint sourceBonesCount = sourceToTargetMapping.size();
   for ( uint i = 0; i < sourceBonesCount; ++i )
   {
      const int targetBoneIdx = sourceToTargetMapping[i];
      if ( targetBoneIdx >= 0 )
      {
         mappedTargetBones[targetBoneIdx] = true;
      }
   }

   // copy the mapped values to the actual mapping array, skipping all unmapped bones
   m_boneChains.resize( mappedBonesCount, BoneChain() );
   uint mappingIdx = 0;
   for ( uint i = 0; i < sourceBonesCount; ++i )
   {
      const int targetBoneIdx = sourceToTargetMapping[i];
      if ( targetBoneIdx < 0 )
      {
         continue;
      }

      BoneChain& chain = m_boneChains[mappingIdx];
      ++mappingIdx;

      // build the source chain
      chain.m_sourceBones.push_back( i );
      int parentBoneIdx = m_sourceSkeleton->m_boneParentIndices[i];
      while ( parentBoneIdx >= 0 && sourceToTargetMapping[parentBoneIdx] < 0 ) // keep on mapping until there are parents that are not mapped to any bone
      {
         chain.m_sourceBones.push_back( parentBoneIdx );
         parentBoneIdx = m_sourceSkeleton->m_boneParentIndices[parentBoneIdx];
      }

      // build the target chain
      chain.m_targetBones.push_back( targetBoneIdx );
      parentBoneIdx = m_targetSkeleton->m_boneParentIndices[targetBoneIdx];
      while ( parentBoneIdx >= 0 && !mappedTargetBones[parentBoneIdx] ) // keep on mapping until there are parents that are not mapped to any bone
      {     
         chain.m_targetBones.push_back( parentBoneIdx );
         parentBoneIdx = m_targetSkeleton->m_boneParentIndices[parentBoneIdx];
      }

      // define the chain type
      if ( chain.m_sourceBones.size() > 1 && chain.m_targetBones.size() > 1 )
      {
         chain.m_type = BoneChain::ManyToMany;
      }
      else if ( chain.m_sourceBones.size() > 1 && chain.m_targetBones.size() == 1 )
      {
         chain.m_type = BoneChain::ManyToOne;
      }
      else if ( chain.m_sourceBones.size() == 1 && chain.m_targetBones.size() > 1 )
      {
         chain.m_type = BoneChain::OneToMany;
      }
      else if ( chain.m_sourceBones.size() == 1 && chain.m_targetBones.size() == 1 )
      {
         chain.m_type = BoneChain::OneToOne;
      }
      else
      {
         ASSERT_MSG( false, "Invalid bone chain" );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::calcPoseLocalSpace( const Transform* sourcePoseLocalSpace, Transform* outTargetPoseLocalSpace ) const
{
   static SkeletonMapper::MappingFunc mappingFuncs[4] = {
      &SkeletonMapper::mapOneToOne,
      &SkeletonMapper::mapOneToMany,
      &SkeletonMapper::mapManyToOne,
      &SkeletonMapper::mapManyToMany
   };

   Transform sourceTransform;

   const uint chainsCount = m_boneChains.size();
   for ( uint i = 0; i < chainsCount; ++i )
   {
      const BoneChain& chain = m_boneChains[i];

      SkeletonMapper::MappingFunc mappingFunc = mappingFuncs[chain.m_type];
      (this->*mappingFunc)( chain, sourcePoseLocalSpace, outTargetPoseLocalSpace );      
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapOneToOne( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const
{
   const uint sourceBoneIdx = chain.m_sourceBones[0];
   const uint targetBoneIdx = chain.m_targetBones[0];
   outTargetPose[targetBoneIdx] = sourcePose[sourceBoneIdx];
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapOneToMany( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const
{
   // subtract all unmapped bones from the chain from the source bone transform
   const uint targetBonesCount = chain.m_targetBones.size();
   Transform t, a;
   for ( uint i = 1; i < targetBonesCount; ++i )
   {
      const uint targetBoneIdx = chain.m_targetBones[i];
      a.set( m_targetSkeleton->m_boneLocalMatrices[targetBoneIdx] );
      a.invert();
      t.mul( a );
   }

   const uint sourceBoneIdx = chain.m_sourceBones[0];
   t.preMul( sourcePose[sourceBoneIdx] );

   // and set this transform as the target bone's local space transform
   const uint mappedTargetBoneIdx = chain.m_targetBones[0];
   outTargetPose[mappedTargetBoneIdx] = t;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapManyToOne( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const
{
   // calculate the entire offset of the source chain
   const uint sourceBonesCount = chain.m_sourceBones.size();
   Transform t;
   for ( uint i = 0; i < sourceBonesCount; ++i )
   {
      const uint sourceBoneIdx = chain.m_sourceBones[i];
      t.mul( sourcePose[sourceBoneIdx] );
   }

   // and set it as the target bone's local space transform
   const uint targetBoneIdx = chain.m_targetBones[0];
   outTargetPose[targetBoneIdx] = t;

}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapManyToMany( const BoneChain& chain, const Transform* sourcePose, Transform* outTargetPose ) const
{
   // calculate the entire offset of the source chain
   const uint sourceBonesCount = chain.m_sourceBones.size();
   Transform tSource;
   for ( uint i = 0; i < sourceBonesCount; ++i )
   {
      const uint sourceBoneIdx = chain.m_sourceBones[i];
      tSource.mul( sourcePose[sourceBoneIdx] );
   }

   // subtract all unmapped bones from the chain from the source bone transform
   const uint targetBonesCount = chain.m_targetBones.size();
   Transform tTarget, a;
   for ( uint i = 1; i < targetBonesCount; ++i )
   {
      const uint targetBoneIdx = chain.m_targetBones[i];
      a.set( m_targetSkeleton->m_boneLocalMatrices[targetBoneIdx] );
      a.invert();
      tTarget.mul( a );
   }

   tTarget.preMul( tSource );

   // and set this transform as the target bone's local space transform
   const uint mappedTargetBoneIdx = chain.m_targetBones[0];
   outTargetPose[mappedTargetBoneIdx] = tTarget;

}

///////////////////////////////////////////////////////////////////////////////