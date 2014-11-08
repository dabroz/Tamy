#include "PrefabEditor.h"
#include "core-MVC\Model.h"
#include "core-MVC\Prefab.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\CubeTexture.h"
#include "ext-RenderingPipeline\LocationRenderSettings.h"
#include "core\MatrixUtils.h"

// widgets
#include <QtWidgets\QToolBar>
#include "TamySceneWidget.h"



///////////////////////////////////////////////////////////////////////////////

PrefabEditor::PrefabEditor( Prefab& prefab )
   : BaseSceneEditor( prefab )
   , m_prefab( prefab )
   , m_scene( NULL )
   , m_prefabbedEntity( NULL )
   , m_lightLocked( true )
   , m_additionaLightsEnabled( true )
   , m_camera( NULL )
   , m_frontLight( NULL )
   , m_backLight( NULL )
{
   initializeScene();
   initializeBase( m_prefabbedEntity, m_scene );
}

///////////////////////////////////////////////////////////////////////////////

PrefabEditor::~PrefabEditor()
{
   m_scene->removeReference();
   m_scene = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::initializeScene()
{
   m_scene = new Model();

   // instantiate the prefabbed entity, but switch its prefabbed flags off
   {
      m_prefabbedEntity = new Entity( "PrefabbedEntity" );
      m_prefabbedEntity->setPrefab( &m_prefab );
      
      m_scene->addChild( m_prefabbedEntity );
      m_prefabbedEntity->makeUnique();
   }

   // add the lights
   {
      Entity* sceneRoot = m_scene->getRoot();

      m_frontLight = new Entity( "frontLight" );
      {
         sceneRoot->addChild( m_frontLight );

         m_frontLightComp = new DirectionalLight();
         m_frontLight->addChild( m_frontLightComp );
         MatrixUtils::generateLookAtLH( Vector( 10000, 10000, 10000 ), Vector( 0, 0, 0 ), Vector_OZ, m_frontLight->accessLocalMtx() );
      }

      m_backLight = new Entity( "backLight" );
      {
         sceneRoot->addChild( m_backLight );

         m_backLightComp = new DirectionalLight();
         m_backLight->addChild( m_backLightComp );
         m_backLightComp->m_strength = 0.7f;
         m_backLightComp->m_color = Color( 0.7f, 0.7f, 1.0f, 1.0f );
         MatrixUtils::generateLookAtLH( Vector( -10000, -10000, -10000 ), Vector( 0, 0, 0 ), Vector_OZ, m_backLight->accessLocalMtx() );
      }

      LocationRenderSettings* renderSettings = new LocationRenderSettings();
      {
         sceneRoot->addChild( renderSettings );
         renderSettings->m_ambientLight = Color( 0.6f, 0.6f, 0.6f, 1.0f );

         ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
         renderSettings->setSkybox( resMgr.create< CubeTexture >( FilePath( "/Renderer/Materials/SkyBox_1/skyBox_1.ctex" ) ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::onInitialize()
{
   BaseSceneEditor::onInitialize();

   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.add( *this );

   // initialize the camera
   m_camera = &m_sceneWidget->getCamera();

   // fill the toolbar
   {

      // ability to lock the light so that it turns around together with the camera
      {
         m_actionLockLight = m_toolBar->addAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/dynamicPrefabLight.png" ) ), tr( "Lock light" ) );
         connect( m_actionLockLight, SIGNAL( triggered( ) ), this, SLOT( lockLight( ) ) );
      }

      // ability to turn the additional lights off
      {
         m_actionToggleAdditionalLights = m_toolBar->addAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/lightingOn.png" ) ), tr( "Toggle additional lights" ) );
         connect( m_actionToggleAdditionalLights, SIGNAL( triggered() ), this, SLOT( toggleAdditionalLights() ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::onDeinitialize( bool saveProgress )
{
   // remove self from the time controller
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.remove( *this );

   BaseSceneEditor::onDeinitialize( saveProgress );
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::onPreSaveResource()
{
   m_prefab.setEntity( m_prefabbedEntity );
 
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::update( float timeElapsed )
{
   if ( m_lightLocked )
   {
      // update light's direction      
      const Vector& entityPos = m_prefabbedEntity->getGlobalMtx().position();
      const Vector& cameraPos = m_camera->getGlobalMtx().position();

      Vector dirToObject, dirToCamera;
      dirToObject.setSub( entityPos, cameraPos ); dirToObject.normalize();
      dirToCamera.setSub( cameraPos, entityPos ); dirToCamera.normalize();

      // front light
      {
         static Vector startFront( 10000.0f, 10000.0f, 10000.0f );
         Vector end; end.setAdd( startFront, dirToCamera );
         MatrixUtils::generateLookAtLH( startFront, end, Vector_OZ, m_frontLight->accessLocalMtx() );
      }

      // back light
      {
         static Vector startBack( -10000.0f, -10000.0f, -10000.0f );
         Vector end; end.setAdd( startBack, dirToObject );
         MatrixUtils::generateLookAtLH( startBack, end, Vector_OZ, m_backLight->accessLocalMtx() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::lockLight()
{
   m_lightLocked = !m_lightLocked;

   // update action icon
   if ( m_lightLocked )
   {
      m_actionLockLight->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/dynamicPrefabLight.png" ) ) );
   }
   else
   {
      m_actionLockLight->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/fixedPrefabLight.png" ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PrefabEditor::toggleAdditionalLights()
{
   m_additionaLightsEnabled = !m_additionaLightsEnabled;

   // update action icon
   if ( m_additionaLightsEnabled )
   {
      m_actionToggleAdditionalLights->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/lightingOn.png" ) ) );
   }
   else
   {
      m_actionToggleAdditionalLights->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/lightingOff.png" ) ) );
   }

   // add/remove lights from the scene
   m_frontLightComp->setEnabled( m_additionaLightsEnabled );
   m_backLightComp->setEnabled( m_additionaLightsEnabled );
}

///////////////////////////////////////////////////////////////////////////////
