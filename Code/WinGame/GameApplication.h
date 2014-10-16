/// @file   WinGame/GameApplication.h
/// @brief  game application
#pragma once

#include "core-AppFlow/Application.h"
#include "core-AppFlow/WindowBuilder.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Game application.
 */
class GameApplication : public Application
{
private:
   Renderer*            m_renderer;
   uint                 m_windowWidth;
   uint                 m_windowHeight;
   bool                 m_fullScreen;
   WindowHandle         m_mainWindow;

public:
   /**
    * Constructor.
    *
    * @param windowWidth
    * @param windowHeight
    * @param fullScreen
    * @param mainWindow
    */
   GameApplication( uint windowWidth, uint windowHeight, bool fullScreen, WindowHandle mainWindow );
   ~GameApplication();

   // -------------------------------------------------------------------------
   // Application implementation
   // -------------------------------------------------------------------------
   void initialize();
   void deinitialize();
   void hibernate();
   void dehibernate();
   void notify( const std::string& senderApp, int signalCode );

private:
   Renderer* initializeRenderer( bool fullScreen, uint windowWidth, uint windowHeight, WindowHandle mainWindow );
};

///////////////////////////////////////////////////////////////////////////////
