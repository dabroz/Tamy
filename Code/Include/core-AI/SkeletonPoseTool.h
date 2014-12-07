/// @file   core-AI\SkeletonPoseTool.h
/// @brief  a tool for working with skeleton poses
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Transform.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class SkeletonPoseTool
{
   DECLARE_ALLOCATOR( SkeletonPoseTool, AM_DEFAULT );

private:
   Skeleton&                           m_skeleton;

   List< std::string >                 m_boneParents;

   mutable Array< Transform >          m_poseLocal;
   mutable Array< Transform >          m_poseModel;

   mutable bool                        m_localDirty;
   mutable bool                        m_modelDirty;

   std::string                         m_errorMsg;

public:
   /**
    * Constructor.
    *
    * @param skeleton
    */
   SkeletonPoseTool( Skeleton& skeleton );

   // -------------------------------------------------------------------------
   // Skeleton building building
   // -------------------------------------------------------------------------
   
   /**
    * Starts the skeleton building process.
    */
   SkeletonPoseTool& startSkeleton( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation );

   /**
    * Adds a new bone to the skeleton.
    *
    * @param 
    */
   SkeletonPoseTool& bone( const char* boneName, const char* parentBoneName, const Vector& rotationAxis, float angle, const Vector& translation );

   /**
    * Finishes the skeleton building process.
    */
   void buildSkeleton();

   // -------------------------------------------------------------------------
   // Pose building
   // -------------------------------------------------------------------------
   
   /**
    * Starts the pose building process.
    */
   SkeletonPoseTool& start();

   /**
    * Changes bone's rotation, leaving its translation unaffected.
    *
    * @param boneName
    * @param rotationAxis
    * @param angle
    */
   SkeletonPoseTool& rotate( const char* boneName, const Vector& rotationAxis, float angle );

   /**
    * Changes bone's translation, leaving its rotation unaffected.
    *
    * @param boneName
    * @param translation
    */
   SkeletonPoseTool& translate( const char* boneName, const Vector& translation );

   /**
   * Changes bone's rotation and translation.
   *
   * @param boneName
   * @param rotationAxis
   * @param angle
   * @param translation
   */
   SkeletonPoseTool& rotateAndTranslate( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation );

   /**
    * Finishes the pose building process, compiling the pose.
    */
   void end();

   // -------------------------------------------------------------------------
   // Pose access
   // -------------------------------------------------------------------------

   /**
    * Returns the pose, where the bone transforms are defined in local space.
    */
   const Transform* getLocal() const;

   /**
    * Returns the pose, where the bone transforms are defined in local space ( non-const ).
    */
   Transform* accessLocal();

   /**
    * Returns the pose, where the bone transforms are defined in model space.
    */
   const Transform* getModel() const;

   /**
    * Returns the pose, where the bone transforms are defined in model space ( non-const ).
    */
   Transform* accessModel();

   // -------------------------------------------------------------------------
   // Pose testing
   // -------------------------------------------------------------------------

   /**
    * Tests the bone's transform in model space.
    * In addition to returning the test result, the method will set an error message if the transforms don't match.
    * The message can be retrieved using 'getErrorMsg' method.
    *
    * @param boneName
    * @param rotationAxis
    * @param angle
    * @param translation
    */
   bool testBoneModel( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation );

   /**
    * Returns the last issues error message.
    */
   inline const char* getErrorMsg() const {
      return m_errorMsg.c_str();
   }

private:
   void syncPoses() const;
};

///////////////////////////////////////////////////////////////////////////////
