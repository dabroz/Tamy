#include "core.h"
#include "core-AppFlow.h"
#include "win-AppFlow.h"
#include "GameApplication.h"


///////////////////////////////////////////////////////////////////////////////

// type includes
#include "core/TypesRegistry.cpp"
#include "core-MVC/TypesRegistry.cpp"
#include "core-Renderer/TypesRegistry.cpp"
#include "core-AI/TypesRegistry.cpp"
#include "core-AppFlow/TypesRegistry.cpp"
#include "core-Physics/TypesRegistry.cpp"
#include "ext-StoryTeller/TypesRegistry.cpp"
#include "ext-RenderingPipeline/TypesRegistry.cpp"
#include "ext-2DGameLevel/TypesRegistry.cpp"
#include "ext-ProceduralAnimation/TypesRegistry.cpp"

///////////////////////////////////////////////////////////////////////////////

MAIN_FUNCTION_START()
{
   const uint windowWidth = 800;
   const uint windowHeight = 600;
   const bool fullScreen = false;
   const char* appName = "WinGame";

   // create the main application window
   WindowHandle mainWindow = WindowBuilder::createWindow( appName, fullScreen, windowWidth, windowHeight );

   // initialize the thread system
   TSingleton< ThreadSystem >::initialize();

   // build the types registry
   TSingleton< ReflectionTypesRegistry >::getInstance().build( TSingleton< PatchesDB >::getInstance() );

   // create the application and plug it into the manager
   GameApplication* application = new GameApplication( windowWidth, windowHeight, fullScreen, mainWindow );
   ApplicationManager& appMgr = WindowBuilder::getApplicationManager();
   appMgr.addApplication( *application );
   appMgr.setEntryApplication( application->getName() );

   // run the main loop
   EventsDispatcher& eventsDispatcher = TSingleton< EventsDispatcher >::getInstance();
   RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   AISystem& animSys = TSingleton< AISystem >::getInstance();
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
   CTimer mainTimer;
   while( appMgr.step() )
   {
      mainTimer.tick();
      float timeElapsed = mainTimer.getTimeElapsed();

      // update the updatables
      timeController.update( timeElapsed );

      // dispatch events
      eventsDispatcher.dispatchEvents();

      // update game systems
      animSys.tick( timeElapsed );

      // update transforms
      transformsMgr.tick();

      // render
      renderSystem.render();
   }

   // cleanup
   delete application;

   // deinitialize
   SingletonsManager::deinitialize();

   return 0;
}
MAIN_FUNCTION_END()
   
///////////////////////////////////////////////////////////////////////////////
