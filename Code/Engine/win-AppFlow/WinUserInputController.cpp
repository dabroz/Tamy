#include "win-AppFlow\WinUserInputController.h"


///////////////////////////////////////////////////////////////////////////////

WinUserInputController::WinUserInputController()
   : m_rightMouseButton( false )
   , m_leftMouseButton( false )
   , m_relativePt( 200, 200 )
{
   memset(m_keyBuffer, 0, 256 * sizeof(unsigned char));

   m_mouseSpeed.setUniform( 0 );
   SetCursor(NULL);
}

///////////////////////////////////////////////////////////////////////////////

void WinUserInputController::update( float timeElapsed )
{
   Point newMousePos;
   checkUserInput();

   setMousePos( m_relativePt );
   Point mousePosChange = newMousePos - m_relativePt;
  
   // determine mouse speed
   if ( timeElapsed > 0 )
   {
      m_mouseSpeed.v[0] = ( (float)mousePosChange.x ) / timeElapsed;
      m_mouseSpeed.v[1] = ( (float)mousePosChange.y ) / timeElapsed;
   }
   else
   {
      m_mouseSpeed.v[0] = 0;
      m_mouseSpeed.v[1] = 0;
   }

   // set the mouse and cursor position
   m_mousePos = newMousePos;
}

///////////////////////////////////////////////////////////////////////////////

void WinUserInputController::checkUserInput()
{
   if ( !GetKeyboardState( m_keyBuffer ) ) 
   {
      return;
   }

   setKey( VK_RBUTTON, m_rightMouseButton);
   setKey( VK_RBUTTON, m_leftMouseButton);

   POINT cursorPos;
   GetCursorPos(&cursorPos);
   m_mousePos.x = cursorPos.x;
   m_mousePos.y = cursorPos.y;
}

///////////////////////////////////////////////////////////////////////////////

void WinUserInputController::setMousePos(const Point& pos)
{
   SetCursorPos(pos.x, pos.y);
}

///////////////////////////////////////////////////////////////////////////////

bool WinUserInputController::isKeyPressed(unsigned char keyCode) const
{
   return (m_keyBuffer[keyCode] & 0xF0) != 0; 
}

///////////////////////////////////////////////////////////////////////////////

void WinUserInputController::setKey( unsigned char keyCode, bool pressed )
{
   if ( pressed )
   {
      m_keyBuffer[keyCode] = 0xF0; 
   }
   else
   {
      m_keyBuffer[keyCode] = 0; 
   }
}

///////////////////////////////////////////////////////////////////////////////

bool WinUserInputController::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_RBUTTONDOWN:
      {  
         SetCapture(hWnd);
         m_rightMouseButton = true;  
         break;
      }

   case WM_LBUTTONDOWN:
      {
         SetCapture(hWnd);
         m_leftMouseButton = true;
         break;
      }

   case WM_RBUTTONUP:
      {
         ReleaseCapture();
         m_rightMouseButton = false;
         break;
      }

   case WM_LBUTTONUP:
      {
         ReleaseCapture();
         m_leftMouseButton = false;
         break;
      }

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
