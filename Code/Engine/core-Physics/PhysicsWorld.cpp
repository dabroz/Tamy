#include "core-Physics\PhysicsWorld.h"

// physx
#include "PxPhysics.h"
#include "PxFiltering.h"
#include "PxScene.h"
#include "PxSceneLock.h"
#include "PxRigidActor.h"
#include "extensions\PxDefaultCpuDispatcher.h"
#include "extensions\PxDefaultSimulationFilterShader.h"
#include "pxtask\PxCudaContextManager.h"
#include "characterkinematic\PxControllerManager.h"

// physics
#include "core-Physics\PhysicsObject.h"
#include "core-Physics\PhysicsSystem.h"
#include "core-Physics\Defines.h"

// utils
#include "core-Physics\PxMathConverter.h"
#include "core\ListUtils.h"

// math
#include "core\Vector.h"

// scene
#include "core-MVC\Model.h"
#include "core-MVC\SceneNode.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A custom collision filtering shader
 */
static physx::PxFilterFlags CollisionFilterShader(
   physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
   physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
   physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize )
{
   // let triggers through
   if ( physx::PxFilterObjectIsTrigger( attributes0 ) || physx::PxFilterObjectIsTrigger( attributes1 ) )
   {
      pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
      return physx::PxFilterFlag::eDEFAULT;
   }
   // generate contacts for all that were not filtered above
   pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

   // if the two objects' collision layers overlap, make them collide
   const uint collisionBetweenGroups = filterData0.word0 & filterData1.word0;
   const bool systemAssignmentAllowsForCollision = filterData0.word1 == 0 || filterData1.word1 == 0 || ( ( filterData0.word1 & filterData1.word1 ) != filterData0.word1 );

   if ( collisionBetweenGroups && systemAssignmentAllowsForCollision )
   {
      // these two should collide
      return physx::PxFilterFlag::eDEFAULT;
   }
   
   // filter the collision between the two bodies
   return physx::PxFilterFlag::eKILL;
}

///////////////////////////////////////////////////////////////////////////////

PhysicsWorld::PhysicsWorld( const Vector& gravity )
   : m_simulationEnabled( false )
   , m_collisionGroups( Collision_Count )
   , m_nextFreePhysicsSystemId( 1 )
{
   PhysicsSystem& physicsSys = TSingleton< PhysicsSystem >::getInstance();
   physx::PxPhysics* physicsCore = physicsSys.getPhysXInterface();

   // initialize the PhysX scene
   physx::PxSceneDesc sceneDesc( physicsCore->getTolerancesScale() );
   sceneDesc.flags = physx::PxSceneFlag::eREQUIRE_RW_LOCK;
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

   // assign our custom collision filtering shader
   sceneDesc.filterShader = CollisionFilterShader;

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

   // setup debug visualization options
#ifdef PVD_SUPPORT
   {
      physx::PxSceneWriteLock scopedLock( *m_scene );
      m_scene->setVisualizationParameter( physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f );
      m_scene->setVisualizationParameter( physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f );
   }
#endif

   m_characterControllersManager = PxCreateControllerManager( *m_scene );
   if ( !m_characterControllersManager )
   {
      ASSERT_MSG( false, "PxCreateControllerManager failed!" );
      return;
   }

   // register self with the system
   physicsSys.addWorld( this );

   // initialize collision groups
   m_collisionGroups.resize( Collision_Count, CollisionGroup() );
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
   {
      physx::PxSceneWriteLock scopedLock( *m_scene );
      m_scene->simulate( deltaTime );
      m_scene->fetchResults( true );
   }
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

void PhysicsWorld::assignCollisionGroup( physx::PxRigidActor* actor, PhysicsCollisionGroup collisionGroup, uint systemId )
{
   const CollisionGroup& group = m_collisionGroups[collisionGroup];

   physx::PxFilterData filterData;
   filterData.word0 = group.m_collisionMask;
   filterData.word1 = systemId;

   // assign the collision group  to the actor
   const uint numShapes = actor->getNbShapes();
   Array< physx::PxShape* > shapes( numShapes );
   shapes.resize( numShapes, NULL );
   actor->getShapes( shapes.getRaw(), numShapes );

   for ( uint i = 0; i < numShapes; ++i )
   {
      physx::PxShape* shape = shapes[i];
      shape->setSimulationFilterData( filterData );
   }
   
}

///////////////////////////////////////////////////////////////////////////////

uint PhysicsWorld::allocatePhysicsSystemId()
{
   uint allocatedId = m_nextFreePhysicsSystemId;
   ++m_nextFreePhysicsSystemId;

   return allocatedId;
}

///////////////////////////////////////////////////////////////////////////////
