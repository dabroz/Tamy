#include "core-AI\Skeleton.h"
#include "core\LocalList.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Skeleton, tsk, AM_BINARY );
   PROPERTY( Array< std::string >, m_boneNames );
   PROPERTY( Array< Matrix >, m_boneLocalMatrices );
   PROPERTY( Array< Matrix >, m_boneInvBindPoseMtx );
   PROPERTY( Array< int >, m_boneParentIndices );
   PROPERTY( Array< FastFloat >, m_boneLengths );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Skeleton::Skeleton( const FilePath& resourceName )
   : Resource( resourceName )
{
}

///////////////////////////////////////////////////////////////////////////////

Skeleton::~Skeleton()
{
   m_bonesUpdateOrder.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::replaceContents( Resource& rhs )
{
   Skeleton& rhsSkeleton = static_cast< Skeleton& >( rhs );
   m_boneNames = rhsSkeleton.m_boneNames;
   m_boneLocalMatrices = rhsSkeleton.m_boneLocalMatrices;
   m_boneInvBindPoseMtx = rhsSkeleton.m_boneInvBindPoseMtx;
   m_boneParentIndices = rhsSkeleton.m_boneParentIndices;
   m_boneLengths = rhsSkeleton.m_boneLengths;
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::onResourceLoaded( ResourcesManager& mgr )
{
   Resource::onResourceLoaded( mgr );

   sortBones();
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::addBone( const char* boneName, const Matrix& localMtx, int parentBoneIdx, float boneLength )
{
   m_boneNames.push_back( boneName );
   m_boneLocalMatrices.push_back( localMtx );
   m_boneInvBindPoseMtx.push_back( Matrix::IDENTITY );
   m_boneParentIndices.push_back( parentBoneIdx );
   m_boneLengths.push_back( FastFloat::fromFloat( boneLength ) );
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::addBone( const char* boneName, const Transform& localTransform, int parentBoneIdx, float boneLength )
{
   Matrix boneLocalMtx;
   localTransform.toMatrix( boneLocalMtx );
   addBone( boneName, boneLocalMtx, parentBoneIdx, boneLength );
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::clear()
{
   m_bonesUpdateOrder.clear();

   m_boneNames.clear();
   m_boneLocalMatrices.clear();
   m_boneInvBindPoseMtx.clear();
   m_boneParentIndices.clear();
   m_boneLengths.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::buildSkeleton()
{
   sortBones();
   calculateBindPose();
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::sortBones()
{
   m_bonesUpdateOrder.clear();

   // Do a breadth-first search on the tree and linearize the list of bones
   // We assume that the skeleton's not ill defined and that there are no cycles in it.

   // First we need to build a tree structure that describes our skeleton
   struct BoneDef
   {
      LocalList< uint >     m_children;
   };

   LocalList< uint > bonesToCheck;
   uint bonesCount = m_boneNames.size();
   Array< BoneDef > bonesTree;
   {  
      bonesTree.resize( bonesCount, BoneDef() );

      for ( uint oldBoneIdx = 0; oldBoneIdx < bonesCount; ++oldBoneIdx )
      {
         int oldParentIdx = m_boneParentIndices[oldBoneIdx];
         if( oldParentIdx >= 0 )
         {
            bonesTree[oldParentIdx].m_children.pushBack( oldBoneIdx );
         }
         else
         {
            // while we're at it - let's identify root bones and start our search from them
            bonesToCheck.pushBack( oldBoneIdx );
         }
      }
   }

   // now run the search and create a linear list of nodes - from the roots to the leaves
   m_bonesUpdateOrder.resize( bonesCount, 0 );
   {
      uint newBoneIdx = 0;

      while( !bonesToCheck.empty() )
      {
         uint oldBoneIdx = bonesToCheck.front();
         bonesToCheck.popFront();

         m_bonesUpdateOrder[newBoneIdx] = oldBoneIdx;
         ++newBoneIdx;

         const BoneDef& boneDef = bonesTree[oldBoneIdx];
         for ( LocalList< uint >::const_iterator it = boneDef.m_children.begin(); !it.isEnd(); ++it )
         {
            bonesToCheck.pushBack( *it );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateBindPose()
{
   const uint bonesCount = m_boneLocalMatrices.size();
   Array< Transform > boneModelMtx( bonesCount );
   boneModelMtx.resize( bonesCount, Transform::IDENTITY );

   calculateLocalToModel( m_boneLocalMatrices, boneModelMtx );

   // calculate the inverse of the model pose
   Transform invTransform;
   for ( uint i = 0; i < bonesCount; ++i )
   {
      invTransform.setInverse( boneModelMtx[i] );
      invTransform.toMatrix( m_boneInvBindPoseMtx[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

int Skeleton::getBoneIndex( const char* name ) const
{
   const uint count = m_boneNames.size();
   for ( uint idx = 0; idx < count; ++idx )
   {
      if ( m_boneNames[idx] == name )
      {
         return idx;
      }
   }
   return -1;
}

///////////////////////////////////////////////////////////////////////////////

bool Skeleton::verifyParentChildRelation( const std::string& parentBoneName, const std::string& childBoneName ) const
{
   int parentBoneIdx = getBoneIndex( parentBoneName.c_str() );
   int childBoneIdx = getBoneIndex( childBoneName.c_str() );

   if ( parentBoneIdx < 0 || childBoneIdx < 0 )
   {
      // one of the bones doesn't exist
      return false;
   }

   for ( int boneIdx = m_boneParentIndices[childBoneIdx]; boneIdx >= 0; boneIdx = m_boneParentIndices[boneIdx] )
   {
      if ( boneIdx == parentBoneIdx )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateLocalToModel( const Matrix* inPoseLocalSpace, Transform* outPoseModelSpace ) const
{
   const uint bonesCount = m_boneLocalMatrices.size();

   Transform boneLocalTransf;
   for ( uint i = 0; i < bonesCount; ++i )
   {
      const int orderedBoneIdx = m_bonesUpdateOrder[i];

      const int parentBoneIdx = m_boneParentIndices[orderedBoneIdx];
      const Transform& parentBoneModelSpace = parentBoneIdx < 0 ? Transform::IDENTITY : outPoseModelSpace[parentBoneIdx];

      Transform& boneModelSpace = outPoseModelSpace[orderedBoneIdx];
      boneLocalTransf.set( inPoseLocalSpace[orderedBoneIdx] );
      boneModelSpace.setMul( boneLocalTransf, parentBoneModelSpace );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateLocalToModel( const Transform* inPoseLocalSpace, Transform* outPoseModelSpace ) const
{
   const uint bonesCount = m_boneLocalMatrices.size();

   for ( uint i = 0; i < bonesCount; ++i )
   {
      const int orderedBoneIdx = m_bonesUpdateOrder[i];

      const int parentBoneIdx = m_boneParentIndices[orderedBoneIdx];
      const Transform& parentBoneModelSpace = parentBoneIdx < 0 ? Transform::IDENTITY : outPoseModelSpace[parentBoneIdx];

      Transform& boneModelSpace = outPoseModelSpace[orderedBoneIdx];
      boneModelSpace.setMul( inPoseLocalSpace[orderedBoneIdx], parentBoneModelSpace );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateModelToLocal( const Transform* inPoseModelSpace, Transform* outPoseLocalSpace ) const
{
   const uint bonesCount = m_boneLocalMatrices.size();

   for ( uint i = 0; i < bonesCount; ++i )
   {
      const int parentBoneIdx = m_boneParentIndices[i];
      const Transform& parentBoneModelSpace = parentBoneIdx < 0 ? Transform::IDENTITY : inPoseModelSpace[parentBoneIdx];

      Transform& boneLocalSpace = outPoseLocalSpace[i];
      boneLocalSpace.setMulInverse( inPoseModelSpace[i], parentBoneModelSpace );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateTransformDeviations( const Transform* inPoseModelSpace, Transform* outDeviations ) const
{
   const uint bonesCount = m_boneLocalMatrices.size();

   Transform a, b;
   for ( uint i = 0; i < bonesCount; ++i )
   {
      const int parentBoneIdx = m_boneParentIndices[i];
      if ( parentBoneIdx >= 0 )
      {
         a.setMulInverse( inPoseModelSpace[i], inPoseModelSpace[parentBoneIdx] );
      }
      else
      {
         a = inPoseModelSpace[i];
      }

      b.set( m_boneLocalMatrices[i] );      
      outDeviations[i].setMulInverse( a, b );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::applyTransformDeviations( const Transform* inDeviations, Transform* outPoseLocalSpace ) const
{
   const uint bonesCount = m_boneLocalMatrices.size();

   Transform bindPoseTransform;
   for ( uint i = 0; i < bonesCount; ++i )
   {
      bindPoseTransform.set( m_boneLocalMatrices[i] );

      outPoseLocalSpace[i].setMul( inDeviations[i], bindPoseTransform );
   }
}

///////////////////////////////////////////////////////////////////////////////
