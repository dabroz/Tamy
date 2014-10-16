#include "GameApplication.h"
#include "core-Renderer.h"
#include "ext-RenderingPipeline.h"
#include "core-AppFlow.h"
#include "win-AppFlow\WinWindowBuilder.h"
#include "core-MVC\TransformsManagementSystem.h"


///////////////////////////////////////////////////////////////////////////////

GameApplication::GameApplication( uint windowWidth, uint windowHeight, bool fullScreen, WindowHandle mainWindow )
   : Application( "game" )
   , m_windowWidth( windowWidth )
   , m_windowHeight( windowHeight )
   , m_fullScreen( fullScreen )
   , m_mainWindow( mainWindow )
   , m_renderer( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

GameApplication::~GameApplication()
{
   delete m_renderer;
   m_renderer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GameApplication::initialize()
{
   // create the renderer
   {
      m_renderer = initializeRenderer( m_fullScreen, m_windowWidth, m_windowHeight, m_mainWindow );
      if ( !m_renderer )
      {
         ASSERT_MSG( m_renderer, "Renderer was not created" );
         return;
      }
   }


   // get the resources manager instance we're gonna be using from now on
   // and set the filesystem root
   TSingleton< Filesystem >::initialize().changeRootDir( "./Data/" );
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   // load the config resource
   GameConfig* gameConfig = resMgr.create< GameConfig >( FilePath( "game.gcf" ), true );
   if ( !gameConfig )
   {
      ASSERT_MSG( gameConfig, "Game config file was not created" );

      return;
   }


   // load game resources
   {
      // load the rendering pipeline
      DeferredRenderingMechanism* renderingMechanism = new DeferredRenderingMechanism();
      m_renderer->setMechanism( renderingMechanism );

      // load the game scene
      {
         Model* worldScene = resMgr.create< Model >( gameConfig->m_worldModelPath, true );
         renderingMechanism->assignScene( FRS_Main, worldScene );

         // put it up for scene updates
         TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
         transformsMgr.addScene( worldScene );
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////

Renderer* GameApplication::initializeRenderer( bool fullScreen, uint windowWidth, uint windowHeight, WindowHandle mainWindow )
{
   // TODO:
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GameApplication::deinitialize()
{
   delete m_renderer;
   m_renderer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GameApplication::hibernate()
{
}

///////////////////////////////////////////////////////////////////////////////

void GameApplication::dehibernate()
{
}

///////////////////////////////////////////////////////////////////////////////

void GameApplication::notify( const std::string& senderApp, int signalCode )
{
}

///////////////////////////////////////////////////////////////////////////////
