#include "SceneEditor.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "ext-RenderingPipeline\LocationRenderSettings.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\CubeTexture.h"
#include "core\MatrixUtils.h"


///////////////////////////////////////////////////////////////////////////////

SceneEditor::SceneEditor( Model& scene )
   : BaseSceneEditor( scene )
   , m_scene( scene )
{
   initializeBase( m_scene.getRoot(), &m_scene );
}

///////////////////////////////////////////////////////////////////////////////

SceneEditor::~SceneEditor()
{
}


///////////////////////////////////////////////////////////////////////////////

void SceneEditor::onInitialize()
{
   BaseSceneEditor::onInitialize();

   // if the scene we're editing is completely empty, initialize it with default contents
   if ( m_scene.getRoot()->m_children.empty() )
   {
      createDefaultScene();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneEditor::createDefaultScene()
{
   Entity* sceneRoot = m_scene.getRoot();

   Entity* defaultSettings = new Entity( "DefaultSettings" );
   sceneRoot->addChild( defaultSettings );

   Entity* defaultLight = new Entity( "DefaultLight" );
   {
      defaultSettings->addChild( defaultLight );

      DirectionalLight* defaultLightComp = new DirectionalLight();
      defaultLight->addChild( defaultLightComp );
      MatrixUtils::generateLookAtLH( Vector( 10000, 10000, 10000 ), Vector( 0, 0, 0 ), Vector_OZ, defaultLight->accessLocalMtx() );
   }

   LocationRenderSettings* renderSettings = new LocationRenderSettings();
   {
      defaultSettings->addChild( renderSettings );
      renderSettings->m_ambientLight = Color( 0.6f, 0.6f, 0.6f, 1.0f );

      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
      renderSettings->setSkybox( resMgr.create< CubeTexture >( FilePath( "/Renderer/Materials/SkyBox_1/skyBox_1.ctex" ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
