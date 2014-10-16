#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\SAActivateChapter.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

StoryPlayer::StoryPlayer( Story& story )
   : m_story( story )
   , m_playerState( INITIALIZE )
   , m_gameWorld( new Model() )
   , m_renderer( NULL )
   , m_userInputController( NULL )
   , m_chapterPlayer( NULL )
   , m_runtimeData( NULL )
{
   m_gameWorld->attachListener( this );
}

///////////////////////////////////////////////////////////////////////////////

StoryPlayer::~StoryPlayer()
{
   deinitializePlayer();

   m_gameWorld->removeReference();
   m_gameWorld = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::setExternalSystems( Renderer& renderer, UserInputController& uic )
{
   m_renderer = &renderer;
   m_userInputController = &uic;
}

///////////////////////////////////////////////////////////////////////////////

bool StoryPlayer::execute()
{
   switch( m_playerState )
   {
   case INITIALIZE:
      {
         ASSERT( m_chapterPlayer == NULL );
         initializePlayer();
         
         m_playerState = PLAY_STORY;

         // fallthrough to PLAY_STORY
      }

   case PLAY_STORY:
      {
         ASSERT( m_chapterPlayer != NULL );

         List< StoryAction* > actionsToExecute;
         bool isStoryOver = m_chapterPlayer->execute( *this, actionsToExecute );
         ASSERT_MSG( actionsToExecute.empty(), "Root chapter should never schedule any further actions for execution" );

         if ( isStoryOver )
         {
            m_playerState = DEINITIALIZE;
         }
         else
         {
            updateLogic();
         }
         
         // the story always continues in this state
         return true;
      }

   case DEINITIALIZE:
      {
         deinitializePlayer();
         m_playerState = STORY_OVER;

         // fallthrough to STORY_OVER
      };

   case STORY_OVER:
      {
         // the story's over
         ASSERT( m_chapterPlayer == NULL );
         return false;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::reset()
{
   deinitializePlayer();
   m_playerState = INITIALIZE;
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::initializePlayer()
{
   // initialize the new story
   m_runtimeData = new RuntimeDataBuffer();

   m_chapterPlayer = new SAActivateChapter( &m_story.getRoot() );
   m_chapterPlayer->createLayout( *this );
   m_chapterPlayer->initialize( *this );
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::deinitializePlayer()
{
   if ( m_chapterPlayer )
   {
      m_chapterPlayer->deinitialize( *this );
      m_chapterPlayer->destroyLayout( *this );
   }

   m_gameWorld->clear();

   for ( List< StoryNodeInstance* >::iterator it = m_instancesToSimulate.begin(); !it.isEnd(); ++it )
   {
      StoryNodeInstance* instance = *it;
      instance->removeReference();
   }
   m_instancesToSimulate.clear();

   delete m_chapterPlayer;
   m_chapterPlayer = NULL;

   delete m_runtimeData;
   m_runtimeData = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::onNodeAdded( SceneNode* node )
{
   if ( node->isA< StoryNodeInstance >() )
   {
      StoryNodeInstance* instance = static_cast< StoryNodeInstance* >( node );
      instance->addReference();

      instance->initializeContext( *this );

      m_instancesToSimulate.pushBack( instance );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::onNodeRemoved( SceneNode* node )
{
   if ( node->isA< StoryNodeInstance >() )
   {
      StoryNodeInstance* instance = static_cast< StoryNodeInstance* >( node );
      List< StoryNodeInstance* >::iterator it = ListUtils::find( m_instancesToSimulate, instance );
      if ( !it.isEnd() )
      {
         instance->deinitializeContext( *this );

         instance->removeReference();
         it.markForRemoval();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::onNodeChanged( SceneNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::updateLogic()
{
   for ( List< StoryNodeInstance* >::iterator it = m_instancesToSimulate.begin(); !it.isEnd(); ++it )
   {
      StoryNodeInstance* instance = *it;
      instance->updateLogic();
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::collectInstances( const StoryNode* node, List< StoryNodeInstance* >& outInstances )
{
   for ( List< StoryNodeInstance* >::iterator it = m_instancesToSimulate.begin(); !it.isEnd(); ++it )
   {
      StoryNodeInstance* instance = *it;
      if ( instance->getStoryNode() == node )
      {
         outInstances.pushBack( instance );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryPlayer::collectInstances( const std::string& objectTag, List< StoryNodeInstance* >& outInstances )
{
   for ( List< StoryNodeInstance* >::iterator it = m_instancesToSimulate.begin(); !it.isEnd(); ++it )
   {
      StoryNodeInstance* instance = *it;
      if ( instance->getSceneNodeName().find( objectTag ) != std::string::npos )
      {
         outInstances.pushBack( instance );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////
