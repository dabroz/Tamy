#include "core-AI\AISystem.h"
#include "core-AI\AnimationWorld.h"
#include "core\Assert.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( AISystem );

///////////////////////////////////////////////////////////////////////////////

AISystem::AISystem( const SingletonConstruct& )
{
}

///////////////////////////////////////////////////////////////////////////////

AISystem::~AISystem()
{
   ASSERT_MSG( m_worlds.empty(), "Not all animation worlds were unregistered" );
}

///////////////////////////////////////////////////////////////////////////////

void AISystem::registerWorld( AnimationWorld* world )
{
   // check for duplicates first
   List< AnimationWorld* >::iterator it = ListUtils::find( m_worlds, world );
   if ( it.isEnd() )
   {
      // this is the first added instance
      m_worlds.pushBack( world );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AISystem::unregisterWorld( AnimationWorld* world )
{
   List< AnimationWorld* >::iterator it = ListUtils::find( m_worlds, world );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void AISystem::tick( float timeElapsed )
{
   // AI frame definition

   // 1. update animations ( LAST )
   for ( List< AnimationWorld* >::iterator it = m_worlds.begin(); !it.isEnd(); ++it )
   {
      AnimationWorld* world = *it;
      if ( world->isPlaying() )
      {
         world->tickAnimations( timeElapsed );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
