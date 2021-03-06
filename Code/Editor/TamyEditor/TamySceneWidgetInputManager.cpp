#include "TamySceneWidgetInputManager.h"
#include <QtWidgets\QApplication>
#include <QCursor>
#include <QtWidgets\QWidget>
#include <QtGui\QKeyEvent>
#include <QtGui\QMouseEvent>
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TamySceneWidgetInputManager::TamySceneWidgetInputManager( QWidget* renderWindow )
   : m_renderWindow( renderWindow )
   , m_enabled( false )
   , m_relativePt( 0, 0 )
   , m_relativeModeOn( 0 )
{
   memset(m_keyBuffer, 0, 256 * sizeof(unsigned char));

   m_mouseSpeed.setUniform( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::setRelativeMouseMovement( bool enable )
{
   if ( !m_relativeModeOn && enable )
   {
      // the relative mouse movement was just enabled - memorize the cursor position
      m_relativePt = m_mousePos;
   }

   bool hasChanged = m_relativeModeOn != enable;
   m_relativeModeOn = enable;

   if ( hasChanged )
   {
      if ( m_relativeModeOn )
      {
         QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
      }
      else
      {
         QApplication::restoreOverrideCursor();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::setEnabled( bool enable )
{
   if ( m_enabled == enable )
   {
      return;
   }

   m_enabled = enable;
   if ( m_enabled )
   {
      resetInput();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::resetInput()
{
   checkMouseInput( m_mousePos );
   memset(m_keyBuffer, 0, 256 * sizeof(unsigned char));
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::update( float timeElapsed )
{
   if ( !m_enabled )
   {
      return;
   }

   Point newMousePos;
   checkMouseInput( newMousePos );

   Point mousePosChange;
   if ( m_relativeModeOn )
   {
      setMousePos( m_relativePt );
      mousePosChange = newMousePos - m_relativePt;
   }
   else
   {
      mousePosChange = newMousePos - m_mousePos;
   }


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

bool TamySceneWidgetInputManager::isKeyPressed(unsigned char keyCode) const
{
   return (m_keyBuffer[keyCode] & 0xF0) != 0; 
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::setKey( unsigned char keyCode, bool pressed )
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

void TamySceneWidgetInputManager::checkMouseInput( Point& outMousePos )
{
   QPoint cursorPos = m_renderWindow->cursor().pos();
   cursorPos = m_renderWindow->mapFromGlobal( cursorPos );
   outMousePos.x = cursorPos.x();
   outMousePos.y = cursorPos.y();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::setMousePos( const Point& pos )
{
   QPoint cursorPos( pos.x, pos.y );
   cursorPos = m_renderWindow->mapToGlobal( cursorPos );
   m_renderWindow->cursor().setPos( cursorPos );
}

///////////////////////////////////////////////////////////////////////////////

bool TamySceneWidgetInputManager::eventFilter( QObject* obj, QEvent* event )
{
   switch ( event->type() )
   {
   case QEvent::KeyPress:
   {
      QKeyEvent* keyEvent = static_cast< QKeyEvent* >( event );
      if ( !keyEvent->isAutoRepeat() )
      {
         setKey( toDXKey( keyEvent->key() ), true );
      }

      return true;
   }

   case QEvent::KeyRelease:
   {
      QKeyEvent* keyEvent = static_cast< QKeyEvent* >( event );
      if ( !keyEvent->isAutoRepeat() )
      {
         setKey( toDXKey( keyEvent->key() ), false );
      }

      return true;
   }

   case QEvent::MouseButtonPress:
   {
      QMouseEvent* mouseEvent = static_cast< QMouseEvent* >( event );
      onMouseButton( mouseEvent->button(), true );
      
      return true;
   }

   case QEvent::MouseButtonRelease:
   {
      QMouseEvent* mouseEvent = static_cast< QMouseEvent* >( event );
      onMouseButton( mouseEvent->button(), false );
      return true;
   }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidgetInputManager::onMouseButton( Qt::MouseButton button, bool pressed )
{
   switch ( button )
   {
   case Qt::LeftButton:
   {
      setKey( VK_LBUTTON, pressed );
      break;
   }

   case Qt::RightButton:
   {
      setKey( VK_RBUTTON, pressed );
      break;
   }

   case Qt::MidButton:
   {
      setKey( VK_MBUTTON, pressed );
      break;
   }

   case Qt::NoButton:   // fall through
   default:
   {
      break;
   }
   }
}

///////////////////////////////////////////////////////////////////////////////

unsigned char TamySceneWidgetInputManager::toDXKey( int qtKeyCode )
{
   switch ( qtKeyCode )
   {
   case Qt::Key_Escape:    return VK_ESCAPE;
   case Qt::Key_Tab:       return VK_TAB;
   case Qt::Key_Backspace: return VK_CLEAR;
   case Qt::Key_Return:    return VK_RETURN;
   case Qt::Key_Enter:     return VK_TAB;
   case Qt::Key_Insert:    return VK_INSERT;
   case Qt::Key_Delete:    return VK_DELETE;
   case Qt::Key_Pause:     return VK_PAUSE;
   case Qt::Key_Print:     return VK_PRINT;
   case Qt::Key_Home:      return VK_HOME;
   case Qt::Key_End:       return VK_END;
   case Qt::Key_Left:      return VK_LEFT;
   case Qt::Key_Up:        return VK_UP;
   case Qt::Key_Right:     return VK_RIGHT;
   case Qt::Key_Down:      return VK_DOWN;
   case Qt::Key_PageUp:    return VK_PRIOR;
   case Qt::Key_PageDown:  return VK_NEXT;
   case Qt::Key_Shift:     return VK_SHIFT;
   case Qt::Key_Control:   return VK_CONTROL;
   case Qt::Key_Clear:     return VK_CLEAR;
   case Qt::Key_F1:        return VK_F1;
   case Qt::Key_F2:        return VK_F2;
   case Qt::Key_F3:        return VK_F3;
   case Qt::Key_F4:        return VK_F4;
   case Qt::Key_F5:        return VK_F5;
   case Qt::Key_F6:        return VK_F6;
   case Qt::Key_F7:        return VK_F7;
   case Qt::Key_F8:        return VK_F8;
   case Qt::Key_F9:        return VK_F9;
   case Qt::Key_F10:       return VK_F10;
   case Qt::Key_F11:       return VK_F11;
   case Qt::Key_F12:       return VK_F12;
   case Qt::Key_F13:       return VK_F13;
   case Qt::Key_F14:       return VK_F14;
   case Qt::Key_F15:       return VK_F15;
   case Qt::Key_F16:       return VK_F16;
   case Qt::Key_F17:       return VK_F17;
   case Qt::Key_F18:       return VK_F18;
   case Qt::Key_F19:       return VK_F19;
   case Qt::Key_F20:       return VK_F20;
   case Qt::Key_F21:       return VK_F21;
   case Qt::Key_F22:       return VK_F22;
   case Qt::Key_F23:       return VK_F23;
   case Qt::Key_F24:       return VK_F24;
   case Qt::Key_Space:     return VK_SPACE;
   case Qt::Key_Exclam:    return '!';
   case Qt::Key_QuoteDbl:  return '"';
   case Qt::Key_NumberSign:return '#';
   case Qt::Key_Dollar:    return '$';
   case Qt::Key_Percent:   return '%';
   case Qt::Key_Ampersand: return '&';
   case Qt::Key_Apostrophe:return '\'';
   case Qt::Key_ParenLeft: return '(';
   case Qt::Key_ParenRight:return ')';
   case Qt::Key_Asterisk:  return '*';
   case Qt::Key_Plus:      return '+';
   case Qt::Key_Comma:     return ',';
   case Qt::Key_Minus:     return '-';
   case Qt::Key_Period:    return '.';
   case Qt::Key_Slash:     return '/';
   case Qt::Key_0:         return '0';
   case Qt::Key_1:         return '1';
   case Qt::Key_2:         return '2';
   case Qt::Key_3:         return '3';
   case Qt::Key_4:         return '4';
   case Qt::Key_5:         return '5';
   case Qt::Key_6:         return '6';
   case Qt::Key_7:         return '7';
   case Qt::Key_8:         return '8';
   case Qt::Key_9:         return '9';
   case Qt::Key_Colon:     return ':';
   case Qt::Key_Semicolon: return ';';
   case Qt::Key_Less:      return '<';
   case Qt::Key_Equal:     return '=';
   case Qt::Key_Greater:   return '>';
   case Qt::Key_Question:  return '?';
   case Qt::Key_At:        return '@';
   case Qt::Key_A:         return 'A';
   case Qt::Key_B:         return 'B';
   case Qt::Key_C:         return 'C';
   case Qt::Key_D:         return 'D';
   case Qt::Key_E:         return 'E';
   case Qt::Key_F:         return 'F';
   case Qt::Key_G:         return 'G';
   case Qt::Key_H:         return 'H';
   case Qt::Key_I:         return 'I';
   case Qt::Key_J:         return 'J';
   case Qt::Key_K:         return 'K';
   case Qt::Key_L:         return 'L';
   case Qt::Key_M:         return 'M';
   case Qt::Key_N:         return 'N';
   case Qt::Key_O:         return 'O';
   case Qt::Key_P:         return 'P';
   case Qt::Key_Q:         return 'Q';
   case Qt::Key_R:         return 'R';
   case Qt::Key_S:         return 'S';
   case Qt::Key_T:         return 'T';
   case Qt::Key_U:         return 'U';
   case Qt::Key_V:         return 'V';
   case Qt::Key_W:         return 'W';
   case Qt::Key_X:         return 'X';
   case Qt::Key_Y:         return 'Y';
   case Qt::Key_Z:         return 'Z';
   case Qt::Key_BracketLeft:  return '[';
   case Qt::Key_Backslash:    return '\\';
   case Qt::Key_BracketRight: return ']';
   case Qt::Key_Underscore:   return '_';
   case Qt::Key_BraceLeft:    return '{';
   case Qt::Key_Bar:          return '|';
   case Qt::Key_BraceRight:   return '}';
   case Qt::Key_AsciiTilde:   return '~';
   default:                return 0;
   };
}

///////////////////////////////////////////////////////////////////////////////
