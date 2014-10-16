#include "core-Physics\PhysicsSystem.h"

// physx 
#include "foundation\PxFoundation.h"
#include "physxprofilesdk\PxProfileZoneManager.h"
#include "physxvisualdebuggersdk\PvdConnection.h"
#include "PxPhysics.h"
#include "common\PxTolerancesScale.h"
#include "extensions\PxExtensionsAPI.h"
#include "extensions\PxVisualDebuggerExt.h"
#include "pxtask\PxCudaContextManager.h"
#include "cooking\PxCooking.h"

// core systems
#include "core-Physics\PXErrorCallback.h"
#include "core-Physics\PXMemoryAllocator.h"

// definitions
#include "core-Physics\Defines.h"

// utils
#include "core\ListUtils.h"
#include "core\Algorithms.h"

// runtime
#include "core-Physics\PhysicsWorld.h"
#include "core-Physics\PhysicsMaterial.h"

// threads
#include "core\ThreadSystem.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( PhysicsSystem );

///////////////////////////////////////////////////////////////////////////////

PhysicsSystem::PhysicsSystem( const SingletonConstruct& )
   : m_errorCallback( new PXErrorCallback() )
   , m_memoryAllocator( new PXMemoryAllocator() )
   , m_cudaContextManager( NULL )
{
   // initialize the base systems
   m_foundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_memoryAllocator, *m_errorCallback );
   if ( !m_foundation )
   {
      ASSERT_MSG( false, "PxCreateFoundation failed!" );
      return;
   }

   m_profileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager( m_foundation );
   if ( !m_profileZoneManager )
   {
      ASSERT_MSG( false, "PxProfileZoneManager::createProfileZoneManager failed!" );
      return;
   }

#ifdef RECORD_PHYSICS_MEMORY_ALLOCATIONS
   const bool recordMemoryAllocations = true;
#else
   const bool recordMemoryAllocations = false;
#endif 

   m_physics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), recordMemoryAllocations, m_profileZoneManager );
   if ( !m_physics )
   {
      ASSERT_MSG( false, "PxCreatePhysics failed!" );
      return;
   }

   // initialize the extensions
   if ( !PxInitExtensions( *m_physics ) )
   {
      ASSERT_MSG( false, "PxInitExtensions failed!" );
      return;
   }

   // initialize CUDA
#ifdef PX_WINDOWS
   physx::PxCudaContextManagerDesc cudaContextManagerDesc;

   m_cudaContextManager = PxCreateCudaContextManager( *m_foundation, cudaContextManagerDesc, m_profileZoneManager );
   if ( m_cudaContextManager )
   {
      if ( !m_cudaContextManager->contextIsValid() )
      {
         m_cudaContextManager->release();
         m_cudaContextManager = NULL;
      }
   }
#endif

   // initialize the cooking interface
   m_cookingInterface = PxCreateCooking( PX_PHYSICS_VERSION, *m_foundation, physx::PxCookingParams( physx::PxTolerancesScale() ) );
   if ( !m_cookingInterface )
   {
      ASSERT_MSG( false, "PxCreateCooking failed!" );
      return;
   }

   // initialize the debugger connection, if available
   if ( m_physics->getPvdConnectionManager() )
   {
      // setup connection parameters
      const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
      int             port = 5425;         // TCP port to connect to, where PVD is listening
      unsigned int    timeout = 100;          // timeout in milliseconds to wait for PVD to respond,

      // consoles and remote PCs need a higher timeout.
      physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerExt::getAllConnectionFlags();

      // and now try to connect
      m_debuggerConnection = physx::PxVisualDebuggerExt::createConnection( m_physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags );
   }
}

///////////////////////////////////////////////////////////////////////////////

PhysicsSystem::~PhysicsSystem()
{
   m_worlds.clear();

   // destroy PhysX systems
   if ( m_debuggerConnection )
   {
      m_debuggerConnection->release();
      m_debuggerConnection = NULL;
   }

   if ( m_cookingInterface )
   {
      m_cookingInterface->release();
      m_cookingInterface = NULL;
   }

   if ( m_cudaContextManager )
   {
      m_cudaContextManager->release();
      m_cudaContextManager = NULL;
   }

   PxCloseExtensions();

   if ( m_physics )
   {
      m_physics->release();
      m_physics = NULL;
   }

   if ( m_profileZoneManager )
   {
      m_profileZoneManager->release();
      m_profileZoneManager = NULL;
   }

   if ( m_foundation )
   {
      m_foundation->release();
      m_foundation = NULL;
   }

   delete m_memoryAllocator;
   m_memoryAllocator = NULL;

   delete m_errorCallback;
   m_errorCallback = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::addWorld( PhysicsWorld* world )
{
   if ( !world )
   {
      return;
   }

   if ( ListUtils::find( m_worlds, world ).isEnd() )
   {
      m_worlds.pushBack( world );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::removeWorld( PhysicsWorld* world )
{
   if ( !world )
   {
      return;
   }

   List< PhysicsWorld* >::iterator it = ListUtils::find( m_worlds, world );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::addMaterial( PhysicsMaterial* material )
{
   if ( !material )
   {
      return;
   }

   if ( ListUtils::find( m_materials, material ).isEnd() )
   {
      material->createPhysXMaterial();
      m_materials.pushBack( material );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::removeMaterial( PhysicsMaterial* material )
{
   if ( !material )
   {
      return;
   }
   List< PhysicsMaterial* >::iterator it = ListUtils::find( m_materials, material );
   if ( !it.isEnd() )
   {
      PhysicsMaterial* material = *it;
      material->releasePhysXMaterial();

      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsSystem::tick( float deltaTime )
{
   for ( List< PhysicsWorld* >::iterator it = m_worlds.begin(); !it.isEnd(); ++it )
   {
      PhysicsWorld* world = *it;
      world->tick( deltaTime );
   }
}

///////////////////////////////////////////////////////////////////////////////

uint PhysicsSystem::getPhysicsThreadsCount() const
{
   // allow physics to use half the available threads, but not less than one
   ThreadSystem& threadSys = TSingleton< ThreadSystem >::getInstance();
   return max2<uint>( 1, threadSys.getCoresCount() );
}

///////////////////////////////////////////////////////////////////////////////
