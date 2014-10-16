#include "core-Physics\PhysicsWorld.h"

// physx
#include "PxPhysics.h"
#include "PxScene.h"
#include "extensions\PxDefaultCpuDispatcher.h"
#include "extensions\PxDefaultSimulationFilterShader.h"
#include "pxtask\PxCudaContextManager.h"
#include "characterkinematic\PxControllerManager.h"

// physics
#include "core-Physics\PhysicsObject.h"
#include "core-Physics\PhysicsSystem.h"

// utils
#include "core-Physics\PxMathConverter.h"
#include "core\ListUtils.h"

// math
#include "core\Vector.h"

// scene
#include "core-MVC\Model.h"
#include "core-MVC\SceneNode.h"


///////////////////////////////////////////////////////////////////////////////

PhysicsWorld::PhysicsWorld( const Vector& gravity )
   : m_simulationEnabled( false )
{
   PhysicsSystem& physicsSys = TSingleton< PhysicsSystem >::getInstance();
   physx::PxPhysics* physicsCore = physicsSys.getPhysXInterface();

   // initialize the PhysX scene
   physx::PxSceneDesc sceneDesc( physicsCore->getTolerancesScale() );
   PxMathConverter::convert( gravity, sceneDesc.gravity );

   //customizeSceneDesc( sceneDesc );

   if ( !sceneDesc.cpuDispatcher )
   {
      m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate( physicsSys.getPhysicsThreadsCount() );
      if ( !m_cpuDispatcher )
      {
         ASSERT_MSG( false, "PxDefaultCpuDispatcherCreate failed!" );
      }
      sceneDesc.cpuDispatcher = m_cpuDispatcher;
   }
   if ( !sceneDesc.filterShader )
   {
      sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
   }

   physx::PxCudaContextManager* cudaContextManager = physicsSys.getCudaContextManager();
   if ( !sceneDesc.gpuDispatcher && cudaContextManager )
   {
      sceneDesc.gpuDispatcher = cudaContextManager->getGpuDispatcher();
   }

   m_scene = physicsCore->createScene( sceneDesc );
   if ( !m_scene )
   {
      ASSERT_MSG( false, "createScene failed!" );
      return;
   }

   m_characterControllersManager = PxCreateControllerManager( *m_scene );
   if ( !m_characterControllersManager )
   {
      ASSERT_MSG( false, "PxCreateControllerManager failed!" );
      return;
   }

   // register self with the system
   physicsSys.addWorld( this );
}

///////////////////////////////////////////////////////////////////////////////

PhysicsWorld::~PhysicsWorld()
{
   // remove all registered bodies
   for ( List< PhysicsObject* >::iterator it = m_objects.begin(); !it.isEnd(); ++it )
   {
      PhysicsObject* object = *it;
      object->removeFromWorld( *this );
   }
   m_objects.clear();

   // remove self from the system
   PhysicsSystem& sys = TSingleton< PhysicsSystem >::getInstance();
   sys.removeWorld( this );

   // delete PhysX representations

   if ( m_characterControllersManager )
   {
      m_characterControllersManager->purgeControllers();
      m_characterControllersManager->release();
      m_characterControllersManager = NULL;
   }

   if ( m_scene )
   {
      m_scene->release();
      m_scene = NULL;
   }

   if ( m_cpuDispatcher )
   {
      m_cpuDispatcher->release();
      m_cpuDispatcher = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::onAttachedToModel( Model& model )
{
   // pull the contents of the model
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::onDetachedFromModel( Model& model )
{
   resetContents( model );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::onNodeAdded( SceneNode* node )
{
   if ( node->isA< PhysicsObject >() )
   {
      PhysicsObject* object = static_cast< PhysicsObject* >( node );
      m_objects.pushBack( object );

      if ( m_simulationEnabled )
      {
         object->addToWorld( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::onNodeRemoved( SceneNode* node )
{
   if ( node->isA< PhysicsObject >() )
   {
      PhysicsObject* object = static_cast< PhysicsObject* >( node );

      List< PhysicsObject* >::iterator it = ListUtils::find( m_objects, object );
      it.markForRemoval();
      
      if ( m_simulationEnabled )
      {
         object->removeFromWorld( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::onNodeChanged( SceneNode* node )
{
   if ( node->isA< PhysicsObject >() )
   {
      PhysicsObject* object = static_cast< PhysicsObject* >( node );
      if ( m_simulationEnabled )
      {
         object->removeFromWorld( *this );
         object->addToWorld( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::resetContents( Model& model )
{
   // remove all registered bodies
   for ( List< PhysicsObject* >::iterator it = m_objects.begin(); !it.isEnd(); ++it )
   {
      PhysicsObject* object = *it;
      if ( object->getHostModel() == &model )
      {
         if ( m_simulationEnabled )
         {
            object->removeFromWorld( *this );
         }
         it.markForRemoval();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::tick( float deltaTime )
{
   if ( !m_simulationEnabled || !m_scene )
   {
      return;
   }

   // tick the objects
   for ( List< PhysicsObject* >::iterator it = m_objects.begin(); !it.isEnd(); ++it )
   {
      PhysicsObject* object = *it;
      object->tick( deltaTime );
   }

   // run the simulation
   m_scene->simulate( deltaTime );
   m_scene->fetchResults( true );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsWorld::enableSimulation( bool enable )
{
   if ( m_simulationEnabled == enable )
   {
      // nothing to do here
      return;
   }

   m_simulationEnabled = enable;

   // start/stop the simulation
   if ( m_simulationEnabled )
   {
      for ( List< PhysicsObject* >::iterator it = m_objects.begin(); !it.isEnd(); ++it )
      {
         PhysicsObject* object = *it;
         object->addToWorld( *this );
      }
   }
   else
   {
      for ( List< PhysicsObject* >::iterator it = m_objects.begin(); !it.isEnd(); ++it )
      {
         PhysicsObject* object = *it;
         object->removeFromWorld( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
