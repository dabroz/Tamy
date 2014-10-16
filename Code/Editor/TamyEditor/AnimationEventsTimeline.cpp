#include "AnimationEventsTimeline.h"
#include "BTAEventItem.h"
#include <QtWidgets\QSlider>
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>


///////////////////////////////////////////////////////////////////////////////

QPen AnimationEventsTimeline::s_markerPen( QBrush( QColor( 26, 184, 246 ) ), 3.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
QPen AnimationEventsTimeline::s_primaryTimeLinesPen( QColor( 64, 64, 64, 64 ), 2 );
QPen AnimationEventsTimeline::s_secondaryTimeLinesPen( QColor( 64, 64, 64, 64 ), 1 );
QPen AnimationEventsTimeline::s_timeValuesPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine );
QFont AnimationEventsTimeline::s_font( "Arial" );

///////////////////////////////////////////////////////////////////////////////

AnimationEventsTimeline::AnimationEventsTimeline( QWidget* parentWidget )
   : QGraphicsView( parentWidget )
   , m_slider( NULL )
   , m_timeBarPos( 0.0f )
{
   this->setMinimumHeight( 40 );
   this->setMaximumHeight( 40 );

   setViewportUpdateMode( QGraphicsView::FullViewportUpdate );

   // allow the widget to display custom context menus
   setContextMenuPolicy( Qt::CustomContextMenu );
}

///////////////////////////////////////////////////////////////////////////////

AnimationEventsTimeline::~AnimationEventsTimeline()
{
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsTimeline::setSlider( QSlider* slider )
{
   if ( m_slider )
   {
      disconnect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( timeValueChanged( int ) ) );
   }

   m_timeBarPos = 0.0f;
   m_slider = slider;

   if ( m_slider )
   {
      m_timeBarPos = (float)m_slider->sliderPosition() / (float)m_slider->maximum();
      connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( timeValueChanged( int ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsTimeline::adjustRect()
{
   // we don't want to change the rendered scene rectangle, so be sure to reset it
   // to the view's rectangle
   QRectF viewRect = rect();
   float w = viewRect.width();
   float h = viewRect.height();
   viewRect.translate( 1, 1 - ( h * 0.5f ) );
   viewRect.setWidth( w - 2 );
   viewRect.setHeight( h - 2 );
   setSceneRect( viewRect );
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsTimeline::resizeEvent( QResizeEvent* event )
{
   QGraphicsView::resizeEvent( event );

   adjustRect();

   QList< QGraphicsItem* > items = scene()->items();
   foreach( QGraphicsItem* item, items )
   {
      BTAEventItem* eventItem = static_cast< BTAEventItem* >( item );
      eventItem->calculateBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsTimeline::drawBackground( QPainter* painter, const QRectF& rect )
{
   QGraphicsView::drawBackground( painter, rect );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );

   const QRectF bounds = this->rect();
   const float width = bounds.width();

   // draw timescale
   {     
      painter->setOpacity( 1.0f );
      painter->setFont( s_font );

      const float textVertPos = rect.top() + 10;

      // always draw main lines
      {
         const float halfWidth = width * 0.5f;

         QPainterPath linesPath;
         linesPath.moveTo( 0.0f, rect.top() );
         linesPath.lineTo( 0.0f, rect.bottom() );
         linesPath.moveTo( halfWidth, rect.top() );
         linesPath.lineTo( halfWidth, rect.bottom() );
         linesPath.moveTo( width, rect.top() );
         linesPath.lineTo( width, rect.bottom() );

         painter->setPen( s_primaryTimeLinesPen );
         painter->drawPath( linesPath );

         // draw text on top of it
         painter->setPen( s_timeValuesPen );
         painter->drawText( QPointF( 6, textVertPos ), "0%" );
         painter->drawText( QPointF( halfWidth + 6, textVertPos ), "50%" );
         painter->drawText( QPointF( width - 30, textVertPos ), "100%" );
      }
      
      // if the width allows, draw secondary lines
      if ( width > 600.0f )
      {
         QPainterPath linesPath;

         const float lineStep = width * 0.1f;
         for ( float pos = 0.0f; pos <= width; pos += lineStep )
         {
            linesPath.moveTo( pos, rect.top() );
            linesPath.lineTo( pos, rect.bottom() );
         }
         
         painter->setPen( s_secondaryTimeLinesPen );
         painter->drawPath( linesPath );
      }
   }

   // draw the time bar
   {
      float pos = width * m_timeBarPos;

      QPainterPath path;
      QPoint top( pos, bounds.top() );
      QPoint bottom( pos, bounds.bottom() );
      QPointF topS = this->mapToScene( top );
      QPointF bottomS = this->mapToScene( bottom );
      path.moveTo( topS );
      path.lineTo( bottomS );

      painter->setPen( s_markerPen );
      painter->setOpacity( 0.5f );
      painter->drawPath( path );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsTimeline::timeValueChanged( int timeInMillisecs )
{
   m_timeBarPos = (float)timeInMillisecs / (float)m_slider->maximum();
   update();
}

///////////////////////////////////////////////////////////////////////////////

float AnimationEventsTimeline::timeToViewportPos( float eventTime ) const
{
   QRectF bounds = rect();
   float width = bounds.width();
   float eventTimeOnTheTimeline = eventTime * width * 0.5f; 

   // NOTE: that multiplication by half there - it's the weirdest thing!
   // Without it, it's as if we were only looking at the left half of the scene ( if you move
   // an item, it would move twice as far than where your mouse would be with respect to the left
   // side of the view ).
   //
   // The thing is - I want to show a fixed view of the scene - as large, as the viewport itself 
   // ( I always keep the View::sceneRect synchronized with the View::geometry ) - that's
   // to avoid showing the scrollbars etc.
   // So with every action I make ( a resize or an addition/removal of an item ) I adjust those
   // rectangles. They are always in sync.
   // Moreover - I need to actually change the position of an item when I display/move it around.
   // So I set its position Item::setPos and adjust the global bounds that I return it its  Item::boundingRect method.
   //
   // And even though that mechanism guarantees that both the bounds and the position are in sync 
   // ( the global bounding box always spans around the position in the scene at which the item is located ),
   // and I can actually confirm that by clicking at the item and seeing that it can always be selected,
   // as soon as I move it - it ends up twice as far to the right and then it tracks mouse's position like that ( event.scenePos.x * 2 basically )
   //
   // And here's where that multiplication by 0.5 originated.
   //
   //
   // ---------
   // Another issue - all my events live in a (0..1) timeframe. So I tried reflecting that in the scene,
   // by adding them at that range of positions, and having the view show only that portion of the scene.
   // But it didn't work - the extents weren't scaled up to fill the extents of the view automatically.
   //
   // So instead, I always keep the positions in the same scale as the viewport ( if it's 800px wide,
   // the positions are gonna range from 0 to 800 ).

   return eventTimeOnTheTimeline;
}

///////////////////////////////////////////////////////////////////////////////

float AnimationEventsTimeline::viewportPosToTime( float viewportPos ) const
{
   QRectF bounds = rect();
   float width = bounds.width();
   float eventTime = viewportPos / width;

   return eventTime;
}

///////////////////////////////////////////////////////////////////////////////
