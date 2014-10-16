/// @file   ext-StoryTeller/StoryPlayer.h
/// @brief  a runtime story player
#pragma once

#include "core\MemoryRouter.h"
#include "core\RefPtr.h"
#include "core\List.h"
#include "core-MVC\ModelView.h"


///////////////////////////////////////////////////////////////////////////////

class Story;
class StoryChapter;
class Model;
class RuntimeDataBuffer;
class SAActivateChapter;
class StoryNode;
class StoryNodeInstance;
class Renderer;
class UserInputController;

///////////////////////////////////////////////////////////////////////////////

/**
 * A runtime story player.
 */
class StoryPlayer : public ModelView
{
   DECLARE_ALLOCATOR( StoryPlayer, AM_DEFAULT );

private:
   enum State
   {
      INITIALIZE,
      PLAY_STORY,
      DEINITIALIZE,
      STORY_OVER
   };

private:
   Story&                           m_story;
   State                            m_playerState;
   RuntimeDataBuffer*               m_runtimeData;

   SAActivateChapter*               m_chapterPlayer;

   Renderer*                        m_renderer;
   UserInputController*             m_userInputController;
   Model*                           m_gameWorld;
   List< StoryNodeInstance* >       m_instancesToSimulate;

public:
   /**
    * Constructor.
    *
    * @param story
    * @param renderer
    */
   StoryPlayer( Story& story );
   ~StoryPlayer();

   // -------------------------------------------------------------------------
   // Initialization
   // -------------------------------------------------------------------------
   /**
    * Sets references to external game systems certain BehaviorTree access.
    *
    * @param renderer      an instance of a renderer that is rendering the story scene
    * @param uic           an instance of a user input controller.
    */
   void setExternalSystems( Renderer& renderer, UserInputController& uic );

   // -------------------------------------------------------------------------
   // Execution
   // -------------------------------------------------------------------------
   /**
    * Executes the story.
    *
    * @return  'true' if the execution continues, 'false' if it has ended
    */
   bool execute();

   /**
    * Resets the player, allowing the scene to be played from the start.
    */
   void reset();

   // -------------------------------------------------------------------------
   // Game context data access
   // -------------------------------------------------------------------------
   /**
    * Returns the runtime data buffer used during simulation.
    */
   inline RuntimeDataBuffer& data() { return *m_runtimeData; }

   /**
    * Returns the game world instance.
    */
   inline Model& gameWorld() { return *m_gameWorld; }

   /**
    * Returns an instance of a renderer that renders the story scene.
    */
   inline Renderer* renderer() { return m_renderer; }

   /**
    * Returns an instance of a user input controller.
    */
   inline UserInputController* userInputController() { return m_userInputController; }

   /**
    * Collects all spawned instances of the specified story node.
    *
    * @param node
    * @param outInstances
    */
   void collectInstances( const StoryNode* node, List< StoryNodeInstance* >& outInstances );

   /**
   * Collects all spawned instances with the specified tag
   *
   * @param objectTag
   * @param outInstances
   */
   void collectInstances( const std::string& objectTag, List< StoryNodeInstance* >& outInstances );


   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );

private:
   void initializePlayer();
   void deinitializePlayer();

   /**
    * Updates logic of objects on the scene.
    */
   void updateLogic();
};

///////////////////////////////////////////////////////////////////////////////
