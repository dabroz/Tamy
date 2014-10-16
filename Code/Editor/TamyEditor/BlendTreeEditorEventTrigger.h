/// @file   TamyEditor/BlendTreeEditorEventTrigger.h
/// @brief  a trigger that triggers blend tree events from the editor
#pragma once

#include <QtWidgets\QPushButton>


///////////////////////////////////////////////////////////////////////////////

class BlendTreePlayer;
class BlendTreeEvent;

///////////////////////////////////////////////////////////////////////////////

class BlendTreeEditorEventTrigger : public QPushButton
{
   Q_OBJECT

private:
   BlendTreePlayer*  m_blendTreePlayer;
   BlendTreeEvent*   m_btEvent;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTreePlayer
    */
   BlendTreeEditorEventTrigger( QWidget* parent, BlendTreePlayer* blendTreePlayer );
   ~BlendTreeEditorEventTrigger();

   /**
    * Initializes the trigger button.
    *
    * @param btEvent
    */
   void initialize( BlendTreeEvent* btEvent );

public slots:
   void triggerEvent( bool );
};

///////////////////////////////////////////////////////////////////////////////
