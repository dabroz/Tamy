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
   , m_bonesUpdateOrder( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Skeleton::~Skeleton()
{
   delete [] m_bonesUpdateOrder;
   m_bonesUpdateOrder = NULL;
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

void Skeleton::clear()
{
   delete [] m_bonesUpdateOrder;
   m_bonesUpdateOrder = NULL;

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
   delete [] m_bonesUpdateOrder;
   m_bonesUpdateOrder = NULL;

   // Do a breadth-first search on the tree and linearize the list of bones
   // We assume that the skeleton's not ill defined and that there are no cycles in it.

   // First we need to build a tree structure that describes our skeleton
   struct BoneDef
   {
      LocalList< uint >     m_children;
   };

   LocalList< uint > bonesToCheck;
   uint bonesCount = m_boneNames.size();
   BoneDef* bonesTree = NULL;
   {  
      bonesTree = new BoneDef[bonesCount];

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
   m_bonesUpdateOrder = new uint[bonesCount];
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

   // cleanup
   delete [] bonesTree;
}

///////////////////////////////////////////////////////////////////////////////

void Skeleton::calculateBindPose()
{
   uint bonesCount = m_boneLocalMatrices.size();

   Matrix* boneModelMtx = new Matrix[bonesCount];

   // calculate the model pose
   for ( uint i = 0; i < bonesCount; ++i )
   {
      int orderedBoneIdx = m_bonesUpdateOrder[i];

      int parentBoneIdx = m_boneParentIndices[orderedBoneIdx];
      const Matrix& parentModelMtx = parentBoneIdx < 0 ? Matrix::IDENTITY : boneModelMtx[ parentBoneIdx ];

      Matrix& modelMtx = boneModelMtx[orderedBoneIdx];
      modelMtx.setMul( m_boneLocalMatrices[orderedBoneIdx], parentModelMtx );
   }

   // calculate the inverse of the model pose
   for ( uint i = 0; i < bonesCount; ++i )
   {
      m_boneInvBindPoseMtx[i].setInverse( boneModelMtx[i] );
   }

   delete [] boneModelMtx;
}

///////////////////////////////////////////////////////////////////////////////

int Skeleton::getBoneIndex( const char* name ) const
{
   uint count = m_boneNames.size();
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
