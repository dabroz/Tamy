/// @file   core-AI\AISystem.h
/// @brief  animation system that takes care of running all active animation worlds
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class AnimationWorld;

///////////////////////////////////////////////////////////////////////////////

/**
 * Animation system that takes care of running all active animation worlds.
 */
class AISystem
{
   PRIORITY_SINGLETON( 10 );
   DECLARE_ALLOCATOR( AISystem, AM_DEFAULT );

private:
   List< AnimationWorld* >      m_worlds;

public:
   /**
    * Constructor.
    */
   AISystem( const SingletonConstruct& );
   ~AISystem();

   /**
    * Registers a new animation world.
    *
    * @param world
    */
   void registerWorld( AnimationWorld* world );

   /**
    * Unregisters an animation world, usually when it goes out of scope.
    *
    * @param world
    */
   void unregisterWorld( AnimationWorld* world );

   /**
    * Updates managed animations.
    *
    * @param timeElapsed
    */
   void tick( float timeElapsed );

};

///////////////////////////////////////////////////////////////////////////////
