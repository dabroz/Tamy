/// @file   TamyEditor/PrefabEditor.h
/// @brief  editor of Prefab resources
#pragma once

#include "BaseSceneEditor.h"
#include <QtWidgets\QAction>
#include "core\TimeDependent.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Camera;
class Entity;
class DirectionalLight;

///////////////////////////////////////////////////////////////////////////////

/**
 * Editor of Prefab resources.
 */
class PrefabEditor : public BaseSceneEditor, public TimeDependent
{
   Q_OBJECT

private:
   Prefab&                                   m_prefab;
   Model*                                    m_scene;
   Entity*                                   m_prefabbedEntity;

   bool                                      m_lightLocked;
   bool                                      m_additionaLightsEnabled;
   Camera*                                   m_camera;
   Entity*                                   m_frontLight;
   DirectionalLight*                         m_frontLightComp;
   Entity*                                   m_backLight;
   DirectionalLight*                         m_backLightComp;

   QAction*                                  m_actionLockLight;
   QAction*                                  m_actionToggleAdditionalLights;

public:
   /**
    * Constructor.
    *
    * @param prefab
    */
   PrefabEditor( Prefab& prefab );
   ~PrefabEditor();

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

   // -------------------------------------------------------------------------
   // BaseSceneEditor implementation
   // -------------------------------------------------------------------------
   void onPreSaveResource();

public slots:
   void lockLight();
   void toggleAdditionalLights();

private:
   void initializeScene();
};

///////////////////////////////////////////////////////////////////////////////
