/// @file   core-Physics\PhysicsMaterial.h
/// @brief  physics material
#pragma once

#include "core\Resource.h"


///////////////////////////////////////////////////////////////////////////////

namespace physx
{
   class PxMaterial;
};

///////////////////////////////////////////////////////////////////////////////

class PhysicsMaterial : public Resource
{
   DECLARE_ALLOCATOR( PhysicsMaterial, AM_DEFAULT );
   DECLARE_RESOURCE();

private:
   // static data
   float                                        m_staticFriction;
   float                                        m_dynamicFriction;
   float                                        m_restitution;

   // runtime data
   physx::PxMaterial*                           m_material;

public:
   /**
    * Constructor.
    *
    * @param path
    */
   PhysicsMaterial( const FilePath& path = FilePath() );
   ~PhysicsMaterial();

   /**
   * Sets the new static friction of the body.
   *
   * @param friction
   */
   void setStaticFriction( float friction );

   /**
   * Returns the dynamic friction of the body.
   */
   inline float getStaticFriction() const {
      return m_staticFriction;
   }

   /**
   * Sets the new static friction of the body.
   *
   * @param friction
   */
   void setDynamicFriction( float friction );

   /**
   * Returns the dynamic friction of the body.
   */
   inline float getDynamicFriction() const {
      return m_dynamicFriction;
   }

   /**
   * Sets the new restitution of the body.
   *
   * @param restitution
   */
   void setRestitution( float restitution );

   /**
   * Returns the restitution of the body.
   */
   inline float getRestitution() const {
      return m_restitution;
   }

   // -------------------------------------------------------------------------
   // PhysX material management
   // -------------------------------------------------------------------------
   /**
    * Creates a PhysX material implementation.
    */
   void createPhysXMaterial();

   /**
    * Releases the PhysX material implementation.
    */
   void releasePhysXMaterial();

   /**
   * Returns the PhysX material represented by this resource.
   */
   inline physx::PxMaterial* getPhysXMaterial() const {
      return m_material;
   }

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& mgr );
};

///////////////////////////////////////////////////////////////////////////////
