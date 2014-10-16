#include "core-Physics\PhysicsCharacterController.h"

// scene
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"

// entity geometry
#include "core-Renderer\GeometryComponent.h"

// physics
#include "core-Physics\PhysicsWorld.h"
#include "core-Physics\PhysicsMaterial.h"
#include "core-Physics\PCCHitsCallbacksInterface.h"

// physx
#include "characterkinematic\PxCapsuleController.h"
#include "characterkinematic\PxControllerManager.h"
#include "PxScene.h"

// utils
#include "core-Physics\PxMathConverter.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PhysicsCharacterController );
   PARENT( PhysicsObject );
   PROPERTY_EDIT( "Material", PhysicsMaterial*, m_material );
   PROPERTY_EDIT( "Capsule height", float, m_height );
   PROPERTY_EDIT( "Capsule radius", float, m_radius );
   PROPERTY_EDIT( "Jump height", float, m_jumpHeight );
   PROPERTY_EDIT( "Slope limit [deg]", float, m_slopeLimit );
   PROPERTY_EDIT( "Max obstacle height", float, m_maxObstacleHeight );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PhysicsCharacterController::PhysicsCharacterController( const char* name )
   : PhysicsObject( name )
   , m_material( NULL )
   , m_height( 1.8f )
   , m_radius( 0.4f )
   , m_jumpHeight( 0.5f )
   , m_slopeLimit( 45.f )
   , m_maxObstacleHeight( 0.5f )
   , m_controller( NULL )
   , m_hitsCallback( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsCharacterController::PhysicsCharacterController( const PhysicsCharacterController& rhs )
   : PhysicsObject( rhs )
   , m_material( rhs.m_material )
   , m_height( rhs.m_height )
   , m_radius( rhs.m_radius )
   , m_jumpHeight( rhs.m_jumpHeight )
   , m_slopeLimit( rhs.m_slopeLimit )
   , m_maxObstacleHeight( rhs.m_maxObstacleHeight )
   , m_controller( NULL )
   , m_hitsCallback( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsCharacterController::~PhysicsCharacterController()
{
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::addToWorld( PhysicsWorld& world )
{
   physx::PxMaterial* material = NULL;
   if ( !m_material || !m_material->getPhysXMaterial() )
   {
      LOG( "Physics material not assigned to body %s - using a default one", getSceneNodeName().c_str() );

      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
      PhysicsMaterial* defaultMat = resMgr.create< PhysicsMaterial >( FilePath( "/Physics/Materials/defaultPhysMat.tpm" ) );
      material = defaultMat->getPhysXMaterial();
   }
   else
   {
      material = m_material->getPhysXMaterial();
   }

   m_hitsCallback = new PCCHitsCallbacksInterface( *this );

   physx::PxCapsuleControllerDesc desc;
   desc.height = m_height;
   desc.radius = m_radius;
   desc.maxJumpHeight = m_jumpHeight;
   desc.material = material;
   desc.slopeLimit = DEG2RAD( m_slopeLimit );
   desc.stepOffset = m_maxObstacleHeight;
   desc.upDirection = physx::PxVec3( 0, 0, 1 );
   desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
   desc.reportCallback = m_hitsCallback;
   desc.contactOffset = 0.01f;
   desc.invisibleWallHeight = 6.0f;
   desc.scaleCoeff = 1.0f;

   // get the height of the attached mesh
   GeometryComponent* geometry = EntityUtils::getFirstChild< GeometryComponent >( getParent() );
   float entityHeight = 0.f;
   if ( geometry )
   {
      AxisAlignedBox bounds;
      geometry->getBoundingBox( bounds );

      Vector extents;
      bounds.getExtents( extents );

      entityHeight = extents.dot( Vector_OZ ).getFloat();
   }

   PxMathConverter::convert( getGlobalMtx().position(), desc.position );
   desc.position.z += ( entityHeight * 0.5f );

   m_controller = world.m_characterControllersManager->createController( desc );

   // reset the velocities
   m_linearVelocity.setZero();
   m_angularVelocity.setZero();

   // get the initial rotation of the character
   Entity* parent = getParent();
   const Matrix& globalMtx = parent->getGlobalMtx();
   globalMtx.getRotation( m_orientation );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::setLinearVelocity( const Vector& velocity )
{
   m_linearVelocity = velocity;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::setAngularVelocity( const Vector& velocity )
{
   m_angularVelocity = velocity;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::addLinearVelocity( const Vector& velocity )
{
   m_linearVelocity.add( velocity );
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::teleport( const Vector& pos )
{
   if ( m_controller )
   {
      physx::PxExtendedVec3 pxPos;
      PxMathConverter::convert( pos, pxPos );
      m_controller->setPosition( pxPos );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::removeFromWorld( PhysicsWorld& world )
{
   if ( m_controller )
   {
      m_controller->release();
      m_controller = NULL;
   }

   delete m_hitsCallback;
   m_hitsCallback = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::tick( float deltaTime )
{
   if ( m_controller )
   {
      // calculate the linear offset
      physx::PxVec3 pxVel;
      PxMathConverter::convert( m_linearVelocity, pxVel );
      m_controller->move( pxVel * deltaTime, 0.f, deltaTime, physx::PxControllerFilters() );

      // calculate the rotation
      m_orientation.addScaledVector( m_angularVelocity, FastFloat::fromFloat( deltaTime ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::updateTransforms()
{
   if ( m_controller )
   {     
      Vector newPos;
      PxMathConverter::convert( m_controller->getFootPosition(), newPos );

      Matrix newIntegratedTransform;
      newIntegratedTransform.setRotation( m_orientation );
      newIntegratedTransform.setPosition< 3 >( newPos );

      Entity* parent = getParent();
      parent->setLocalMtx( newIntegratedTransform );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::onShapeHit( const physx::PxControllerShapeHit& hit )
{
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::onControllerHit( const physx::PxControllersHit& hit )
{
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsCharacterController::onObstacleHit( const physx::PxControllerObstacleHit& hit )
{
}

///////////////////////////////////////////////////////////////////////////////
