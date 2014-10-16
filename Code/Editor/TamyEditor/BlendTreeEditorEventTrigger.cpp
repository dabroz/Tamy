#include "BlendTreeEditorEventTrigger.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeEvent.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeEditorEventTrigger::BlendTreeEditorEventTrigger( QWidget* parent, BlendTreePlayer* blendTreePlayer )
   : QPushButton( parent )
   , m_blendTreePlayer( blendTreePlayer)
   , m_btEvent( NULL )
{
   connect( this, SIGNAL( clicked( bool ) ), this, SLOT( triggerEvent( bool ) ) );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeEditorEventTrigger::~BlendTreeEditorEventTrigger()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorEventTrigger::initialize( BlendTreeEvent* btEvent )
{
   m_btEvent = btEvent;
   setText( m_btEvent->getName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorEventTrigger::triggerEvent( bool )
{
   if ( m_btEvent )
   {
      m_blendTreePlayer->triggerEvent( m_btEvent );
   }
}

///////////////////////////////////////////////////////////////////////////////
