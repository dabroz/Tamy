/// @file      TamyEditor/GizmoNodeManipulationState.h
/// @brief     SceneObjectsManipulator node manipulation state that manipulates nodes using a selected gizmo axis
#pragma once

#include "core-AI\FSMState.h"
#include "SceneObjectsManipulator.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

enum NodeTransformControlMode;
class UserInputController;
class TamySceneWidget;
class Renderer;
class Camera;
class GizmoAxis;
class Entity;
class SceneNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * SceneObjectsManipulator node manipulation state that manipulates nodes 
 * using a selected gizmo axis.
 */
class GizmoNodeManipulationState : public FSMState< SceneObjectsManipulator >
{
   DECLARE_CLASS()

private:
   enum MovementDirection
   {
      MD_LEFT,
      MD_RIGHT,
      MD_FRONT,
      MD_BACK
   };

private:
   TamySceneWidget*                    m_sceneWidget;
   UserInputController*                m_uic;
   Renderer*                           m_renderer;
   Camera*                             m_camera;

   Matrix                              m_gizmoBindMtx;
   GizmoAxis*                          m_selectedGizmoAxis;

   bool                                m_movementDir[4];

   Matrix                              m_gizmoBaseTransform;
   Vector                              m_viewportSpaceTransformation;
   Matrix                              m_prevNodesTransformation;

public:
   /**
    * Constructor.
    */
   GizmoNodeManipulationState();
   ~GizmoNodeManipulationState();

   /**
    * Initializes the input controller this state uses to check the user's input.
    *
    * @param widget
    */
   void initInput( TamySceneWidget& widget );

   /**
    * Sets a pointer to the axis we should use to manipulate objects.
    *
    * @param axis
    */
   void setGizmoAxis( GizmoAxis* axis ) { m_selectedGizmoAxis = axis; }

   // ----------------------------------------------------------------------
   // FSMState implementation
   // ----------------------------------------------------------------------
   void activate();
   void deactivate();
   void execute( float timeElapsed );

private:
   void setupController();
   void update( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
