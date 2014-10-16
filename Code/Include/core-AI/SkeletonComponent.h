/// @file   core-AI/SkeletonComponent.h
/// @brief  animated entity
#pragma once

#include "core-MVC\Component.h"


///////////////////////////////////////////////////////////////////////////////

class Skeleton;
struct Matrix;

///////////////////////////////////////////////////////////////////////////////

/**
 * An entity with a skeleton that can be animated.
 */
class SkeletonComponent : public Component
{
   DECLARE_ALLOCATOR( SkeletonComponent, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   Skeleton*            m_skeleton;
   Matrix               m_skeletonLocalMtx;

   // runtime data
   Matrix*              m_boneLocalMtx;
   Matrix*              m_boneModelMtx;
   Matrix*              m_boneGlobalMtx;

   Matrix*              m_skinningMtx;

   typedef const Matrix*   MatrixPtr;
   MatrixPtr*           m_parentModelMtxPtr;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   SkeletonComponent( const char* name = "SkeletonComponent" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   SkeletonComponent( const SkeletonComponent& rhs );
   ~SkeletonComponent();

   /**
    * Sets a different local matrix, offsetting the skeleton instance from the entity's origin.
    *
    * @param mtx
    */
   void setLocalMtx( const Matrix& mtx );

   /**
    * Sets an instance of skeleton this entity should use.
    *
    * @param skeleton
    */
   void setSkeleton( Skeleton* skeleton );

   /**
    * Reset entity's pose to T-pose.
    */
   void resetToTPose();

   // ----------------------------------------------------------------------
   // Entity implementation
   // ----------------------------------------------------------------------
   void updateTransforms();
   void onPrePropertyChanged( ReflectionProperty& property );
   void onPropertyChanged( ReflectionProperty& property );
   void onObjectLoaded();

private:
   void initializeSkeletonInstance();
   void deinitializeSkeletonInstance();
};

///////////////////////////////////////////////////////////////////////////////
