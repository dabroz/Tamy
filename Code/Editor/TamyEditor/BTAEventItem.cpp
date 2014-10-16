#include "BTAEventItem.h"
#include "core-AI\BlendTreeAnimationEvent.h"
#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeAnimation.h"
#include "AnimationEventsTimeline.h"
#include "QPropertiesView.h"
#include "BlendTreeItemsRenderers.h"
#include <QtWidgets\QGraphicsSceneHoverEvent>


///////////////////////////////////////////////////////////////////////////////

BTAEAnimationEventItem::BTAEAnimationEventItem( BlendTreeAnimationEvent* btaEvent, AnimationEventsTimeline* view )
   : m_btaEvent( btaEvent )
   , m_view( view )
   , m_localBounds( -5.0f, -18.0f, 10.0f, 36.0f )
{
   setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
   setAcceptHoverEvents( true );
   calculateBounds();

   m_btaEvent->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BTAEAnimationEventItem::~BTAEAnimationEventItem()
{
   m_btaEvent->detachListener( *this );
   m_btaEvent = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::onObservedPropertyChanged( ReflectionProperty& property )
{
   float eventTime = m_btaEvent->getTime();
   setPos( eventTime, 0 );

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::calculateBounds()
{
   float eventTime = m_btaEvent->getTime();
   float newPos = m_view->timeToViewportPos( eventTime ); 
   setPos( newPos, 0 );

   m_globalBounds = m_localBounds;
   m_globalBounds.translate( scenePos() );
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   BlendTreeItemsRenderers::paintEvent( painter, m_globalBounds );
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
   QPointF pos = event->scenePos();
   float eventTime = m_view->viewportPosToTime( pos.x() );

   m_btaEvent->setTime( eventTime );

   update();
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   BlendTreeEvent* btEvent = m_btaEvent->getBlendTreeEvent();
   if ( btEvent )
   {
      setToolTip( btEvent->getName().c_str() );
   }

   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

QAction* BTAEAnimationEventItem::createRemovalAction( BlendTreeAnimation& animationNode )
{
   return new RemoveAnimationEventAction( animationNode, m_btaEvent );
}

///////////////////////////////////////////////////////////////////////////////

void BTAEAnimationEventItem::viewProperties( QPropertiesView* propertiesView )
{
   propertiesView->set( *m_btaEvent );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BTASyncPointItem::BTASyncPointItem( BlendTreeSyncPoint* btaSyncPoint, AnimationEventsTimeline* view )
   : m_btaSyncPoint( btaSyncPoint )
   , m_view( view )
   , m_localBounds( -5.0f, -18.0f, 10.0f, 36.0f )
{
   setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
   setAcceptHoverEvents( true );
   calculateBounds();

   m_btaSyncPoint->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BTASyncPointItem::~BTASyncPointItem()
{
   m_btaSyncPoint->detachListener( *this );
   m_btaSyncPoint = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::onObservedPropertyChanged( ReflectionProperty& property )
{
   float eventTime = m_btaSyncPoint->getTime();
   setPos( eventTime, 0 );

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::calculateBounds()
{
   float eventTime = m_btaSyncPoint->getTime();
   float newPos = m_view->timeToViewportPos( eventTime ); 
   setPos( newPos, 0 );

   m_globalBounds = m_localBounds;
   m_globalBounds.translate( scenePos() );
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   BlendTreeItemsRenderers::paintSyncPoint( painter, m_globalBounds );
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
   QPointF pos = event->scenePos();
   float eventTime = m_view->viewportPosToTime( pos.x() );

   m_btaSyncPoint->setTime( eventTime );

   update();
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   BlendTreeEvent* btEvent = m_btaSyncPoint->getBlendTreeEvent();
   if ( btEvent )
   {
      setToolTip( btEvent->getName().c_str() );
   }

   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

QAction* BTASyncPointItem::createRemovalAction( BlendTreeAnimation& animationNode )
{
   return new RemoveAnimationSyncPoint( animationNode, m_btaSyncPoint );
}

///////////////////////////////////////////////////////////////////////////////

void BTASyncPointItem::viewProperties( QPropertiesView* propertiesView )
{
   propertiesView->set( *m_btaSyncPoint );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RemoveAnimationEventAction::RemoveAnimationEventAction( BlendTreeAnimation& animationNode, BlendTreeAnimationEvent* btaAnimEvent )
   : QAction( QIcon(), QString( "Remove " ) + QString( btaAnimEvent->getBlendTreeEvent() ? btaAnimEvent->getBlendTreeEvent()->getName().c_str() : "empty event" ), NULL )
   , m_animationNode( animationNode )
   , m_btaAnimEvent( btaAnimEvent )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void RemoveAnimationEventAction::onTriggered()
{
   m_animationNode.removeEvent( m_btaAnimEvent );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RemoveAnimationSyncPoint::RemoveAnimationSyncPoint( BlendTreeAnimation& animationNode, BlendTreeSyncPoint* btaSyncPoint )
   : QAction( QIcon(), QString( "Remove " ) + QString( btaSyncPoint->getBlendTreeEvent() ? btaSyncPoint->getBlendTreeEvent()->getName().c_str() : "empty sync point" ), NULL )
   , m_animationNode( animationNode )
   , m_btaSyncPoint( btaSyncPoint )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void RemoveAnimationSyncPoint::onTriggered()
{
   m_animationNode.removeSyncPoint( m_btaSyncPoint );
}

///////////////////////////////////////////////////////////////////////////////
