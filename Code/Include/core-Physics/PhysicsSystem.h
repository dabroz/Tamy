/// @file   core-Physics\PhysicsSystem.h
/// @brief  physics system
#pragma once

#include "core\Singleton.h"
#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxFoundation;
   class PxProfileZoneManager;
   class PxPhysics;
   class PxCudaContextManager;
   class PxCooking;

   namespace debugger
   {
      namespace comm
      {
         class PvdConnection;
      };
   }
   typedef debugger::comm::PvdConnection PxVisualDebuggerConnection;

}

class PhysicsWorld;
class PhysicsMaterial;

///////////////////////////////////////////////////////////////////////////////

class PhysicsSystem
{
   REGULAR_SINGLETON();
   DECLARE_ALLOCATOR( PhysicsSystem, AM_DEFAULT );

private:
   class PXErrorCallback*                 m_errorCallback;
   class PXMemoryAllocator*               m_memoryAllocator;

   physx::PxFoundation*                   m_foundation;
   physx::PxProfileZoneManager*           m_profileZoneManager;
   physx::PxPhysics*                      m_physics;
   physx::PxCudaContextManager*           m_cudaContextManager;
   physx::PxCooking*                      m_cookingInterface;
   physx::PxVisualDebuggerConnection*     m_debuggerConnection;

   List< PhysicsWorld* >                  m_worlds;
   List< PhysicsMaterial* >               m_materials;            // the system keeps track of all materials, deinitializing them
                                                                  // when the time comes ( they are resources, so their actual destruction
                                                                  // might happen later )

public:
   /**
    * Singleton constructor.
    */
   PhysicsSystem( const SingletonConstruct& );
   ~PhysicsSystem();

   /**
    * Runs the physics simulation.
    *
    * @param deltaTime
    */
   void tick( float deltaTime );

   // -------------------------------------------------------------------------
   // Worlds management
   // -------------------------------------------------------------------------
   /**
   * Registers a new physics world. The system will take care of ticking it from now on.
   *
   * @param world
   */
   void addWorld( PhysicsWorld* world );

   /**
   * Unregisters a physics world.
   *
   * @param world
   */
   void removeWorld( PhysicsWorld* world );

   // -------------------------------------------------------------------------
   // Materials management
   // -------------------------------------------------------------------------
   /**
   * Registers a new physics material.
   *
   * @param material
   */
   void addMaterial( PhysicsMaterial* material );

   /**
   * Unregisters a physics material.
   *
   * @param material
   */
   void removeMaterial( PhysicsMaterial* material );


   // -------------------------------------------------------------------------
   // PhysX data access
   // -------------------------------------------------------------------------

   /**
    * Gives access to the core PhysX interface.
    */
   inline physx::PxPhysics* getPhysXInterface() const {
      return m_physics;
   }

   /**
    * Gives access to CUDA context manager.
    */
   inline physx::PxCudaContextManager* getCudaContextManager() const {
      return m_cudaContextManager;
   }

   /**
    * Returns the cooking interface.
    */
   inline physx::PxCooking* getCookingInterface() const {
      return m_cookingInterface;
   }

   /**
    * How many threads a physics scene is allowed to use.
    */
   uint getPhysicsThreadsCount() const;

};

///////////////////////////////////////////////////////////////////////////////
