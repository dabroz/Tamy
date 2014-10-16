/// @file   TamyEditor/GizmoAxis.h
/// @brief  a single axis of the nodes manipulation gizmo
#pragma once

#include "DebugGeometry.h"


///////////////////////////////////////////////////////////////////////////////

class Camera;
class VertexShader;
class GeometryResource;
class GizmoAxisGeometryComponent;

///////////////////////////////////////////////////////////////////////////////

/**
 * A single axis of the nodes manipulation gizmo.
 */
class GizmoAxis : public DebugGeometry
{
   DECLARE_CLASS()

public:
   enum AxisId
   {
      OX,
      OY,
      OZ,
      UNKNOWN,
   };

private:
   byte                                m_axisIdx;

   GizmoAxisGeometryComponent*         m_geomComp;

public:
   /**
    * Constructor.
    *
    * @param axisIdx
    * @param editedNode
    */
   GizmoAxis( byte axisIdx, Entity& editedNode );
   ~GizmoAxis();

   /**
    * Returns axis idx
    */
   inline byte getAxisId() const { return m_axisIdx; }

   /**
    * Sets a new mesh that will represent this axis.
    *
    * @param mesh
    */
   void setMesh( GeometryResource& mesh );

   /**
    * Shows/hides the axis.
    *
    * @param visible
    */
   void show( bool visible );
};

///////////////////////////////////////////////////////////////////////////////

class GizmoAxisGeometryComponent : public GeometryComponent
{
   DECLARE_CLASS();

private:
   Entity&                             m_editedNode;
public:
   /**
    * Constructor.
    *
    * @param editedNode
    */
   GizmoAxisGeometryComponent( Entity& editedNode );
};

///////////////////////////////////////////////////////////////////////////////
