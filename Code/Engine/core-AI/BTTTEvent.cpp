#include "core-AI\BTTTEvent.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeEvent.h"
#include "core\ReflectionProperty.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTTTEvent );
   PARENT( BlendTreeTransitionTrigger );
   PROPERTY_EDIT_REFERENCE( "Event", BlendTreeEvent*, m_btEvent );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTTTEvent::BTTTEvent()
   : m_btEvent( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTTTEvent::~BTTTEvent()
{
   // don't erase the pointer to m_btEvent, or it won't be unregistered properly by the reference tracking mechanism
}

///////////////////////////////////////////////////////////////////////////////

void BTTTEvent::setEvent( BlendTreeEvent* btEvent )
{
   NOTIFY_PROPERTY_CHANGE( m_btEvent );

   m_btEvent = btEvent;
}

///////////////////////////////////////////////////////////////////////////////

bool BTTTEvent::testActivation( BlendTreePlayer* player ) const
{
   if ( m_btEvent )
   {
      bool canActivate = player->wasEventTriggered( m_btEvent );
      return canActivate;
   }
   else
   {
      // no event set - no activation
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
