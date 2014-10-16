/// @file   core-Renderer\Light.h
/// @brief  base class and render commands for lights
#pragma once

#include "core-MVC\Component.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for the light.
 */
class Light : public Component
{
   DECLARE_ALLOCATOR( Light, AM_DEFAULT );
   DECLARE_CLASS();

public:
   enum LightType
   {
      LT_Directional,
      LT_Point,
      LT_Spot
   };

public:
   bool                                   m_castsShadows;
   LightType                              m_lightType;
   bool                                   m_enabled;

protected:
   // runtime data
   AxisAlignedBox                         m_boundingVol;
   AxisAlignedBox                         m_worldSpaceBounds;

public:
   /**
    * Constructor.
    *
    * @param type
    * @param name
    */
   Light( LightType type, const char* name = "" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Light( const Light& rhs );

   virtual ~Light() {}

   /**
    * Returns type of this light.
    */
   inline LightType getType() const { return m_lightType; }

   /**
    * Enables or disables the shadows cast by this light.
    *
    * @param enable
    */
   inline void setShadowsCaster( bool enable ) { m_castsShadows = enable; }

   /**
    * Tells if the light is set to cast shadows.
    */
   inline bool castsShadows() const { return m_castsShadows; }

   /**
    * Turns the light on/off.
    */
   inline void setEnabled( bool enabled ) {
      m_enabled = enabled;
   }

   /**
   * Is the light shining?
   */
   inline bool isEnabled() const {
      return m_enabled;
   }

   // -------------------------------------------------------------------------
   // BoundingVolume management
   // -------------------------------------------------------------------------
   /**
    * Calculates a bounding box around the geometry ( in world space coordinates ).
    *
    * @param boundingBox
    */
   void getBoundingBox( AxisAlignedBox& boundingBox ) const;

   /**
    * Returns the bounding volume around the entity that owns this component.
    */
   const AxisAlignedBox& getBoundingVolume() const {
      return m_worldSpaceBounds;
   }

protected:
   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void updateTransforms() override;
};

///////////////////////////////////////////////////////////////////////////////
