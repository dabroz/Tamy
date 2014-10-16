/// @file   TamyEditor\DRSpotLight.h
/// @brief  spot light debug geometry
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class SpotLight;
class GeometryComponent;
class TriangleMesh;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DRSpotLight : public DebugGeometry
{
   DECLARE_ALLOCATOR( DRSpotLight, AM_DEFAULT );
   DECLARE_CLASS()

private:
   SpotLight&              m_light;

   TriangleMesh*           m_mesh;

   DebugGeometryComponent* m_geomComp;

public:
   /**
   * Constructor.
   *
   * @param light
   */
   DRSpotLight( SpotLight& light );
   ~DRSpotLight();

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );
   void updateDebugData( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
