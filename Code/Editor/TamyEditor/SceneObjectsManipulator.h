/// @file   TamyEditor/SceneObjectsManipulator.h
/// @brief  tool for manual manipulation of the scene objects
#pragma once

#include "core-AI\FSMController.h"
#include "SelectionManagerListener.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

enum NodeTransformControlMode;
class Entity;
class SceneEditor;
class TamySceneWidget;
class Entity;
struct AxisAlignedBox;

///////////////////////////////////////////////////////////////////////////////

class SceneObjectsManipulator : public FSMController< SceneObjectsManipulator >
{
   DECLARE_CLASS()

private:
   TamySceneWidget*                    m_widget;
   

public:
   /**
    * Constructor.
    */
   SceneObjectsManipulator();
   ~SceneObjectsManipulator();

   /**
    * Initializes the controller.
    */
   void initialize( TamySceneWidget& sceneRenderingWidget );

   /**
    * Enables/disables objects manipulation and selection abilities.
    */
   void enable( bool flag );
};

///////////////////////////////////////////////////////////////////////////////
