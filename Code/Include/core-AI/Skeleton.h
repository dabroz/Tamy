/// @file   core-AI\Skeleton.h
/// @brief  a resource with a skeletal structure of a model
#pragma once

#include <string>
#include "core\Resource.h"
#include "core\Matrix.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A skeleton represents a hierarchy of nodes along with
 * a set of animations that operate on it
 */
class Skeleton : public Resource
{
   DECLARE_ALLOCATOR( Skeleton, AM_ALIGNED_16 );
   DECLARE_RESOURCE()

public:
   // static data
   Array< std::string >          m_boneNames;
   Array< Matrix >               m_boneLocalMatrices;
   Array< Matrix >               m_boneInvBindPoseMtx;
   Array< int >                  m_boneParentIndices;
   Array< FastFloat >            m_boneLengths;

   // runtime data
   uint*                         m_bonesUpdateOrder;

public:  
   /**
    * Constructor.
    *
    * @param resourceName
    */
   Skeleton( const FilePath& resourceName = FilePath() );
   ~Skeleton();

   /**
    * Adds a new bone to the skeleton.
    *
    * @param boneName
    * @param localMtx
    * @param parentBoneIdx    >= 0 if the bone has a parent, -1 if its a root bone
    * @param boneLength       length of the bone
    */
   void addBone( const char* boneName, const Matrix& localMtx, int parentBoneIdx, float boneLength );

   /**
    * Clears the skeleton of all data.
    */
   void clear();

   /**
    * Call this if you made any changes to the skeletal structure ( such as introducing new bones etc ).
    */
   void buildSkeleton();

   /**
    * Returns the number of bones in the skeleton.
    */
   inline uint getBoneCount() const { return m_boneNames.size(); }
  
   /**
    * Returns the index of a bone with the specified name, or -1 if the skeleton doesn't contain such bone.
    *
    * @param name
    */
   int getBoneIndex( const char* name ) const;

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& mgr );

private:
   void sortBones();
   void calculateBindPose();
};

///////////////////////////////////////////////////////////////////////////////
