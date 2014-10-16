/// @file   TamyEditor\SceneEditor.h
/// @brief  Simple editor for opening a new scene in the editor
#pragma once

#include "BaseSceneEditor.h"


///////////////////////////////////////////////////////////////////////////////

class Model;

///////////////////////////////////////////////////////////////////////////////

/**
 * Simple editor for opening a new scene in the editor.
 *
 * the class implements TimeDependent because it needs to be ticked every frame in order
 * to respond to the input coming from the scene rendering widget
 */
class SceneEditor : public BaseSceneEditor
{
   Q_OBJECT  

private:
   Model&                                    m_scene;

public:
   /**
    * Constructor.
    *
    * @param scene      scene to be edited
    */
   SceneEditor( Model& scene );
   ~SceneEditor();

   /**
    * Returns the edited scene.
    */
   inline Model& getScene() const { return m_scene; }
};

///////////////////////////////////////////////////////////////////////////////
