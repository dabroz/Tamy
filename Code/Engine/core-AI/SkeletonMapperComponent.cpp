#include "core-AI\SkeletonMapperComponent.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"
#include "core-AI\Skeleton.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonMapperComponent );
   PARENT( Component );
   PROPERTY( SkeletonMapper*, m_mapper );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperComponent::SkeletonMapperComponent( const char* name )
   : Component( name )
   , m_mapper( NULL )
   , m_isInitialized( false )
{
   m_sourceChainSkeleton = new Skeleton();
   m_targetChainSkeleton = new Skeleton();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperComponent::SkeletonMapperComponent( const SkeletonMapperComponent& rhs )
   : Component( rhs )
   , m_mapper( rhs.m_mapper )
   , m_isInitialized( false )
{
   m_sourceChainSkeleton = new Skeleton();
   m_targetChainSkeleton = new Skeleton();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonMapperComponent::~SkeletonMapperComponent()
{
   m_sourceChainSkeleton->removeReference();
   m_sourceChainSkeleton = NULL;

   m_targetChainSkeleton->removeReference();
   m_targetChainSkeleton = NULL;

   m_mapper = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::setMapper( SkeletonMapper* mapper )
{
   m_mapper = mapper;

   if ( m_isInitialized )
   {
      // recompile the mapper
      destroyRuntime();
      compileRuntime();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::onAttachToModel( Model* model )
{
   Component::onAttachToModel( model );
   compileRuntime();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::onDetachFromModel( Model* model )
{
   Component::onDetachFromModel( model );
   destroyRuntime();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::calcPoseLocalSpace( const Transform* sourcePoseLocalSpace, Transform* outTargetPoseLocalSpace ) const
{
   if ( !m_isInitialized )
   {
      // the mapper hasn't been initialized
      return;
   }

   // calculate the model pose of the base skeleton and apply it to the source chain skeleton
   m_mapper->m_sourceSkeleton->calculateLocalToModel( sourcePoseLocalSpace, m_tmpSourceBasePose.getRaw() );
   const uint sourceChainsCount = m_mapper->m_sourceChains.size();
   for ( uint i = 0; i < sourceChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_mapper->m_sourceChains[i];
      m_tmpSourceChainPose[i] = m_tmpSourceBasePose[chain->m_firstBoneIdx];
   }

   // map the transforms of chains
   const uint mappingsCount = m_mapper->m_chainMappings.size();
   for ( uint targetChainIdx = 0; targetChainIdx < mappingsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_mapper->m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      // here's the juice:     
      Transform dtSourceSpace;
      {
         Transform invBindPose, bindPose;
         invBindPose.set( m_sourceChainSkeleton->m_boneInvBindPoseMtx[sourceChainIdx] );
         dtSourceSpace.setMul( invBindPose, m_tmpSourceChainPose[sourceChainIdx] );
      }

      m_tmpTargetChainPose[targetChainIdx].setMul( m_targetBindPose[targetChainIdx], dtSourceSpace );
   }

   // map the target chain to the base target skeleton
   const uint targetChainsCount = m_mapper->m_targetChains.size();
   for ( uint i = 0; i < targetChainsCount; ++i )
   {
      SkeletonBoneChain* chain = m_mapper->m_targetChains[i];
      chain->updatePose( m_tmpTargetChainPose[i], m_tmpTargetBasePose.getRaw() );
   }

   // back to the local pose
   m_mapper->m_targetSkeleton->calculateModelToLocal( m_tmpTargetBasePose.getRaw(), outTargetPoseLocalSpace );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::compileRuntime()
{
   if ( !m_mapper )
   {
      return;
   }

   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();
   SkeletonMapperUtils::buildChainSkeleton( m_mapper->m_sourceSkeleton, m_mapper->m_sourceChains, m_sourceChainSkeleton );
   SkeletonMapperUtils::buildChainSkeleton( m_mapper->m_targetSkeleton, m_mapper->m_targetChains, m_targetChainSkeleton );

   const uint sourceChainsCount = m_sourceChainSkeleton->getBoneCount();
   const uint targetChainsCount = m_targetChainSkeleton->getBoneCount();

   m_tmpSourceBasePose.resize( m_mapper->m_sourceSkeleton->getBoneCount(), Transform::IDENTITY );
   m_tmpSourceChainPose.resize( sourceChainsCount, Transform::IDENTITY );
   m_tmpTargetChainPose.resize( targetChainsCount, Transform::IDENTITY );
   m_tmpTargetBasePose.resize( m_mapper->m_targetSkeleton->getBoneCount(), Transform::IDENTITY );

   // allocate and calculate the transition matrices between the two chain skeletons
   m_targetBindPose.resize( targetChainsCount, Transform::IDENTITY );
   m_targetToSource.resize( targetChainsCount, Transform::IDENTITY );
   m_sourceToTarget.resize( targetChainsCount, Transform::IDENTITY );

   m_sourceChainSkeleton->calculateLocalToModel( m_sourceChainSkeleton->m_boneLocalMatrices.getRaw(), m_tmpSourceChainPose.getRaw() );
   m_targetChainSkeleton->calculateLocalToModel( m_targetChainSkeleton->m_boneLocalMatrices.getRaw(), m_targetBindPose.getRaw() );

   for ( uint targetChainIdx = 0; targetChainIdx< targetChainsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = m_mapper->m_chainMappings[targetChainIdx];
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      m_targetToSource[targetChainIdx].setMulInverse( m_targetBindPose[targetChainIdx], m_tmpSourceChainPose[sourceChainIdx] );
      m_sourceToTarget[targetChainIdx].setMulInverse( m_tmpSourceChainPose[sourceChainIdx], m_targetBindPose[targetChainIdx] );
   }

   m_isInitialized = true;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonMapperComponent::destroyRuntime()
{
   m_isInitialized = false;

   m_tmpSourceBasePose.clear();
   m_tmpSourceChainPose.clear();
   m_tmpTargetChainPose.clear();
   m_tmpTargetBasePose.clear();
   m_targetBindPose.clear();
   m_targetToSource.clear();
   m_sourceToTarget.clear();

   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();
}

///////////////////////////////////////////////////////////////////////////////
