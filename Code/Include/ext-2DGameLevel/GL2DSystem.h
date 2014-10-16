/// @file   ext-2DGameLevel/GL2DSystem.h
/// @brief  a system that takes care of running all 2d game levels
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DWorld;

///////////////////////////////////////////////////////////////////////////////

/**
* Physics system that takes care of running all active physics worlds.
*/
class GL2DSystem
{
   REGULAR_SINGLETON();
   DECLARE_ALLOCATOR( GL2DSystem, AM_DEFAULT );

private:
   List< GL2DWorld* >         m_worlds;

public:
   /**
   * Constructor.
   */
   GL2DSystem( const SingletonConstruct& );
   ~GL2DSystem( );

   /**
   * Registers a new game level.
   *
   * @param world
   */
   void registerWorld( GL2DWorld* world );

   /**
   * Unregisters a game level, usually when it goes out of scope.
   *
   * @param world
   */
   void unregisterWorld( GL2DWorld* world );

   /**
   * Updates managed game levels.
   *
   * @param timeElapsed
   */
   void tick( float timeElapsed );

};

///////////////////////////////////////////////////////////////////////////////
