/// @file   TamyEditor/TamySceneWidgetInputManager.h
/// @brief  rendering widget input manager
#pragma once

#include "core\Point.h"
#include "core-AppFlow\UserInputController.h"


///////////////////////////////////////////////////////////////////////////////

typedef TVector<2> MouseSpeedVec;
class QWidget;

///////////////////////////////////////////////////////////////////////////////

/**
 * User input handling mechanism.
 */
class TamySceneWidgetInputManager : public UserInputController
{
   DECLARE_ALLOCATOR( TamySceneWidgetInputManager, AM_DEFAULT );

private:
   QWidget*          m_renderWindow;
   bool              m_enabled;

   Point             m_relativePt;
   Point             m_prevMousePos;

   unsigned char     m_keyBuffer[256];
   Point             m_mousePos;
   MouseSpeedVec		m_mouseSpeed;

   bool              m_relativeModeOn;

public:
   TamySceneWidgetInputManager( QWidget* renderWindow );

   /**
    * Enables/disables user input.
    *
    * @param enable
    */
   void setEnabled( bool enable );

   /**
    * Resets buffered input status.
    */
   void resetInput();

   /**
    * Using this utility method, an implementation should inform about a key
    * being pressed or released
    *
    * @param keyCode    code of the changed key
    * @param pressed    'true' if  pressed, 'false' - if released
    */
   void setKey( unsigned char keyCode, bool pressed );

   /**
    * This method sets the mouse at the specified position
    */
   void setMousePos( const Point& pos );


   /**
    * Updates the manager.
    */
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // UserInputController implementation
   // -------------------------------------------------------------------------
   void setRelativeMouseMovement( bool enable );
   const Point& getMousePos() const { return m_mousePos; }
   const MouseSpeedVec& getMouseSpeed() const { return m_mouseSpeed; }
   bool isKeyPressed( unsigned char keyCode ) const;

private:
   void checkMouseInput( Point& outMousePos );
};

///////////////////////////////////////////////////////////////////////////////
