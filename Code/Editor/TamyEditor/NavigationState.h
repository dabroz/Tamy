/// @file      TamyEditor/NavigationState.h
/// @brief     SceneObjectsManipulator navigation state
#pragma once

#include "core-AI\FSMState.h"
#include "SceneObjectsManipulator.h"
#include "core-Renderer\RendererSelectionListener.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class UserInputController;
class Entity;
class Renderer;
class TamyCameraController;

///////////////////////////////////////////////////////////////////////////////

/**
 * SceneObjectsManipulator navigation state.
 *
 * Allows to select objects by pressing Left Mouse Button twice on
 * the desired object or to switch to manipulation mode by pressing
 * and holding the Left Mouse Button and moving the mouse around.
 */
class NavigationState : public FSMState< SceneObjectsManipulator >, public RendererSelectionListener
{
   DECLARE_CLASS()

public:
   enum CameraControlMode
   {
      CCM_ORBIT,
      CCM_FREE,
   };

private:
   enum MovementDirection
   {
      MD_LEFT,
      MD_RIGHT,
      MD_FRONT,
      MD_BACK
   };

   enum SelectionQueryMode
   {
      SM_NONE,
      SM_NODES,
      SM_GIZMO,
   };

private:
   SelectionQueryMode                     m_selectionQueryMode;
   bool                                   m_objectsSelectionEnabled;

   TamySceneWidget*                       m_sceneWidget;
   UserInputController*                   m_uic;

   Renderer*                              m_renderer;

   // camera controller
   TamyCameraController*                  m_cameraController;

   // camera movement state
   bool                                   m_rotating;
   bool                                   m_movementDir[4];

public:
   /**
    * Constructor.
    */
   NavigationState();
   ~NavigationState();

   /**
    * Initializes the input controller this state uses to check the user's input.
    *
    * @param widget
    */
   void initInput( TamySceneWidget& widget );

   /**
    * Changes the camera control mode.
    *
    * @param mode
    */
   void setCameraControlMode( CameraControlMode mode );

   /**
    * Enables/disables scene objects selection.
    */
   void enableObjectsSelection( bool enable );

   // -------------------------------------------------------------------------
   // RendererSelectionListener implementation
   // -------------------------------------------------------------------------
   void onEntitiesSelected( const List< Entity* >& entities );

   // ----------------------------------------------------------------------
   // FSMState implementation
   // ----------------------------------------------------------------------
   void activate();
   void deactivate();
   void execute( float timeElapsed );

private:
   void updateCamera( float timeElapsed );
};

///////////////////////////////////////////////////////////////////////////////
