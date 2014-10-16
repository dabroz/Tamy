/// @file   TamyEditor\Gizmo.h
/// @brief  A selected object manipulation gizmo.
#pragma once

#include "core\Color.h"
#include "core-MVC\Entity.h"
#include "core-Renderer\RenderState.h"
#include "core\TimeDependent.h"
#include "core\ResourceHandle.h"
#include "core\List.h"
#include "SelectionManagerListener.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class GizmoAxis;
class GizmoMaterial;
class TriangleMesh;
class Camera;
class Entity;
class SelectionManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * A gizmo showing the selected object's orientation and indicating 
 * the selected manipulation mode ( moving, rotating, scaling ).
 */
class Gizmo : public Entity, public SelectionManagerListener
{
   DECLARE_CLASS()

public:
   enum Mode
   {
      GM_TRANSLATION,
      GM_ROTATION,
      GM_SCALING
   };

   enum GizmoReferenceFrame
   {
      GRF_Local,
      GRF_World,
   };

private:
   SelectionManager*                m_selectionMgr;

   Mode                             m_mode;
   GizmoReferenceFrame              m_gizmoReferenceFrame;


   AxisAlignedBox                   m_boundingVolume;
   TriangleMesh*                    m_meshes[3];
   GizmoAxis*                       m_geometry[3];

   // selected entities management
   List< Entity* >                  m_selectedRootEntities;


public:
   /**
    * Constructor.
    *
    * @param selectionMgr
    * @param mode             mode in which the gizmo should work
    * @param refFrame
    */
   Gizmo( SelectionManager* selectionMgr, Mode mode, GizmoReferenceFrame refFrame );
   ~Gizmo();

   /**
    * Sets a new mode in which the gizmo will be displayed.
    *
    * @param mode
    */
   void setMode( Mode mode );

   /**
    * Changes the reference system the gizmo operates in.
    *
    * @param refFrame
    */
   inline void setReferenceFrame( GizmoReferenceFrame refFrame ) {
      m_gizmoReferenceFrame = refFrame;
   }

   /**
    * Shows/hides the gizmo.
    *
    * @param visible
    */
   void show( bool visible );

   /**
    * Returns a list of currently selected root entities.
    */
   inline const List< Entity* >& getManipulatedEntities() const {
      return m_selectedRootEntities;
   }

   /**
    * Updates the gizmo.
    *
    * @param timeElapsed
    */
   void update( float timeElapsed );

   // ----------------------------------------------------------------------
   // SelectionManagerListener implementation
   // ----------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );

private:
   void createTranslationGizmo();
   void createRotationGizmo();
   void createScalingGizmo();

   bool updateSelectedNodesList( const List< SceneNode* >& nodes );
   void calculateSelectionBounds( AxisAlignedBox& outBounds ) const;
};

///////////////////////////////////////////////////////////////////////////////
