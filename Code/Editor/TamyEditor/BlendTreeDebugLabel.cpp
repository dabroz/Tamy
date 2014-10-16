#include "BlendTreeDebugLabel.h"
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>


///////////////////////////////////////////////////////////////////////////////

QPen BlendTreeDebugLabel::s_textPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine );
QFont BlendTreeDebugLabel::s_font( "Arial" );

///////////////////////////////////////////////////////////////////////////////

BlendTreeDebugLabel::BlendTreeDebugLabel( const char* text, float blockHeight, float trackOffset )
   : m_text( text )
   , m_globalBounds( 0, 0, 8192.0f, blockHeight )
   , m_fillBrush( QPointF(0, 0), QPointF( 0, 36 ) )
{
   setPos( QPointF( 0, trackOffset ) );

   m_fillBrush.setColorAt( 0, QColor( 123, 123, 123 ) );
   m_fillBrush.setColorAt( 1.0f, QColor( 150, 150, 150 ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugLabel::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );

   // block
   {
      QPainterPath blockRectPath;
      blockRectPath.addRect( m_globalBounds );

      painter->setBrush( m_fillBrush );
      painter->drawPath( blockRectPath );
   }

   // text
   {
      painter->setPen( s_textPen );
      painter->setFont( s_font );
      painter->drawText( QPointF( 3.0f, m_globalBounds.height() - 6.0f ), m_text );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////
