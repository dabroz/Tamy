#include "win-AppFlow\WinApplicationManager.h"
#include "core\Timer.h"
#include <cassert>


///////////////////////////////////////////////////////////////////////////////

WinApplicationManager::WinApplicationManager()
   : m_timer(new CTimer())
   , m_lastFrameRate(0)
{
}

///////////////////////////////////////////////////////////////////////////////

WinApplicationManager::~WinApplicationManager()
{
   delete m_timer;
   m_timer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

float WinApplicationManager::getTimeElapsed()
{
   return m_timer->getTimeElapsed();
}

///////////////////////////////////////////////////////////////////////////////

ApplicationManager::ProcessingCode WinApplicationManager::onStep()
{
    // process system messages
   MSG msg;
   if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
   {
      if (msg.message == WM_QUIT) {return APC_EXIT;}

      TranslateMessage(&msg);
      DispatchMessage (&msg);
      return APC_SYSTEM;
   } 
   else
   {
      m_timer->tick();

      return APC_APPLICATION;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool WinApplicationManager::wndProc(HWND hWnd, 
                                    UINT message, 
                                    WPARAM wParam, 
                                    LPARAM lParam)
{
   switch (message)
   {
   case WM_DESTROY:
      {
         PostQuitMessage(0);
         break;
      }

   default:
      {
         return false;
      }
   }
   return true;
}

///////////////////////////////////////////////////////////////////////////////
