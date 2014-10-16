#include "core-AppFlow\WindowBuilder.h"
#include "win-AppFlow\WinWindowBuilder.h"
#include <tchar.h>
#include "win-AppFlow/WinUserInputController.h"
#include "win-AppFlow/WinApplicationManager.h"


///////////////////////////////////////////////////////////////////////////////

WindowHandle WindowBuilder::createWindow( const std::string& appName, bool fullScreen, unsigned int winWidth, unsigned int winHeight )
{
   WinWindowBuilder& builder = WinWindowBuilder::getInstance();

   WinWindowBuilder::WindowParams params;
   strcpy_s( params.windowTitle, appName.c_str() );
   strcpy_s( params.windowClassName, (appName + "Class").c_str() );
   params.width = winWidth;
   params.height = winHeight;

   HWND hWnd;
   if (fullScreen)
   {
      hWnd = builder.createFullScreenWindow( params );
      
   }
   else
   {
      hWnd = builder.createWindowedModeWindow( params );
   }

   return (WindowHandle)hWnd;
}

///////////////////////////////////////////////////////////////////////////////

ApplicationManager& WindowBuilder::getApplicationManager()
{
   WinWindowBuilder& builder = WinWindowBuilder::getInstance();
   return builder.getApplicationMgr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WinWindowBuilder WinWindowBuilder::s_theInstance;
bool WinWindowBuilder::s_windowInitialized = false;

///////////////////////////////////////////////////////////////////////////////

WinWindowBuilder::WinWindowBuilder()
   : m_hInstance(NULL)
   , m_nCmdShow(0)
   , m_icon(NULL)
{
   // prepare window messages processors
   m_appManager = new WinApplicationManager();
   m_inputManager = new WinUserInputController();
   m_msgProcessors = new CompositeWindowMessagesProcessor();
   m_msgProcessors->add( *m_appManager );
   m_msgProcessors->add( *m_inputManager );

}

///////////////////////////////////////////////////////////////////////////////

WinWindowBuilder::~WinWindowBuilder()
{
   delete m_appManager;
   m_appManager = NULL;

   delete m_inputManager;
   m_inputManager = NULL;

   delete m_msgProcessors;
   m_msgProcessors = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void WinWindowBuilder::initialize( HINSTANCE hInstance, int nCmdShow, HICON icon )
{
   s_theInstance.m_hInstance = hInstance;
   s_theInstance.m_nCmdShow = nCmdShow;
   s_theInstance.m_icon = icon;
}

///////////////////////////////////////////////////////////////////////////////

HWND WinWindowBuilder::createFullScreenWindow( const WindowParams& windowParams ) const
{
   WNDCLASSEX wcex;
   wcex.cbSize          = sizeof(WNDCLASSEX);

   wcex.style			   = CS_BYTEALIGNCLIENT;
   wcex.lpfnWndProc	   = WndProc;
   wcex.cbClsExtra		= 0;
   wcex.cbWndExtra		= 0;
   wcex.hInstance		   = m_hInstance;
   wcex.hIcon			   = NULL;
   wcex.hCursor		   = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName	   = NULL;
   wcex.lpszClassName	= windowParams.windowClassName;
   wcex.hIconSm		   = NULL;
   RegisterClassEx(&wcex);

   s_windowInitialized = false;
   HWND hWnd = CreateWindowEx(WS_EX_TOPMOST, windowParams.windowClassName, windowParams.windowTitle, 0,
      0, 0, windowParams.width, windowParams.height, NULL, NULL, m_hInstance, m_msgProcessors );

   ShowWindow( hWnd, m_nCmdShow );

   MSG msg;
   while( s_windowInitialized == false )
   {
      if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
      {
         TranslateMessage(&msg);
         DispatchMessage (&msg);
      } 
   }

   return hWnd;
}

///////////////////////////////////////////////////////////////////////////////

HWND WinWindowBuilder::createWindowedModeWindow( const WindowParams& windowParams ) const
{
   WNDCLASSEX wcex;
   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style			   = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc	   = WndProc;
   wcex.cbClsExtra		= 0;
   wcex.cbWndExtra		= 0;
   wcex.hInstance		   = m_hInstance;
   wcex.hIcon			   = m_icon;
   wcex.hCursor		   = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName	   = (windowParams.menuID >= 0) ? MAKEINTRESOURCE(windowParams.menuID) : NULL;
   wcex.lpszClassName	= windowParams.windowClassName;
   wcex.hIconSm		   = m_icon;
   RegisterClassEx(&wcex);

   s_windowInitialized = false;
   HWND hWnd = CreateWindowEx(0, windowParams.windowClassName, windowParams.windowTitle, 
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      0, 0, windowParams.width, windowParams.height, 
      NULL, NULL, m_hInstance, m_msgProcessors );
   ShowWindow( hWnd, m_nCmdShow );

   MSG msg;
   while( s_windowInitialized == false )
   {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
      {
         TranslateMessage(&msg);
         DispatchMessage (&msg);
      } 
   }

   return hWnd;
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WinWindowBuilder::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // If this is a create message, trap the 'this' pointer passed in and store it within the window.
   if (message == WM_CREATE) 
   {
      SetWindowLong(hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);
      s_windowInitialized = true;
   }

   IWindowMessagesProcessor *destination = (IWindowMessagesProcessor*)GetWindowLong(hWnd, GWL_USERDATA);
   bool wasMessageProcessed = false;
   if (destination) 
   {
      wasMessageProcessed = destination->wndProc(hWnd, message, wParam, lParam);
   }

   if (wasMessageProcessed == false)
   {
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   else
   {
      return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
