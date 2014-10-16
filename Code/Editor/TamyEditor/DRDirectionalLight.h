/// @file   TamyEditor/DRDirectionalLight.h
/// @brief  debug representation of a directional light
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class DirectionalLight;
class GeometryComponent;
class TriangleMesh;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DRDirectionalLight : public DebugGeometry
{
   DECLARE_ALLOCATOR( DRDirectionalLight, AM_DEFAULT );
   DECLARE_CLASS()

private:
   DirectionalLight&       m_light;

   TriangleMesh*           m_mesh;

   DebugGeometryComponent* m_geomComp;

public:
   /**
    * Constructor.
    *
    * @param light
    */
   DRDirectionalLight( DirectionalLight& light );
   ~DRDirectionalLight();

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );
   void updateDebugData( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
