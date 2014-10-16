/// @file   TamyEditor/DebugGeometry.h
/// @brief  a helper geometry class for rendering simple debug shapes
#pragma once

#include "core-MVC\Entity.h"
#include "core-Renderer\GeometryComponent.h"
#include "core\ResourceHandle.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class DebugGeometryComponent;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DebugGeometry : public Entity
{
   DECLARE_CLASS();

private:
   SceneNode*                             m_representedNodePtr;

public:
   /**
    * Constructor.
    *
    * @param representedNodePtr         pointer to the represented sceneNode
    */
   DebugGeometry( SceneNode* representedNodePtr );
   ~DebugGeometry();

   /**
    * Put the update code that needs to be called every frame here.
    *
    * @param timeElapsed
    */
   virtual void updateDebugData( float timeElapsed ) {}

   /**
    * Sets a pointer to the represented entity.
    *
    * @param ptr
    */
   inline void setRepresentedNodePtr( SceneNode* ptr ) { m_representedNodePtr = ptr; }

   /**
    * Returns a pointer to the represented entity.
    */
   inline SceneNode* getRepresentedNodePtr() { return m_representedNodePtr; }

   // -------------------------------------------------------------------------
   // Debug features controlled by the host DebugEntitiesManager
   // -------------------------------------------------------------------------
   /**
    * Shows the bounding box of the object.
    */
   virtual void enableBoundingBox( bool enable ) {}
   
   /**
    * Shows the assigned debug shape.
    */
   virtual void enableDebugShape( bool enable ) {}

};

///////////////////////////////////////////////////////////////////////////////

class DebugGeometryComponent : public GeometryComponent
{
   DECLARE_CLASS();

private:
   Matrix                              m_extraTransform;

public:
   /**
    * Constructor.
    */
   DebugGeometryComponent();

   /**
    * Constructor.
    *
    * @param mesh
    */
   DebugGeometryComponent( GeometryResource& mesh );

   /**
    * Sets an extra transform that should be applied to this geometry before rendering it.
    *
    * @param transform
    */
   inline void setExtraTransform( const Matrix& transform ) { m_extraTransform = transform; }

   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   const Matrix& getGlobalMtx() const {
      return m_extraTransform;
   }
};

///////////////////////////////////////////////////////////////////////////////
