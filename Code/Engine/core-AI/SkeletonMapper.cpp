#include "core-AI\SkeletonMapper.h"
#include "core-AI\Skeleton.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonMapper );
   PARENT( ReflectionObject );
   PROPERTY( Skeleton*, m_sourceSkeleton );
   PROPERTY( Skeleton*, m_targetSkeleton );
   PROPERTY( Array< int >, m_mapping );
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
   m_mapping.clear();

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
   m_mapping.resize( bonesCount * 2, 0 );
   for ( uint i = 0; i < bonesCount; ++i )
   {
      m_mapping[i * 2] = i;
      m_mapping[i * 2 + 1] = i;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::mapByDistance()
{
   const uint sourceBonesCount = m_sourceSkeleton->getBoneCount();
   const uint targetBonesCount = m_targetSkeleton->getBoneCount();

   Array<int> tmpMapping( sourceBonesCount );
   tmpMapping.resize( sourceBonesCount, -1 );

   Array< Matrix > sourceModelPose( sourceBonesCount );
   Array< Matrix > targetModelPose( targetBonesCount );
   m_sourceSkeleton->calculateModelPose( sourceModelPose );
   m_targetSkeleton->calculateModelPose( targetModelPose );

   Array< bool > boneMapped( targetBonesCount );
   boneMapped.resize( targetBonesCount, false );

   int bestMatchIdx;
   Vector tmpDispl, sDir, tDir;
   FastFloat distSq, bestWeight;

   uint mappedBonesCount = 0;
   for ( uint sourceBoneIdx = 0; sourceBoneIdx < sourceBonesCount; ++sourceBoneIdx )
   {
      const Matrix& sourceTransform = sourceModelPose[sourceBoneIdx];

      bestWeight = Float_1;
      bestMatchIdx = -1;
      for ( uint targetBoneIdx = 0; targetBoneIdx < targetBonesCount; ++targetBoneIdx )
      {
         if ( boneMapped[targetBoneIdx] )
         {
            continue;
         }

         const Matrix& targetTransform = targetModelPose[targetBoneIdx];
         tmpDispl.setSub( sourceTransform.position(), targetTransform.position() );
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
   // copy the mapped values to the actual mapping array, skipping all unmapped bones
   const uint sourceBonesCount = sourceToTargetMapping.size();
   m_mapping.resize( mappedBonesCount * 2, 0 );
   uint idx = 0;
   for ( uint i = 0; i < sourceBonesCount; ++i )
   {
      if ( sourceToTargetMapping[i] >= 0 )
      {
         m_mapping[idx++] = i;
         m_mapping[idx++] = sourceToTargetMapping[i];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapper::setPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const
{
   const uint mappingsCount = m_mapping.size() / 2;
   
   const uint* mapping = m_mapping.getRaw();
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      const uint sourceBoneIdx = *mapping; ++mapping;
      const uint targetBoneIdx = *mapping; ++mapping;

      outTargetPose[targetBoneIdx] = sourcePose[sourceBoneIdx];
   }
}

///////////////////////////////////////////////////////////////////////////////
