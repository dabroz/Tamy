/// @file   win-AppFlow/WinApplicationManager.h
/// @brief  Windows implementation of the ApplicationManager
#pragma once

#include "core-AppFlow\ApplicationManager.h"
#include "win-AppFlow\IWindowMessagesProcessor.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class CTimer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Windows implementation of the ApplicationManager.
 */
class WinApplicationManager : public ApplicationManager,
                              public IWindowMessagesProcessor
{
private:
   CTimer*              m_timer;
   
   unsigned long        m_lastFrameRate;

public:
   WinApplicationManager();
   ~WinApplicationManager();

   bool wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
   ProcessingCode onStep();
   
   float getTimeElapsed();
};

///////////////////////////////////////////////////////////////////////////////
