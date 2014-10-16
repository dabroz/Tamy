#include "TamySceneWidgetInputManager.h"
#include <QtWidgets\QApplication>
#include <QCursor>
#include <QtWidgets\QWidget>


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
