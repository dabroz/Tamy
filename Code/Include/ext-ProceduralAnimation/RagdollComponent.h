/// @file   ext-ProceduralAnimation\RagdollComponent.h
/// @brief  a component that creates a ragdoll for the specified character
#pragma once

#include "core-Physics\PhysicsObject.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\ResourceHandle.h"
#include "core\Array.h"
#include "core\Matrix.h"
#include "core\Transform.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxRigidDynamic;
   class PxJoint;
   class PxAggregate;
}

class Skeleton;
class SkeletonMapper;
class PhysicsWorld;
class PhysicsMaterial;
class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

class RagdollComponent : public PhysicsObject, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( RagdollComponent, AM_DEFAULT );
   DECLARE_CLASS();

private:
   struct BodyDesc
   {
      DECLARE_ALLOCATOR( BodyDesc, AM_DEFAULT );

      Transform      m_transform;
      float          m_radius;
      float          m_length;

      BodyDesc()
         : m_radius( 0.0f )
         , m_length( 0.0f )
      {}
   };

public:
   // static data
   SkeletonMapper*                           m_mapper;

private:
   // static data
   Skeleton*                                 m_ragdollSkeleton;
   TriangleMesh*                             m_ragdollMesh;

   // runtime
   TResourceHandle< PhysicsMaterial >        m_defaultMaterial;
   PhysicsWorld*                             m_physicsWorld;
   
   physx::PxAggregate*                       m_aggregate;
   Array< physx::PxRigidDynamic* >           m_bodies;
   Array< physx::PxJoint* >                  m_joints;
   Array< BodyDesc >                         m_bodyDescriptions;

   // The following are mutable - they are used in a single constant method, calcPoseLocalSpace, and as a matter of fact,
   // they are local to that method ( the data the method puts in them becomes invalid once the method is over ).
   // We store them as class members, because we don't want to reallocate the matrices every time the method is called,
   // which is very often.
   mutable Array< Transform >                m_poseWorldSpace;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   RagdollComponent( const char* name = "RagdollComponent" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   RagdollComponent( const RagdollComponent& rhs );
   ~RagdollComponent();

   /**
    * Returns the ragdoll skeleton.
    */
   inline const Skeleton* getSkeleton() const {
      return m_ragdollSkeleton;
   }

   /**
    * Returns an array of ragdoll bodies.
    */
   inline const Array< physx::PxRigidDynamic* >& getBodies() const {
      return m_bodies;
   }

   /**
    * Calculates the pose of the ragdoll, expressed as a list of transforms,
    * each of which corresponding to a local space transform of a single ragdoll body.
    *
    * @param outTransforms
    */
   void calcPoseLocalSpace( Array< Transform >& outTransforms ) const;

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPrePropertyChanged( ReflectionProperty& property ) override;
   void onObservedPropertyChanged( ReflectionProperty& property ) override;
   void onObservedObjectDeleted( ReflectionObject* deletedObject ) override;

protected:
   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property ) override;
   void onPropertyChanged( ReflectionProperty& property ) override;

   // -------------------------------------------------------------------------
   // PhysicsObject implementation
   // -------------------------------------------------------------------------
   void addToWorld( PhysicsWorld& world ) override;
   void removeFromWorld( PhysicsWorld& world ) override;

private:
   void buildRagdoll();
   void destroyRagdoll();
   void calculateBodyPartBounds( Array< AxisAlignedBox >& outBodiesBounds ) const;
   void calculateBodyDescriptions( const Array< AxisAlignedBox >& bodiesBounds, Array< BodyDesc >& outBodiesDescriptions ) const;
   void createBodies( Array< BodyDesc >& bodyDescriptions, PhysicsMaterial* material );
   void createJoints( const Array< BodyDesc >& bodyDescriptions );
};

///////////////////////////////////////////////////////////////////////////////
