/// @file   core-MVC\TransformsManagementSystem.h
/// @brief  a system for centralized scene node transforms management
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Transformable;

///////////////////////////////////////////////////////////////////////////////

/**
 * A system for centralized scene node transforms management.
 */
class TransformsManagementSystem
{
   PRIORITY_SINGLETON( 12 );
   DECLARE_ALLOCATOR( TransformsManagementSystem, AM_DEFAULT );

private:
   List< Model* >                m_scenes;
   List< Transformable* >        m_transformables;

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
    * Adds an individual transformable object that's not a part of any scene,
    * yet we want its transform updated.
    *
    * A good example of such an entity would be a camera.
    *
    * @param obj
    */
   void addTransformable( Transformable* obj );

   /**
    * Stops updating the specified individual transformable object, that was added using the 'addTransformable' method.
    *
    * @param obj
    */
   void removeTransformable( Transformable* obj );

   /**
    * Ticks the manager.
    */
   void tick();
};

///////////////////////////////////////////////////////////////////////////////
