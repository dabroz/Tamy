/// @file   TamyEditor/DebugGrid.h
/// @brief  debug grid
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class RenderState;
class TriangleMesh;
class VertexShader;

///////////////////////////////////////////////////////////////////////////////

/**
 * Debug grid entity.
 */
class DebugGrid : public DebugGeometry
{
   DECLARE_CLASS()

private:
   TriangleMesh*                    m_mesh;

   DebugGeometryComponent*          m_geomComp;

public:
   /**
    * Constructor.
    */
   DebugGrid();
   ~DebugGrid();

   /**
    * Shows/hides the grid.
    *
    * @param enable
    */
   void show( bool enable );
};

///////////////////////////////////////////////////////////////////////////////
