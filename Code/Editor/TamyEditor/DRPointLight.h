/// @file   TamyEditor/DRPointLight.h
/// @brief  debug representation of a point light
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class PointLight;
class GeometryComponent;
class TriangleMesh;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DRPointLight : public DebugGeometry
{
   DECLARE_ALLOCATOR( DRPointLight, AM_DEFAULT );
   DECLARE_CLASS()

private:
   PointLight&             m_light;

   TriangleMesh*           m_mesh;

   DebugGeometryComponent* m_geomComp;

public:
   /**
    * Constructor.
    *
    * @param light
    */
   DRPointLight( PointLight& light );
   ~DRPointLight();

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );
   void updateDebugData( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
