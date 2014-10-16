/// @file   core-AI/AnimationWorld.h
/// @brief  a model view that will gather all animation players attached to a scene in order to tick them when applicable
#pragma once

#include "core-MVC\ModelView.h"
#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class AnimationPlayer;

///////////////////////////////////////////////////////////////////////////////

class AnimationWorld : public ModelView
{
   DECLARE_ALLOCATOR( AnimationWorld, AM_DEFAULT );
   
private:
   List< AnimationPlayer* >      m_players;
   bool                          m_playing;

public:
   /**
    * Constructor.
    */
   AnimationWorld();
   ~AnimationWorld();

   /**
    * Is the world playing animations ( true ), or is it paused ( false ).
    */
   inline bool isPlaying() const { return m_playing; }

   /**
    * Runs/stops playing registered animation players.
    *
    * @param flag
    */
   void play( bool flag );

   /**
    * Updates registered animation players.
    *
    * @param timeElapsed
    */
   void tickAnimations( float deltaTime );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& model );

private:
   void simulationStarted();
   void simulationFinished();
};

///////////////////////////////////////////////////////////////////////////////
