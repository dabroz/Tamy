#pragma once

#include "core-AppFlow\UserInputController.h"

///////////////////////////////////////////////////////////////////////////////

class UserInputControllerMock : public UserInputController
{
private:
   int m_keysDown;
   Point m_mousePos;
   unsigned char m_keyBuffer[256];

   int m_width;
   int m_height;

public:
   UserInputControllerMock()
      : m_width(-1),
      m_height(-1)
   {
      memset(m_keyBuffer, 0, 256 * sizeof(unsigned char));
   }


   void keyDown(unsigned char keys)
   {
      m_keyBuffer[keys] = 0xF0;
   }
   
   void mouseMove(const Point& abs)
   {
      m_mousePos = abs;
      if ((m_width >= 0) && (m_mousePos.x > m_width)) {m_mousePos.x = m_width;}
      if ((m_height >= 0) && (m_mousePos.y > m_height)) {m_mousePos.y = m_height;}
   }

   void setBounds(int width, int height)
   {
      m_width = width;
      m_height = height;
   }

   const Point& getCursorPosition() const 
   {
      return m_mousePos;
   }

protected:
   void onRelativeMouseMovement() {}
   void onAbsoluteMouseMovement() {}

   void checkUserInput(unsigned char* keyBuffer, Point& mousePos)
   {
      memcpy(keyBuffer, m_keyBuffer, 256 * sizeof(char));
      memset(m_keyBuffer, 0, 256 * sizeof(unsigned char));
      mousePos = m_mousePos;
   }

   void setMousePos(const Point& pos)
   {
      m_mousePos = pos;
   }
};

///////////////////////////////////////////////////////////////////////////////
