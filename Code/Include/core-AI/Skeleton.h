/// @file   core-AI\Skeleton.h
/// @brief  a resource with a skeletal structure of a model
#pragma once

#include <string>
#include "core\Resource.h"
#include "core\Matrix.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

struct Transform;

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
   Array< uint >                 m_bonesUpdateOrder;

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
    * Adds a new bone to the skeleton ( accomplishes the same thing as the method above, just with the use of a Transform ).
    *
    * @param boneName
    * @param localTransform
    * @param parentBoneIdx    >= 0 if the bone has a parent, -1 if its a root bone
    * @param boneLength       length of the bone
    */
   void addBone( const char* boneName, const Transform& localTransform, int parentBoneIdx, float boneLength );

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
   // Pose management utilities
   // -------------------------------------------------------------------------
   /**
    * Calculates the model pose of this skeleton based on the specified local pose.
    *
    * @param inPoseLocalSpace
    * @param outPoseModelSpace
    */
   void calculateLocalToModel( const Matrix* inPoseLocalSpace, Transform* outPoseModelSpace ) const;

   /**
    * Calculates the model pose of this skeleton based on the specified local pose.    
    *
    * @param inPoseLocalSpace
    * @param outPoseModelSpace
    */
   void calculateLocalToModel( const Transform* inPoseLocalSpace, Transform* outPoseModelSpace ) const;

   /**
    * Calculates the local pose of this skeleton based on the specified model pose.    
    *
    * @param inPoseLocalSpace
    * @param outPoseModelSpace
    */
   void calculateModelToLocal( const Transform* inPoseModelSpace, Transform* outPoseLocalSpace ) const;

   /**
    * Calculates the transform deviations of bones in the specified model space pose from the bind pose.
    *
    * @param inPoseModelSpace
    * @param outDeviations
    */
   void calculateTransformDeviations( const Transform* inPoseModelSpace, Transform* outDeviations ) const;

   /**
    * Applies the bone transform deviations to the bind pose, producing a new pose in local space.
    *
    * @param inDeviations
    * @param outPoseLocalSpace
    */
   void applyTransformDeviations( const Transform* inDeviations, Transform* outPoseLocalSpace ) const;

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
