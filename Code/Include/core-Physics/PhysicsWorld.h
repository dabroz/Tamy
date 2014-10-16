/// @file   core-Physics\PhysicsWorld.h
/// @brief  a physics world where all physical bodies are simulated
#pragma once

#include "core\MemoryRouter.h"
#include "core-MVC\ModelView.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxScene;
   class PxControllerManager;
   class PxDefaultCpuDispatcher;
};

struct Vector;
class PhysicsObject;

///////////////////////////////////////////////////////////////////////////////

class PhysicsWorld : public ModelView
{
   DECLARE_ALLOCATOR( PhysicsWorld, AM_DEFAULT );

public:
   physx::PxScene*                        m_scene;
   physx::PxControllerManager*            m_characterControllersManager;
   physx::PxDefaultCpuDispatcher*         m_cpuDispatcher;

private:
   List< PhysicsObject* >                 m_objects;
   bool                                   m_simulationEnabled;

public:
   /**
    * Constructor.
    *
    * @param gravity
    */
   PhysicsWorld( const Vector& gravity );
   ~PhysicsWorld();

   /**
    * Ticks the physics simulation in this world.
    *
    * @param deltaTime
    */
   void tick( float deltaTime );

   /**
    * Toggles the simulation on/off.
    *
    * @param enable
    */
   void enableSimulation( bool enable );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onDetachedFromModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& model );
};

///////////////////////////////////////////////////////////////////////////////
