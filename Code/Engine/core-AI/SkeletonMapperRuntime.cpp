#include "core-AI\SkeletonMapperRuntime.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\Skeleton.h"



///////////////////////////////////////////////////////////////////////////////

SkeletonMapperRuntime::SkeletonMapperRuntime( const SkeletonMapper& mapper )
   : m_mapper( mapper )
{
   m_sourceChainSkeleton = new Skeleton();
   m_targetChainSkeleton = new Skeleton();

   compileRuntime();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperRuntime::~SkeletonMapperRuntime()
{
   destroyRuntime();

   m_sourceChainSkeleton->removeReference();
   m_sourceChainSkeleton = NULL;

   m_targetChainSkeleton->removeReference();
   m_targetChainSkeleton = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperRuntime::calcPoseLocalSpace( const Transform* sourcePoseLocalSpace, Transform* outTargetPoseLocalSpace ) const
{
   // calculate the model pose of the base skeleton and apply it to the source chain skeleton
   m_mapper.m_sourceSkeleton->calculateLocalToModel( sourcePoseLocalSpace, m_tmpSourceBasePose.getRaw() );
   const uint sourceChainsCount = m_mapper.m_sourceChains.size();
   for ( uint i = 0; i < sourceChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_mapper.m_sourceChains[i];
      chain->translatePoseToChainBoneTransform( m_tmpSourceBasePose.getRaw(), m_tmpSourceChainPose[i] );
   }
      
   // map the transforms of chains
   const uint mappingsCount = m_mapper.m_chainMappings.size();
   for ( uint targetChainIdx = 0; targetChainIdx < mappingsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_mapper.m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      // here's the juice - we're calculating how much did the source 'chain bone' move    
      Transform dtSourceSpace;
      {
         Transform invBindPose, bindPose;
         invBindPose.set( m_sourceChainSkeleton->m_boneInvBindPoseMtx[sourceChainIdx] );
         dtSourceSpace.setMul( invBindPose, m_tmpSourceChainPose[sourceChainIdx] );
      }

      // and we're applying the same movement to the corresponding target 'chain bone'
      m_tmpTargetChainPose[targetChainIdx].setMul( m_targetBindPose[targetChainIdx], dtSourceSpace );
   }

   // translate the movement of the target chains onto the movement of the target skeleton's bones
   const uint targetChainsCount = m_mapper.m_targetChains.size();
   for ( uint i = 0; i < targetChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_mapper.m_targetChains[i];
      chain->translateChainBoneTransformToPose( m_tmpTargetChainPose[i], m_tmpTargetBasePose.getRaw() );
   }

   // back to the local pose
   m_mapper.m_targetSkeleton->calculateModelToLocal( m_tmpTargetBasePose.getRaw(), outTargetPoseLocalSpace );
}

///////////////////////////////////////////////////////////////////////////////

Transform* SkeletonMapperRuntime::translatePose( const Transform* sourcePose )
{
   Transform* outTargetPose = m_outTargetPose.getRaw();

   calcPoseLocalSpace( sourcePose, outTargetPose );

   return outTargetPose;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperRuntime::compileRuntime()
{
   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();
   SkeletonMapperUtils::buildChainSkeleton( m_mapper.m_sourceSkeleton, m_mapper.m_sourceChains, m_sourceChainSkeleton );
   SkeletonMapperUtils::buildChainSkeleton( m_mapper.m_targetSkeleton, m_mapper.m_targetChains, m_targetChainSkeleton );

   const uint sourceChainsCount = m_sourceChainSkeleton->getBoneCount();
   const uint targetChainsCount = m_targetChainSkeleton->getBoneCount();

   m_tmpSourceBasePose.resize( m_mapper.m_sourceSkeleton->getBoneCount(), Transform::IDENTITY );
   m_tmpSourceChainPose.resize( sourceChainsCount, Transform::IDENTITY );
   m_tmpTargetChainPose.resize( targetChainsCount, Transform::IDENTITY );
   m_tmpTargetBasePose.resize( m_mapper.m_targetSkeleton->getBoneCount(), Transform::IDENTITY );
   m_outTargetPose.resize( m_mapper.m_targetSkeleton->getBoneCount(), Transform::IDENTITY );

   // allocate and calculate the transition matrices between the two chain skeletons
   m_targetBindPose.resize( targetChainsCount, Transform::IDENTITY );
   m_targetToSource.resize( targetChainsCount, Transform::IDENTITY );
   m_sourceToTarget.resize( targetChainsCount, Transform::IDENTITY );
   

   m_sourceChainSkeleton->calculateLocalToModel( m_sourceChainSkeleton->m_boneLocalMatrices.getRaw(), m_tmpSourceChainPose.getRaw() );
   m_targetChainSkeleton->calculateLocalToModel( m_targetChainSkeleton->m_boneLocalMatrices.getRaw(), m_targetBindPose.getRaw() );

   for ( uint targetChainIdx = 0; targetChainIdx< targetChainsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_mapper.m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      m_targetToSource[targetChainIdx].setMulInverse( m_targetBindPose[targetChainIdx], m_tmpSourceChainPose[sourceChainIdx] );
      m_sourceToTarget[targetChainIdx].setMulInverse( m_tmpSourceChainPose[sourceChainIdx], m_targetBindPose[targetChainIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperRuntime::destroyRuntime()
{
   m_tmpSourceBasePose.clear();
   m_tmpSourceChainPose.clear();
   m_tmpTargetChainPose.clear();
   m_tmpTargetBasePose.clear();
   m_targetBindPose.clear();
   m_targetToSource.clear();
   m_sourceToTarget.clear();
   m_outTargetPose.clear();

   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();
}

///////////////////////////////////////////////////////////////////////////////
