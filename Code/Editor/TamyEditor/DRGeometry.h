/// @file   TamyEditor/DRGeometry.h
/// @brief  debug representation of an entity
#pragma once

#include "DebugGeometry.h"

///////////////////////////////////////////////////////////////////////////////

class TriangleMesh;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DRGeometry : public DebugGeometry
{
   DECLARE_ALLOCATOR( DRGeometry, AM_DEFAULT );
   DECLARE_CLASS()

private:
   Entity&                          m_entity;

   TriangleMesh*                    m_mesh;

   DebugGeometryComponent*          m_geomComp;

   bool                             m_visible;

public:
   /**
    * Constructor.
    *
    * @param entity
    */
   DRGeometry( Entity& entity );
   ~DRGeometry();

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );
   void updateDebugData( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
