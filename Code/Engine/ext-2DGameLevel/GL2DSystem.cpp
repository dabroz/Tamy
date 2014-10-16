#include "ext-2DGameLevel\GL2DSystem.h"
#include "ext-2DGameLevel\GL2DWorld.h"
#include "core\Assert.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( GL2DSystem );

///////////////////////////////////////////////////////////////////////////////

GL2DSystem::GL2DSystem( const SingletonConstruct& )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DSystem::~GL2DSystem( )
{
   ASSERT_MSG( m_worlds.empty( ), "Not all game levels were unregistered" );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DSystem::registerWorld( GL2DWorld* world )
{
   // check for duplicates first
   List< GL2DWorld* >::iterator it = ListUtils::find( m_worlds, world );
   if ( it.isEnd( ) )
   {
      // this is the first added instance
      m_worlds.pushBack( world );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DSystem::unregisterWorld( GL2DWorld* world )
{
   List< GL2DWorld* >::iterator it = ListUtils::find( m_worlds, world );
   if ( !it.isEnd( ) )
   {
      it.markForRemoval( );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DSystem::tick( float timeElapsed )
{
   // Physics frame definition
   for ( List< GL2DWorld* >::iterator it = m_worlds.begin( ); !it.isEnd( ); ++it )
   {
      GL2DWorld* level = *it;
      if ( level->isRunningSimulation( ) )
      {
         level->tickSimulation( timeElapsed );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
