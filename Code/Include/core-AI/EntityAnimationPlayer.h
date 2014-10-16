/// @file   core-AI/EntityAnimationPlayer.h
/// @brief  an entity that animates a single Entity it's attached to
#pragma once

#include "core-AI\AnimationPlayer.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class SnapshotAnimation;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * This entity animates a single Entity it's attached to.
 */
class EntityAnimationPlayer : public AnimationPlayer
{
   DECLARE_ALLOCATOR( EntityAnimationPlayer, AM_DEFAULT );
   DECLARE_CLASS()

private:
   // static data
   SnapshotAnimation*                        m_source;

   // runtime data
   Entity*                                   m_animatedEntity;

   float                                     m_trackTime;
 
public:
   /**
    * Constructor.
    *
    * @param name
    */
   EntityAnimationPlayer( const char* name = "EntityAnimationPlayer" );

   /**
    * Copy constructor.
    */
   EntityAnimationPlayer( const EntityAnimationPlayer& rhs );
   ~EntityAnimationPlayer();

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
   void setAnimationSource( SnapshotAnimation& source );

   // ----------------------------------------------------------------------
   // AnimationPlayer implementation
   // ----------------------------------------------------------------------
   bool isEnabled() const;
   void samplePoses( float deltaTime );
   void onStarted();
   void onFinished();

protected:
   // ----------------------------------------------------------------------
   // Entity implementation
   // ----------------------------------------------------------------------
   void onAttached( Entity* parent );
   void onDetached( Entity* parent );
};

///////////////////////////////////////////////////////////////////////////////
