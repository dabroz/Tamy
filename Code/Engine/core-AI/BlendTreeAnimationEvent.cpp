#include "core-AI\BlendTreeAnimationEvent.h"
#include "core-AI\BlendTreeEvent.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeAnimationEvent );
   PARENT( ReflectionObject );
   PROPERTY_EDIT_REFERENCE( "Event", BlendTreeEvent*, m_btEvent );
   PROPERTY_EDIT( "Time", float, m_time );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimationEvent::BlendTreeAnimationEvent( BlendTreeEvent* btEvent, float time )
   : m_btEvent( btEvent )
   , m_time( 0.0f )
{
   registerReferences();
   m_time = clamp< float >( time, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeAnimationEvent::~BlendTreeAnimationEvent()
{
   // don't erase the pointer to m_btEvent, or it won't be unregistered properly by the reference tracking mechanism
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimationEvent::setBlendTreeEvent( BlendTreeEvent* btEvent )
{
   NOTIFY_PROPERTY_CHANGE( m_btEvent );
   m_btEvent = btEvent;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeAnimationEvent::setTime( float time )
{
   NOTIFY_PROPERTY_CHANGE( m_time );
   m_time = clamp< float >( time, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////
