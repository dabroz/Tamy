#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeEvent.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeSyncPoint );
   PARENT( ReflectionObject );
   PROPERTY_EDIT_REFERENCE( "Event", BlendTreeEvent*, m_btEvent );
   PROPERTY_EDIT( "Time", float, m_time );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeSyncPoint::BlendTreeSyncPoint( BlendTreeEvent* btEvent, float time )
   : m_btEvent( btEvent )
   , m_time( 0.0f )
{
   registerReferences();
   m_time = clamp< float >( time, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeSyncPoint::~BlendTreeSyncPoint()
{
   // don't erase the pointer to m_btEvent, or it won't be unregistered properly by the reference tracking mechanism
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSyncPoint::setBlendTreeEvent( BlendTreeEvent* btEvent )
{
   NOTIFY_PROPERTY_CHANGE( m_btEvent );
   m_btEvent = btEvent;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSyncPoint::setTime( float time )
{
   NOTIFY_PROPERTY_CHANGE( m_time );
   m_time = clamp< float >( time, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////
