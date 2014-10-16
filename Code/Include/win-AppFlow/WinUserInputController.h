/// @file   win-AppFlow/WinUserInputController.h
/// @brief  Windows user input system
#pragma once

#include "core-AppFlow\UserInputController.h"
#include "win-AppFlow\IWindowMessagesProcessor.h"
#include "core\TimeDependent.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

/**
 * Windows user input system.
 */
class WinUserInputController : public UserInputController, public IWindowMessagesProcessor, public TimeDependent
{
private:   
   bool              m_rightMouseButton;
   bool              m_leftMouseButton;

   Point             m_relativePt;

   unsigned char     m_keyBuffer[256];
   Point             m_mousePos;
   MouseSpeedVec		m_mouseSpeed;

public:
   WinUserInputController();

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // UserInputController implementation
   // -------------------------------------------------------------------------
   const Point& getMousePos() const { return m_mousePos; }
   const MouseSpeedVec& getMouseSpeed() const { return m_mouseSpeed; }
   bool isKeyPressed( unsigned char keyCode ) const;

protected:
   void checkUserInput();

   void setMousePos(const Point& pos);
   
   void setKey( unsigned char keyCode, bool pressed );

   bool wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////////////////////////////////
