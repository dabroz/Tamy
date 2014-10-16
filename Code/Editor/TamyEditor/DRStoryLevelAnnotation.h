/// @file   TamyEditor/DRStoryLevelAnnotation.h
/// @brief  debug representation of a story level annotation
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class StoryLevelAnnotation;
class GeometryComponent;
class TriangleMesh;
class RenderState;

///////////////////////////////////////////////////////////////////////////////

class DRStoryLevelAnnotation : public DebugGeometry
{
   DECLARE_ALLOCATOR( DRStoryLevelAnnotation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   StoryLevelAnnotation&         m_annotation;

   TriangleMesh*                 m_mesh;
   DebugGeometryComponent*       m_geomComp;

public:
   /**
   * Constructor.
   *
   * @param annotation
   */
   DRStoryLevelAnnotation( StoryLevelAnnotation& annotation );
   ~DRStoryLevelAnnotation();

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );
   void updateDebugData( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
