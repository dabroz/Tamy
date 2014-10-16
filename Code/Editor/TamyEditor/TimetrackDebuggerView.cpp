#include "TimetrackDebuggerView.h"
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>


///////////////////////////////////////////////////////////////////////////////

QBrush TimetrackDebuggerView::s_oddTrackBrush( QColor( 64, 64, 64, 32 ) );
QPen TimetrackDebuggerView::s_oddTrackPen( QColor( 64, 64, 64, 32 ) );
QPen TimetrackDebuggerView::s_timeLinesPen( QColor( 64, 64, 64, 64 ) );
QPen TimetrackDebuggerView::s_timeValuesPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine );
QFont TimetrackDebuggerView::s_font( "Arial" );

///////////////////////////////////////////////////////////////////////////////

TimetrackDebuggerView::TimetrackDebuggerView( QWidget* parent, float trackHeight )
   : QGraphicsView( parent )
   , m_trackHeight( trackHeight )
   , m_drawTimeTicks( false )
{
   setTransformationAnchor( QGraphicsView::NoAnchor );
   setResizeAnchor( QGraphicsView::NoAnchor );
   setMouseTracking( true );

   setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
   setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
}

///////////////////////////////////////////////////////////////////////////////

TimetrackDebuggerView::~TimetrackDebuggerView()
{
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebuggerView::drawBackground( QPainter* painter, const QRectF& rect )
{
   painter->save();

   // draw track strips
   {
      int firstTrackIdx = (int)( rect.top() / m_trackHeight );
      int tracksCount = (int)( rect.height() / m_trackHeight );
      int lastTrackIdx = firstTrackIdx + tracksCount + 1;

      QRectF drawnRect = rect;

      painter->setBrush( s_oddTrackBrush );
      painter->setPen( s_oddTrackPen );

      for ( int trackIdx = firstTrackIdx; trackIdx < lastTrackIdx; ++trackIdx )
      {
         if ( trackIdx % 2 )
         {
            float trackTopOffset = (float)trackIdx * m_trackHeight;
            drawnRect.setTop( trackTopOffset );
            drawnRect.setBottom( trackTopOffset + m_trackHeight );

            QPainterPath path;
            path.addRect( drawnRect );

            painter->drawPath( path );
         }
      }
   }

   // draw timescale
   {     
      int startTimeVal = (int)rect.left() / 100; // [seconds]
      int endTimeVal = (int)rect.right() / 100;

      // draw the time at even intervals, every 1s
      QPainterPath path;
      painter->setPen( s_timeLinesPen );
      for ( int i = startTimeVal; i <= endTimeVal; ++i )
      {
         float timeVal = (float)i * 100.0f;
         path.moveTo( timeVal, rect.top() );
         path.lineTo( timeVal, rect.bottom() );
      }
      painter->drawPath( path );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebuggerView::drawForeground( QPainter* painter, const QRectF& rect )
{
   if ( m_drawTimeTicks )
   {
      painter->save();

      painter->setRenderHint( QPainter::TextAntialiasing, true );
      painter->setPen( s_timeValuesPen );
      painter->setFont( s_font );

      int startTimeVal = (int)rect.left() / 100; // [seconds]
      int endTimeVal = (int)rect.right() / 100;

      QString timeValStr;

      // draw the time at even intervals, every 1s

      for ( int i = startTimeVal; i <= endTimeVal; ++i )
      {
         float timeVal = (float)i * 100.0f;
         timeValStr.sprintf( "%d", i );

         painter->drawText( QPointF( timeVal + 5, 10 ), timeValStr );
      }


      painter->restore();
   }
}

///////////////////////////////////////////////////////////////////////////////
