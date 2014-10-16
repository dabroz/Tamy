/// @file   win-AppFlow\WinWindowBuilder.h
/// @brief  creates a single window the whole area of which is dedicated to tamy renderer
#pragma once

#include "core-AppFlow\WindowBuilder.h"
#include <string>
#include <windows.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////

#define MAX_LOADSTRING 255

///////////////////////////////////////////////////////////////////////////////

class CompositeWindowMessagesProcessor;
class WinApplicationManager;
class WinUserInputController;
class ApplicationManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * This window builder implementation is dedicated to creating a rendering
 * widget on a Windows system.
 */
class WinWindowBuilder
{
public:
   struct WindowParams
   {
      char                          windowTitle[MAX_LOADSTRING];
      char                          windowClassName[MAX_LOADSTRING];
      UINT                          menuID;
      UINT                          width;
      UINT                          height;

      WindowParams()
         : menuID(-1)
         , width( 800 )
         , height( 600 )
      {
         windowTitle[0] = 0;
         windowClassName[0] = 0;
      }
   };

private:
   static WinWindowBuilder             s_theInstance;
   static bool                         s_windowInitialized;

   HINSTANCE                           m_hInstance;
   int                                 m_nCmdShow;
   HICON                               m_icon;

   WinApplicationManager*              m_appManager;
   WinUserInputController*             m_inputManager;
   CompositeWindowMessagesProcessor*   m_msgProcessors;

public:
   /**
    * Constructor.
    */
   WinWindowBuilder();
   ~WinWindowBuilder();

   /**
    * Initializes the builder instance.
    *
    * @param hInstance
    * @param nCmdShow
    * @param icon
    */
   static void initialize( HINSTANCE hInstance, int nCmdShow,  HICON icon );

   /**
    * REturns the singleton instance of the builder.
    */
   static WinWindowBuilder& getInstance() { return s_theInstance; }

   /**
    * Creates a full screen application window.
    *
    * @param windowParams
    */
   HWND createFullScreenWindow( const WindowParams& windowParams ) const;

   /**
    * Creates a regular application window.
    *
    * @param windowParams
    */
   HWND createWindowedModeWindow( const WindowParams& windowParams ) const;

   /**
    * Returns the ApplicationManager instance that's running this game.
    */
   inline WinApplicationManager& getApplicationMgr() { return *m_appManager; }

protected:
   static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Windows platform definition of the main function
 */
#define MAIN_FUNCTION_START() \
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow ) \
{  \
   WinWindowBuilder::initialize( hInstance, nCmdShow, NULL );

///////////////////////////////////////////////////////////////////////////////

/**
 * Windows platform definition of the main function
 */
#define MAIN_FUNCTION_END() }

///////////////////////////////////////////////////////////////////////////////
