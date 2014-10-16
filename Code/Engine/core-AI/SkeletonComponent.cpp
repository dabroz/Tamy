#include "core-AI\SkeletonComponent.h"
#include "core-AI\Skeleton.h"
#include "core\Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonComponent );
   PARENT( Component );
   PROPERTY_EDIT( "Skeleton", Skeleton*, m_skeleton );
   PROPERTY_EDIT( "Local matrix", Matrix, m_skeletonLocalMtx );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonComponent::SkeletonComponent( const char* name )
   : Component( name )
   , m_skeleton( NULL )
   , m_skeletonLocalMtx( Matrix::IDENTITY )
   , m_boneLocalMtx( NULL )
   , m_boneModelMtx( NULL )
   , m_boneGlobalMtx( NULL )
   , m_skinningMtx( NULL )
   , m_parentModelMtxPtr( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonComponent::SkeletonComponent( const SkeletonComponent& rhs )
   : Component( rhs )
   , m_skeleton( rhs.m_skeleton )
   , m_skeletonLocalMtx( rhs.m_skeletonLocalMtx )
   , m_boneLocalMtx( NULL )
   , m_boneModelMtx( NULL )
   , m_boneGlobalMtx( NULL )
   , m_skinningMtx( NULL )
   , m_parentModelMtxPtr( NULL )
{
   initializeSkeletonInstance();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonComponent::~SkeletonComponent()
{
   deinitializeSkeletonInstance();
   m_skeleton = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::setSkeleton( Skeleton* skeleton )
{
   NOTIFY_PROPERTY_CHANGE( m_skeleton );
   
   m_skeleton = skeleton;
}


///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::setLocalMtx( const Matrix& mtx )
{
   NOTIFY_PROPERTY_CHANGE( m_skeletonLocalMtx );

   m_skeletonLocalMtx = mtx;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::onObjectLoaded()
{
   Component::onObjectLoaded();

   initializeSkeletonInstance();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::onPrePropertyChanged( ReflectionProperty& property )
{
   Component::onPrePropertyChanged( property );

   if ( property.getName() == "m_skeleton" )
   {
      deinitializeSkeletonInstance();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::onPropertyChanged( ReflectionProperty& property )
{
   Component::onPropertyChanged( property );

   if ( property.getName() == "m_skeleton" )
   {
      initializeSkeletonInstance();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::initializeSkeletonInstance()
{
   if ( !m_skeleton )
   {
      return;
   }
   ASSERT( m_boneLocalMtx == NULL );

   uint bonesCount = m_skeleton->getBoneCount();
   m_boneLocalMtx = new Matrix[bonesCount];
   m_boneModelMtx = new Matrix[bonesCount];
   m_boneGlobalMtx = new Matrix[bonesCount];
   m_skinningMtx = new Matrix[bonesCount];
   m_parentModelMtxPtr = new MatrixPtr[bonesCount];

   for ( uint i = 0; i < bonesCount; ++i )
   {
      m_boneLocalMtx[i] = m_skeleton->m_boneLocalMatrices[i];

      int parentBoneIdx = m_skeleton->m_boneParentIndices[i];
      MatrixPtr parentModelMtx = parentBoneIdx < 0 ? &Matrix::IDENTITY : &m_boneModelMtx[parentBoneIdx];
      m_parentModelMtxPtr[i] = parentModelMtx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::deinitializeSkeletonInstance()
{
   if ( !m_skeleton )
   {
      return;
   }
   ASSERT( m_boneLocalMtx != NULL );

   delete [] m_boneLocalMtx;
   m_boneLocalMtx = NULL;

   delete [] m_boneModelMtx;
   m_boneModelMtx = NULL;

   delete [] m_boneGlobalMtx;
   m_boneGlobalMtx = NULL;

   delete [] m_skinningMtx;
   m_skinningMtx = NULL;

   delete [] m_parentModelMtxPtr;
   m_parentModelMtxPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::updateTransforms()
{
   Component::updateTransforms();

   if ( !m_skeleton )
   {
      return;
   }

   const Matrix& entityGlobalMtx = getGlobalMtx();

   Matrix globalMtx = m_skeletonLocalMtx;
   globalMtx.setMul( entityGlobalMtx, m_skeletonLocalMtx );

   uint bonesCount = m_skeleton->getBoneCount();
   for ( uint i = 0; i < bonesCount; ++i )
   {
      int orderedBoneIdx = m_skeleton->m_bonesUpdateOrder[i];

      // update model and global matrices
      Matrix& modelMtx = m_boneModelMtx[orderedBoneIdx];
      modelMtx.setMul( m_boneLocalMtx[orderedBoneIdx], *m_parentModelMtxPtr[orderedBoneIdx] );
      m_boneGlobalMtx[orderedBoneIdx].setMul( modelMtx, entityGlobalMtx );

      // update the skinning matrices
      Matrix skinningMtx;
      skinningMtx.setMul( m_skeleton->m_boneInvBindPoseMtx[orderedBoneIdx], modelMtx );
      m_skinningMtx[orderedBoneIdx] = skinningMtx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonComponent::resetToTPose()
{
   if ( m_skeleton )
   {
      ASSERT( m_boneLocalMtx != NULL );

      uint bonesCount = m_skeleton->getBoneCount();
      for ( uint i = 0; i < bonesCount; ++i )
      {
         m_boneLocalMtx[i] = m_skeleton->m_boneLocalMatrices[i];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
