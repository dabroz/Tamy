/// @file   core-AI/AnimationPlayer.h
/// @brief  base class for all animation players
#pragma once

#include "core-MVC\Component.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all animation players.
 */
class AnimationPlayer : public Component
{
   DECLARE_ALLOCATOR( AnimationPlayer, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // runtime data
   bool        m_isPlaying;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   AnimationPlayer( const char* name = "" );

   /**
    * Copy constructor.
    */
   AnimationPlayer( const AnimationPlayer& rhs );
   virtual ~AnimationPlayer();

   /**
    * Checks if the play is enabled and can actually play the animation.
    */
   virtual bool isEnabled() const { return false; }

   /**
    * Tells if the player is currently playing or not.
    */
   inline bool isPlaying() const { return m_isPlaying; }
   
   /**
    * Starts the player.
    */
   void start();

   /**
    * Stops the player, resetting the animation.
    */
   void stop();

   // -------------------------------------------------------------------------
   // AI frame
   // -------------------------------------------------------------------------
   /**
    * Called when the frame starts.
    */
   virtual void onFrameStart() {}

   /**
    * Samples animation poses.
    *
    * @param timeElapsed
    */
   virtual void samplePoses( float deltaTime ) {}

   /**
    * Called when the frame ends.
    */
   virtual void onFrameEnd() {}

protected:
   // -------------------------------------------------------------------------
   // Simulation stage notifications
   // -------------------------------------------------------------------------

   /**
    * Called when the simulation starts.
    */
   virtual void onStarted() {}

   /**
    * Called when the simulation ends.
    */
   virtual void onFinished() {}
};

///////////////////////////////////////////////////////////////////////////////
