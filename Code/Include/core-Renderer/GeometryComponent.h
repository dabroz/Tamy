/// @file   core-Renderer\GeometryComponent.h
/// @brief  3D geometry representation
#pragma once

#include "core-MVC\Component.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\AxisAlignedBox.h"

enum RenderProfileId
{
   RP_Default,
   RP_Selection,           // this context requires the Renderer::RenderingContext::m_color value to be set
   RP_SelectionQuery,
   RP_ShadowMap,           // make sure to set a new camera on the stack that will represent the shadow caster
};

///////////////////////////////////////////////////////////////////////////////

class GeometryResource;
class RenderState;
class Renderer;
class VertexShaderConfigurator;
class RCBindVertexShader;
class RCBindShader;
class Material;

///////////////////////////////////////////////////////////////////////////////

/**
 * A common interface for 3D geometry which can be rendered.
 */
class GeometryComponent : public Component
{
   DECLARE_ALLOCATOR( GeometryComponent, AM_DEFAULT );
   DECLARE_CLASS()

protected:
   // static data
   GeometryResource*                      m_resource;
   RenderState*                           m_renderState;
   bool                                   m_shadowCaster;

   // runtime data
   AxisAlignedBox                         m_boundingVol;
   AxisAlignedBox                         m_worldSpaceBounds;

   bool                                   m_enabled;        // tells if the geometry will be rendered or not

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   GeometryComponent( const char* name = "GeometryComponent" );

   /**
    * Copy constructor.
    */
   GeometryComponent( const GeometryComponent& rhs );

   /**
    * Constructor.
    *
    * @param resource   shared geometry resource
    * @param name
    */
   GeometryComponent( GeometryResource& resource, const char* name = "" );
   virtual ~GeometryComponent();

   /**
    * Renders the geometry using the material assigned to the component's render state.
    *
    * @param renderer
    * @param profile
    */
   void render( Renderer& renderer, RenderProfileId profile );

   /**
    * Toggles rendering of the node on/off.
    *
    * @param enable
    */
   inline void enableRendering( bool enable ) {
      m_enabled = enable;
   }

   // -------------------------------------------------------------------------
   // Render states management
   // -------------------------------------------------------------------------

   /**
    * Sets the specified rendering material.
    *
    * @param material
    */
   void setMaterial( Material* material );

   /**
    * Returns a vector of the render states that affect the geometry.
    */
   inline const RenderState* getRenderState() const { return m_renderState; }

   /**
    * Returns a vector of the render states that affect the geometry ( non-const ).
    */
   inline RenderState* accessRenderState() {
      return m_renderState;
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
   const AxisAlignedBox& getBoundingVolume() const { return m_worldSpaceBounds; }

   // -------------------------------------------------------------------------
   // GeometryComponent resource management
   // -------------------------------------------------------------------------
   /**
    * Sets the new geometry resource.
    *
    * @param 
    */
   void setMesh( GeometryResource& mesh );

   /**
    * Returns the name of the geometry resource used by this geometry entity.
    */
   std::string getMeshName() const;

   /**
    * Tells if the entity has a geometry resource attached.
    */
   inline bool hasMesh() const { return m_resource != NULL; }

   /**
    * Returns the attached geometry resource.
    */
   inline GeometryResource* getMesh() const { return m_resource; }

   /**
    * Makes the geometry cast shadows or not.
    *
    * @param enable
    */
   void setShadowcaster( bool enable ) {
      m_shadowCaster = enable;
   }

   /**
    * Returns the geometry flags.
    */
   inline bool isShadowcaster() const {
      return m_shadowCaster;
   }

protected:
   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void onAttached( Entity* parent ) override;
   void onDetached( Entity* parent ) override;
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;
   void onObjectLoaded() override;
   void updateTransforms() override;
};

///////////////////////////////////////////////////////////////////////////////
