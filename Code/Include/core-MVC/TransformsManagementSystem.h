/// @file   core-MVC\TransformsManagementSystem.h
/// @brief  a system for centralized scene node transforms management
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * A system for centralized scene node transforms management.
 */
class TransformsManagementSystem
{
   PRIORITY_SINGLETON( 11 );
   DECLARE_ALLOCATOR( TransformsManagementSystem, AM_DEFAULT );

private:
   List< Model* >          m_scenes;
   List< Entity* >         m_entities;

public:
   /**
    * Singleton constructor.
    */
   TransformsManagementSystem( const SingletonConstruct& );
   ~TransformsManagementSystem();

   /**
    * Adds a scene to the system. From this point on its transforms will
    * be updated every frame.
    * @param scene
    */
   void addScene( Model* scene );

   /**
    * Removes a scene from the system. Scene transforms won't be updated any more.
    *
    * @param scene
    */
   void removeScene( Model* scene );

   /**
    * Adds an individual entity that's not a part of any scene,
    * yet we want its transform updated.
    *
    * A good example of such an entity would be a camera.
    *
    * @param entity
    */
   void addEntity( Entity* entity );

   /**
    * Stops updating the specified individual entity, that was added using the 'addEntity' method.
    *
    * @param entity
    */
   void removeEntity( Entity* entity );

   /**
    * Ticks the manager.
    */
   void tick();
};

///////////////////////////////////////////////////////////////////////////////
