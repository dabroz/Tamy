#include "ext-ProceduralAnimation\RagdollComponent.h"

// animation
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonComponent.h"
#include "core-AI\SkeletonMapper.h"

// geometry
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\LitVertex.h"

// physics
#include "core-Physics\PhysicsSystem.h"
#include "core-Physics\PhysicsWorld.h"
#include "core-Physics\PhysicsMaterial.h"
#include "core-Physics\PxMathConverter.h"
#include "core-Physics\Defines.h"

// physx
#include "PxRigidDynamic.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxSceneLock.h"
#include "PxAggregate.h"
#include "extensions\PxSphericalJoint.h"
#include "extensions\PxRevoluteJoint.h"
#include "extensions\PxRigidBodyExt.h"
#include "geometry\PxCapsuleGeometry.h"

// tools
#include "core-MVC\EntityUtils.h"
#include "core-MVC\Entity.h"
#include "core\LocalList.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RagdollComponent );
   PARENT( PhysicsObject );
   PROPERTY_EDIT( "Ragdoll skeleton", Skeleton*, m_ragdollSkeleton );
   PROPERTY_EDIT( "Ragdoll mesh", TriangleMesh*, m_ragdollMesh );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RagdollComponent::RagdollComponent( const char* name )
   : PhysicsObject( name )
   , m_defaultMaterial( FilePath( "/Physics/Materials/defaultPhysMat.tpm" ) )
   , m_ragdollSkeleton( NULL )
   , m_ragdollMesh( NULL )
   , m_physicsWorld( NULL )
   , m_aggregate( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

RagdollComponent::RagdollComponent( const RagdollComponent& rhs )
   : PhysicsObject( rhs )
   , m_defaultMaterial( rhs.m_defaultMaterial )
   , m_ragdollSkeleton( rhs.m_ragdollSkeleton )
   , m_ragdollMesh( rhs.m_ragdollMesh )
   , m_physicsWorld( NULL )
   , m_aggregate( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

RagdollComponent::~RagdollComponent()
{
   ASSERT( m_mappers.empty() );
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onObservedPrePropertyChanged( ReflectionProperty& property )
{
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onObservedPropertyChanged( ReflectionProperty& property )
{
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   if ( m_ragdollSkeleton == NULL )
   {
      m_ragdollSkeleton = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onAttachToModel( Model* model )
{
   PhysicsObject::onAttachToModel( model );

   if ( m_ragdollSkeleton )
   {
      m_ragdollSkeleton->attachListener( *this );
   }

   // pre-load the default material
   m_defaultMaterial.loadResource();
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onDetachFromModel( Model* model )
{
   PhysicsObject::onDetachFromModel( model );

   if ( m_ragdollSkeleton )
   {
      m_ragdollSkeleton->detachListener( *this );
   }

   releaseSkeletonMappers();
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onPrePropertyChanged( ReflectionProperty& property )
{
   PhysicsObject::onPrePropertyChanged( property );

   if ( property.getName() == "m_ragdollSkeleton" )
   {
      destroyRagdoll();
      releaseSkeletonMappers();
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onPropertyChanged( ReflectionProperty& property )
{
   PhysicsObject::onPropertyChanged( property );

   if ( property.getName() == "m_ragdollSkeleton" )
   {
      buildRagdoll();
      buildSkeletonMappers();
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onSiblingAttached( SceneNode* node )
{
   PhysicsObject::onSiblingAttached( node );

   if ( node->isExactlyA< SkeletonComponent >() )
   {
      SkeletonComponent* skeletonComp = static_cast< SkeletonComponent* >( node );

      const SkeletonMapper* existingMapper = getSkeletonMapper( skeletonComp->m_skeleton );
      if ( !existingMapper && skeletonComp->m_skeleton )
      {
         SkeletonMapper* newMapper = new SkeletonMapper();
         newMapper->setSkeletons( m_ragdollSkeleton, skeletonComp->m_skeleton );

         // TODO: build the mapping table here

         m_mappers.push_back( newMapper );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::onSiblingDetached( SceneNode* node )
{
   PhysicsObject::onSiblingDetached( node );

   if ( node->isExactlyA< SkeletonComponent >() )
   {
      SkeletonComponent* skeletonComp = static_cast< SkeletonComponent* >( node );

      const SkeletonMapper* existingMapper = getSkeletonMapper( skeletonComp->m_skeleton );
      if ( existingMapper )
      {
         const uint idx = m_mappers.find( const_cast< SkeletonMapper* >( existingMapper ) );
         m_mappers.remove( idx );

         delete existingMapper;
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::addToWorld( PhysicsWorld& world )
{
   ASSERT( m_physicsWorld == NULL );

   m_physicsWorld = &world;
   buildRagdoll();
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::removeFromWorld( PhysicsWorld& world )
{
   ASSERT( m_physicsWorld != NULL );

   destroyRagdoll();
   m_physicsWorld = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::buildRagdoll()
{
   if ( !m_physicsWorld || !m_ragdollSkeleton || !m_ragdollMesh )
   {
      return;
   }

   PhysicsMaterial* defaultMat = m_defaultMaterial.get();
   if ( !defaultMat )
   {
      return;
   }

   // calculate the pose in model space
   const uint boneCount = m_ragdollSkeleton->getBoneCount();

   Array< AxisAlignedBox > bodiesBounds( boneCount );
   bodiesBounds.resize( boneCount, AxisAlignedBox() );
   calculateBodyPartBounds( bodiesBounds );

   m_bodyDescriptions.resize( boneCount, BodyDesc() );
   calculateBodyDescriptions( bodiesBounds, m_bodyDescriptions );

   createBodies( m_bodyDescriptions, defaultMat );
   createJoints( m_bodyDescriptions );

   // initialize runtime structures
   m_poseWorldSpace.resize( boneCount + 1, Transform::IDENTITY );
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::calculateBodyPartBounds( Array< AxisAlignedBox >& outBodiesBounds ) const
{
   const Array< VertexWeight >& vertexWeights = m_ragdollMesh->getVertexWeights();
   const Array< LitVertex >& vertices = m_ragdollMesh->getVertices();

   Vector vertexPosModelSpace, vertexPosLocalSpace;
   const uint verticesCount = vertexWeights.size();
   for ( uint i = 0; i < verticesCount; ++i )
   {
      // transform the vertex to bone's local space
      const LitVertex& vertexDef = vertices[i];
      vertexPosModelSpace.load<4>( vertexDef.m_coords );

      // see which bones are referencing this vertex
      const VertexWeight& weights = vertexWeights[i];
      for ( byte weightIdx = 0; weightIdx < 4; ++weightIdx )
      {
         const int boneIdx = ( int ) weights.m_indices[weightIdx];
         if ( boneIdx >= 0 )
         {
            const Matrix& modelToLocal = m_ragdollSkeleton->m_boneInvBindPoseMtx[boneIdx];
            modelToLocal.transform( vertexPosModelSpace, vertexPosLocalSpace );

            AxisAlignedBox& bounds = outBodiesBounds[boneIdx];
            bounds.include( vertexPosLocalSpace );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::calculateBodyDescriptions( const Array< AxisAlignedBox >& bodiesBounds, Array< BodyDesc >& outBodiesDescriptions ) const
{
   // define extra transforms
   Quaternion initialRotation[3];
   {
      initialRotation[0].setIdentity();
      initialRotation[1].setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( -90.0f ) ) );
      initialRotation[2].setAxisAngle( Vector_OY, FastFloat::fromFloat( DEG2RAD( -90.0f ) ) );
   }

   const uint boneCount = m_ragdollSkeleton->getBoneCount();

   Vector extents, center;
   Matrix bodyLocalSpaceMtx;
   for ( uint i = 0; i < boneCount; ++i )
   {
      BodyDesc& bodyDesc = outBodiesDescriptions[i];
      const AxisAlignedBox& bounds = bodiesBounds[i];

      bounds.getExtents( extents );
      if ( extents[0] > extents[1] )
      {
         if ( extents[1] > extents[2] )
         {
            ASSERT( extents[0] >= extents[1] && extents[1] >= extents[2] );
            bodyDesc.m_length = extents[0];
            bodyDesc.m_radius = extents[1] * 0.5f;

            // the body should stretch along the OX axis - no need to flip it then
            bodyDesc.m_transform.m_rotation = initialRotation[0];
         }
         else
         {
            if ( extents[0] > extents[2] )
            {
               ASSERT( extents[0] >= extents[2] && extents[2] >= extents[1] );
               bodyDesc.m_length = extents[0];
               bodyDesc.m_radius = extents[2] * 0.5f;

               // the body should stretch along the OX axis - no need to flip it then
               bodyDesc.m_transform.m_rotation = initialRotation[0];
            }
            else
            {
               ASSERT( extents[2] >= extents[0] && extents[0] >= extents[1] );
               bodyDesc.m_length = extents[2];
               bodyDesc.m_radius = extents[0] * 0.5f;

               // the body should stretch along the OZ axis, we need to flip it about the OY axis
               bodyDesc.m_transform.m_rotation = initialRotation[2];
            }
         }
      }
      else
      {
         if ( extents[2] > extents[1] )
         {
            ASSERT( extents[2] >= extents[1] && extents[1] >= extents[0] );
            bodyDesc.m_length = extents[2];
            bodyDesc.m_radius = extents[1] * 0.5f;

            // the body should stretch along the OZ axis, we need to flip it about the OY axis
            bodyDesc.m_transform.m_rotation = initialRotation[2];
         }
         else
         {
            if ( extents[2] > extents[0] )
            {
               ASSERT( extents[1] >= extents[2] && extents[2] >= extents[0] );
               bodyDesc.m_length = extents[1];
               bodyDesc.m_radius = extents[2] * 0.5f;

               // the body should stretch along the OY axis - we need to flip it about the OZ axis
               bodyDesc.m_transform.m_rotation = initialRotation[1];
            }
            else
            {
               ASSERT( extents[1] >= extents[0] && extents[0] >= extents[2] );
               bodyDesc.m_length = extents[1];
               bodyDesc.m_radius = extents[0] * 0.5f;

               // the body should stretch along the OY axis - we need to flip it about the OZ axis
               bodyDesc.m_transform.m_rotation = initialRotation[1];
            }
         }
      }

      bounds.getCenter( center );
      bodyDesc.m_transform.m_translation = center;

      // reduce the length to factor in the extra length the radius will introduce
      bodyDesc.m_length -= 2.0f * bodyDesc.m_radius;
      if ( bodyDesc.m_length < 0.0f )
      {
         bodyDesc.m_length = bodyDesc.m_radius;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::createBodies( Array< BodyDesc >& bodyDescriptions, PhysicsMaterial* material )
{
   PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
   physx::PxPhysics* physicsCore = physSystem.getPhysXInterface();

   physx::PxMaterial* pxMaterial = material->getPhysXMaterial();

   // calculate the transformation matrices of the bones and the character
   const uint boneCount = m_ragdollSkeleton->getBoneCount();
   Array< Transform > modelPose( boneCount ); modelPose.resize( boneCount, Transform::IDENTITY );
   m_ragdollSkeleton->calculateLocalToModel( m_ragdollSkeleton->m_boneLocalMatrices.getRaw(), modelPose.getRaw() );

   Transform modelToWorld;
   modelToWorld.set( getGlobalMtx() );

   // create an aggregate
   const bool selfCollisions = true;
   m_aggregate = physicsCore->createAggregate( boneCount, selfCollisions );

   Array< Transform > bodyTransforms( boneCount + 1 );
   bodyTransforms.resize( boneCount + 1, Transform::IDENTITY );
   bodyTransforms[0] = modelToWorld;

   // instantiate bones
   const float mass = 1.0f;
   const float density = 1.0f;
   Transform bodyModelSpace, bodyWorldSpace, bindPoseTransform;
   physx::PxTransform pxTransform;
   m_bodies.resize( boneCount, NULL );
   for ( uint i = 0; i < boneCount; ++i )
   {
      BodyDesc& bodyDescription = bodyDescriptions[i];

      bodyModelSpace.setMul( bodyDescription.m_transform, modelPose[i] );
      bodyWorldSpace.setMul( bodyModelSpace, modelToWorld );
      PxMathConverter::convert( bodyWorldSpace, pxTransform );

      bodyTransforms[i + 1] = bodyWorldSpace;

      physx::PxRigidDynamic* bone = physicsCore->createRigidDynamic( pxTransform );
      bone->setMass( mass );
      bone->setSolverIterationCounts( 8, 8 );

      physx::PxShape* shape = bone->createShape( physx::PxCapsuleGeometry( bodyDescription.m_radius, bodyDescription.m_length * 0.5f ), *pxMaterial );
      physx::PxRigidBodyExt::updateMassAndInertia( *bone, density );

      // setup the collision filter
      m_physicsWorld->assignCollisionGroup( bone, Collision_Ragdoll );

      // DEBUG code
      //bone->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, true );


      // keep local reference to it
      m_bodies[i] = bone;
      m_aggregate->addActor( *bone );
   }

   // calculate the bind pose
   Transform invBodyTransform;
   for ( uint i = 0; i < boneCount; ++i )
   {
      int parentIdx = m_ragdollSkeleton->m_boneParentIndices[i];
      invBodyTransform.setInverse( bodyTransforms[parentIdx + 1] );

      BodyDesc& bodyDescription = bodyDescriptions[i];
      bodyDescription.m_bindPoseTransform.setMul( bodyTransforms[i + 1], invBodyTransform );
      bodyDescription.m_bindPoseTransform.invert();
   }

   // add bones to the world
   {
      physx::PxSceneWriteLock scopedLock( *m_physicsWorld->m_scene );

      m_physicsWorld->m_scene->addAggregate( *m_aggregate );
   }

}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::createJoints( const Array< BodyDesc >& bodyDescriptions )
{
   physx::PxSceneWriteLock scopedLock( *m_physicsWorld->m_scene );

   PhysicsSystem& physSystem = TSingleton< PhysicsSystem >::getInstance();
   physx::PxPhysics* physicsCore = physSystem.getPhysXInterface();

   const uint boneCount = m_ragdollSkeleton->getBoneCount();
   m_joints.resize( boneCount - 1, NULL );

   physx::PxVec3 localFrame0Pos, localFrame1Pos;
   physx::PxQuat fromToQuat;
   bool baseContact;

   for ( uint i = 1; i < boneCount; ++i )
   {
      const int parentBoneIdx = m_ragdollSkeleton->m_boneParentIndices[i];
      const BodyDesc& parentBodyDescription = bodyDescriptions[parentBoneIdx];
      const BodyDesc& bodyDescription = bodyDescriptions[i];
      const Matrix& jointMtxBoneSpace = m_ragdollSkeleton->m_boneLocalMatrices[i];

      // frame 0 transform
      {
         Matrix convertedTransform;
         parentBodyDescription.m_transform.toMatrix( convertedTransform );

         Matrix invParentBodyMtx;
         invParentBodyMtx.setInverse( convertedTransform );

         Matrix jointMtxParentBodySpace;
         jointMtxParentBodySpace.setMul( jointMtxBoneSpace, invParentBodyMtx );

         PxMathConverter::convert( jointMtxParentBodySpace.position(), localFrame0Pos );

         // rotation   
         Quaternion ftQuat;
         jointMtxParentBodySpace.getRotation( ftQuat );
         PxMathConverter::convert( ftQuat, fromToQuat );
      }

      // frame 1 transform
      {
         Matrix convertedTransform;
         bodyDescription.m_transform.toMatrix( convertedTransform );

         Matrix invFrame1Mtx;
         invFrame1Mtx.setInverse( convertedTransform );

         PxMathConverter::convert( invFrame1Mtx.position(), localFrame1Pos );
      }

      // determine with which part the body is touching its parent
      {
         Vector a;
         bodyDescription.m_transform.transformNorm( Vector_OY, a );

         FastFloat cosBetweenVecs = a.dot( Vector_OY );
         cosBetweenVecs.abs();
         baseContact = ( cosBetweenVecs < Float_Inv2 );
      }

      // physx transforms and actors
      physx::PxTransform localFrame0( localFrame0Pos, fromToQuat );
      physx::PxTransform localFrame1( localFrame1Pos, fromToQuat );
      physx::PxRigidActor* parentBone = m_bodies[parentBoneIdx];
      physx::PxRigidActor* childBone = m_bodies[i];
   
      physx::PxJoint* joint = NULL;
      if ( baseContact )
      {
         // when the body is touching its parent with its base, use a spherical joint
         physx::PxSphericalJoint* sphericalJoint = PxSphericalJointCreate( *physicsCore, parentBone, localFrame0, childBone, localFrame1 );
         sphericalJoint->setLimitCone( physx::PxJointLimitCone( ( float ) M_PI * 0.8f, ( float ) M_PI * 0.8f, 0.1f ) );
         sphericalJoint->setSphericalJointFlag( physx::PxSphericalJointFlag::eLIMIT_ENABLED, true );

         joint = sphericalJoint;
      }
      else
      {
         // when the body is touching its parent with its ledge, use a revolute joint
         physx::PxRevoluteJoint* revoluteJoint = PxRevoluteJointCreate( *physicsCore, parentBone, localFrame0, childBone, localFrame1 );
         revoluteJoint->setLimit( physx::PxJointAngularLimitPair( -( float ) M_PI_4, ( float ) M_PI_4, 0.01f ) );
         revoluteJoint->setRevoluteJointFlag( physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true );

         joint = revoluteJoint;
      }

      ASSERT( joint != NULL );
      m_joints[i - 1] = joint;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::destroyRagdoll()
{
   if ( m_aggregate )
   {
      if ( m_physicsWorld )
      {
         physx::PxSceneWriteLock scopedLock( *m_physicsWorld->m_scene );
         m_physicsWorld->m_scene->removeAggregate( *m_aggregate );
      }

      m_aggregate->release();
      m_aggregate = NULL;
   }

   const uint bodyCount = m_bodies.size();
   for ( uint i = 0; i < bodyCount; ++i )
   {
      m_bodies[i]->release();
   }
   m_bodies.clear();
   
   const uint jointsCount = m_joints.size();
   for ( uint i = 0; i < jointsCount; ++i )
   {
      if ( m_joints[i] )
      {
         m_joints[i]->release();
      }
   }
   m_joints.clear();

   m_bodyDescriptions.clear();
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::calcPoseLocalSpace( Array< Transform >& outTransforms ) const
{
   if ( !m_physicsWorld )
   {
      return;
   }
   physx::PxSceneReadLock scopedLock( *m_physicsWorld->m_scene );

   // obtain the world space pose of the ragdoll
   m_poseWorldSpace[0].set( getGlobalMtx() );

   const uint bodyCount = m_bodies.size();
   for ( uint i = 0; i < bodyCount; ++i )
   {
      physx::PxRigidDynamic* body = m_bodies[i];
      PxMathConverter::convert( body->getGlobalPose(), m_poseWorldSpace[i + 1] );
   }

   // calculate the local space pose
   // Transform parentBoneSpace, childBoneSpace;
   Transform a, b, c;
   for ( uint i = 0; i < bodyCount; ++i )
   {
      int parentTransformIdx = m_ragdollSkeleton->m_boneParentIndices[i];

      a.setInverse( m_poseWorldSpace[parentTransformIdx + 1] );
      b.setMul( m_poseWorldSpace[i + 1], a );

      c.setMul( b, m_bodyDescriptions[i].m_bindPoseTransform );

      outTransforms[i] = c;
   }
}

///////////////////////////////////////////////////////////////////////////////

const SkeletonMapper* RagdollComponent::getSkeletonMapper( const Skeleton* skeleton ) const
{
   if ( !skeleton )
   {
      return NULL;
   }

   bool mapperExists = false;
   const uint mappersCount = m_mappers.size();
   for ( uint i = 0; i < mappersCount; ++i )
   {
      SkeletonMapper* mapper = m_mappers[i];
      if ( mapper->m_targetSkeleton == skeleton )
      {
         return mapper;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::buildSkeletonMappers()
{
   ASSERT( m_mappers.empty() );

   LocalList< const SkeletonComponent* > skeletonComponents;
   EntityUtils::collectNodesByType< SkeletonComponent >( getParent(), skeletonComponents );

   for ( LocalList< const SkeletonComponent* >::iterator it = skeletonComponents.begin(); !it.isEnd(); ++it )
   {
      const SkeletonComponent* skeletonComp = *it;
      if ( skeletonComp->m_skeleton != NULL )
      {
         SkeletonMapper* mapper = new SkeletonMapper();
         mapper->setSkeletons( m_ragdollSkeleton, skeletonComp->m_skeleton );

         // TODO: build the mapping table here

         m_mappers.push_back( mapper );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RagdollComponent::releaseSkeletonMappers()
{
   // delete skeleton mappers
   const uint mappersCount = m_mappers.size();
   for ( uint i = 0; i < mappersCount; ++i )
   {
      delete m_mappers[i];
   }
   m_mappers.clear();
}

///////////////////////////////////////////////////////////////////////////////
