/// @file   core-AI/SkeletonAnimationPlayer.h
/// @brief  an entity that animates a SkeletonComponent it's attached to
#pragma once

#include "core-AI\AnimationPlayer.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class SnapshotAnimation;
class SkeletonComponent;
class Entity;
struct Transform;

///////////////////////////////////////////////////////////////////////////////

/**
 * This entity that animates a SkeletonComponent it's attached to.
 */
class SkeletonAnimationPlayer : public AnimationPlayer
{
   DECLARE_ALLOCATOR( SkeletonAnimationPlayer, AM_DEFAULT );
   DECLARE_CLASS()

private:
   // static data
   SnapshotAnimation*                        m_source;

   // runtime data
   SkeletonComponent*                        m_posesSink;
   Entity*                                   m_motionExtractionSink;
   Transform*                                m_tmpBoneTransforms;

   float                                     m_trackTime;
 
public:
   /**
    * Constructor.
    *
    * @param name
    */
   SkeletonAnimationPlayer( const char* name = "SkeletonAnimationPlayer" );

   /**
    * Copy constructor.
    */
   SkeletonAnimationPlayer( const SkeletonAnimationPlayer& rhs );
   ~SkeletonAnimationPlayer();

   /**
    * Returns the current track time.
    */
   inline float getTrackTime() const { return m_trackTime; }

   /**
    * Sets the new track time.
    *
    * @param time
    */
   void setTrackTime( float time );

   /**
    * Sets a new animation source.
    *
    * @param source           animation source
    */
   void setAnimationSource( SnapshotAnimation* source );

   // ----------------------------------------------------------------------
   // AnimationPlayer implementation
   // ----------------------------------------------------------------------
   bool isEnabled() const override;
   void samplePoses( float deltaTime ) override;
   void onStarted() override;
   void onFinished() override;

protected:
   // ----------------------------------------------------------------------
   // Entity implementation
   // ----------------------------------------------------------------------
   void onSiblingAttached( SceneNode* node ) override;
   void onSiblingDetached( SceneNode* node ) override;
   void onAttached( Entity* parent ) override;
   void onDetached( Entity* parent ) override;
};

///////////////////////////////////////////////////////////////////////////////
