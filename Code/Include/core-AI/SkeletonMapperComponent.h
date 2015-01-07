/// @file   core-AI\SkeletonMapperComponent.h
/// @brief  a skeleton mapper runtime
#pragma once

#include "core-MVC\Component.h"
#include "core\Array.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperComponent : public Component
{
   DECLARE_ALLOCATOR( SkeletonMapperComponent, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   SkeletonMapper*                  m_mapper;

   // runtime data
   bool                             m_isInitialized;

   Skeleton*                        m_sourceChainSkeleton;
   Skeleton*                        m_targetChainSkeleton;

   mutable Array< Transform >       m_tmpSourceBasePose;
   mutable Array< Transform >       m_tmpSourceChainPose;
   mutable Array< Transform >       m_tmpTargetChainPose;
   mutable Array< Transform >       m_tmpTargetBasePose;

   Array< Transform >               m_targetBindPose;
   Array< Transform >               m_targetToSource;
   Array< Transform >               m_sourceToTarget;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   SkeletonMapperComponent( const char* name = "SkeletonMapperComponent" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   SkeletonMapperComponent( const SkeletonMapperComponent& rhs );
   ~SkeletonMapperComponent();

   /**
    * Sets a mapper instance.
    */
   void setMapper( SkeletonMapper* mapper );

   /**
    * Calculates a pose the target skeleton should assume to look the same as the source.
    *
    * @param sourcePose
    * @param outTargetPose
    */
   void calcPoseLocalSpace( const Transform* sourcePose, Transform* outTargetPose ) const;

   /**
    * Tells if the runtime is initialized.
    */
   inline bool isInitialized() const {
      return m_isInitialized;
   }

   // -------------------------------------------------------------------------
   // Component implementation
   // -------------------------------------------------------------------------
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;

   // -------------------------------------------------------------------------
   // Runtime management.
   // -------------------------------------------------------------------------
   /**
    * Compiles the runtime representation of the mapper.
    */
   void compileRuntime();

   /**
    * Destroys the runtime representation of the mapper.
    */
   void destroyRuntime();
};

///////////////////////////////////////////////////////////////////////////////
