#include "BlendTreeDebugEvent.h"
#include "BlendTreeItemsRenderers.h"
#include <QtWidgets\QGraphicsSceneHoverEvent>
#include <QtWidgets\QToolTip>


///////////////////////////////////////////////////////////////////////////////

BlendTreeDebugEvent::BlendTreeDebugEvent( const char* name, float blockHeight, float startTime, float trackOffset )
   : m_name( name )
   , m_globalBounds( 0, 0, 5.0f, blockHeight )
{
   setAcceptHoverEvents( true );
   setPos( QPointF( startTime, trackOffset ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugEvent::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   BlendTreeItemsRenderers::paintEvent( painter, m_globalBounds );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugEvent::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   setToolTip( m_name );
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendTreeDebugSyncPoint::BlendTreeDebugSyncPoint( const char* name, float blockHeight, float startTime, float trackOffset, bool isOnEventsTrack )
   : m_name( name )
   , m_globalBounds( 0, 0, 5.0f, blockHeight )
   , m_opaqueness( isOnEventsTrack ? 1.0f : 0.4f )
{
   setAcceptHoverEvents( true );

   setPos( QPointF( startTime, trackOffset ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugSyncPoint::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   BlendTreeItemsRenderers::paintSyncPoint( painter, m_globalBounds, m_opaqueness );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugSyncPoint::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   setToolTip( m_name );
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
