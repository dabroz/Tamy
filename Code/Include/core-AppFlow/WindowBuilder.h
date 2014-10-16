/// @file   core-AppFlow\WindowBuilder.h
/// @brief  Application window builder
#pragma once

#include <string>


///////////////////////////////////////////////////////////////////////////////

typedef unsigned long      WindowHandle;
class ApplicationManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * Application window builder.
 */
class WindowBuilder
{
public:
   /**
    * The method creates an application window.
    *
    * @param   appName
    * @param   fullScreen
    * @param   winWidth
    * @param   winHeight
    * @return  window handle
    */
   static WindowHandle createWindow( const std::string& appName, bool fullScreen, unsigned int winWidth, unsigned int winHeight );

   /**
    * Returns the singleton instance of ApplicationManager running this game.
    */
   static ApplicationManager& getApplicationManager();
};

///////////////////////////////////////////////////////////////////////////////
