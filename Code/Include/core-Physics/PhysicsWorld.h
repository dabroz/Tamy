/// @file   core-Physics\PhysicsWorld.h
/// @brief  a physics world where all physical bodies are simulated
#pragma once

#include "core\MemoryRouter.h"
#include "core-MVC\ModelView.h"
#include "core\List.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxScene;
   class PxControllerManager;
   class PxDefaultCpuDispatcher;
   class PxRigidActor;
};

struct Vector;
class PhysicsObject;

enum PhysicsCollisionGroup
{
   Collision_Static,
   Collision_Dynamic,
   Collision_Ragdoll,

   Collision_Count
};

///////////////////////////////////////////////////////////////////////////////

class PhysicsWorld : public ModelView
{
   DECLARE_ALLOCATOR( PhysicsWorld, AM_DEFAULT );

private:
   struct CollisionGroup
   {
      DECLARE_ALLOCATOR( CollisionGroup, AM_DEFAULT );

      uint     m_collisionMask;

      CollisionGroup()
         : m_collisionMask( 0xffffffff )
      {}
   };

public:
   physx::PxScene*                        m_scene;
   physx::PxControllerManager*            m_characterControllersManager;
   physx::PxDefaultCpuDispatcher*         m_cpuDispatcher;

private:
   List< PhysicsObject* >                 m_objects;
   bool                                   m_simulationEnabled;
   Array< CollisionGroup >                m_collisionGroups;

   uint                                   m_nextFreePhysicsSystemId;

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
   // Collision filtering
   // -------------------------------------------------------------------------

   /**
    * Assigns the specified actor to the specified collision group.
    *
    * NOTE: Call this method AFTER assigning the actor a shape, because the method sets a flag on the said shape.
    *
    * @param actor
    * @param collisionGroup
    * @param systemId               bodies assigned to the same system don't collide, unless the assigned system has ID 0 ( the default ID ) , 
    *                               in which case it will collide with everything
    */
   void assignCollisionGroup( physx::PxRigidActor* actor, PhysicsCollisionGroup collisionGroup, uint systemId = 0 );

   /**
    * Allocates a new physics system ID.
    */
   uint allocatePhysicsSystemId();

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
