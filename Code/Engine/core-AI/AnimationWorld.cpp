#include "core-AI\AnimationWorld.h"
#include "core-AI\AnimationPlayer.h"
#include "core-AI\AISystem.h"
#include "core-MVC\Model.h"
#include "core\ListUtils.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

AnimationWorld::AnimationWorld()
   : m_playing( false )
{
   AISystem& animSys = TSingleton< AISystem >::getInstance();
   animSys.registerWorld( this );
}

///////////////////////////////////////////////////////////////////////////////

AnimationWorld::~AnimationWorld()
{
   AISystem& animSys = TSingleton< AISystem >::getInstance();
   animSys.unregisterWorld( this );
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::onNodeAdded( SceneNode* node )
{
   if ( !node->isA< AnimationPlayer >() )
   {
      return;
   }

   AnimationPlayer* player = static_cast< AnimationPlayer* >( node );

   // check for duplicates first
   List< AnimationPlayer* >::iterator it = ListUtils::find( m_players, player );
   if ( it.isEnd() )
   {
      // this is the first added instance
      m_players.pushBack( player );

      if ( m_playing && player->isEnabled() )
      {
         player->start();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::onNodeRemoved( SceneNode* node )
{
   if ( !node->isA< AnimationPlayer >() )
   {
      return;
   }

   AnimationPlayer* player = static_cast< AnimationPlayer* >( node );

   List< AnimationPlayer* >::iterator it = ListUtils::find( m_players, player );
   if ( !it.isEnd() )
   {
      AnimationPlayer* player = *it;
      it.markForRemoval();

      // before the player gets detached, make it restore the controlled object's transforms
      if ( player->isPlaying() )
      {
         player->stop();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::onNodeChanged( SceneNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::resetContents( Model& model )
{
   simulationFinished();
   m_players.clear();
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::play( bool flag )
{
   if ( m_playing == flag )
   {
      // nothing has changed
      return;
   }

   if ( flag )
   {
      simulationStarted();
   }
   else
   {
      simulationFinished();
   }

   m_playing = flag;
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::simulationStarted()
{
   for ( List< AnimationPlayer* >::iterator it = m_players.begin(); !it.isEnd(); ++it )
   {
      AnimationPlayer* player = *it;
      if ( player->isEnabled() )
      {
         player->start();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::simulationFinished()
{
   for ( List< AnimationPlayer* >::iterator it = m_players.begin(); !it.isEnd(); ++it )
   {
      AnimationPlayer* player = *it;
      if ( player->isPlaying() )
      {
         player->stop();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationWorld::tickAnimations( float timeElapsed )
{
   for ( List< AnimationPlayer* >::iterator it = m_players.begin(); !it.isEnd(); ++it )
   {
      AnimationPlayer* player = *it;

      // check player's status
      if ( player->isPlaying() )
      {
         if ( !player->isEnabled() )
         {
            // turn the player off
            player->stop();
         }
      }
      else
      {
         if ( player->isEnabled() )
         {
            // turn the player on
            player->start();
         }
      }

      if ( player->isPlaying() )
      {
         player->onFrameStart();
         player->samplePoses( timeElapsed );
         player->onFrameEnd();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
