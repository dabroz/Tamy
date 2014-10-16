#include "GraphWidgetUtils.h"


///////////////////////////////////////////////////////////////////////////////

QPen GraphWidgetUtils::s_textPen( QBrush( QColor( 30, 30, 30 ) ), 1.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
QPen GraphWidgetUtils::s_textShadowPen( QBrush( QColor( 130, 130, 130 ) ), 1.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
QPen GraphWidgetUtils::s_borderPen( QBrush( QColor( 200, 200, 200 ) ), 0.5f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
QPen GraphWidgetUtils::s_highlightedBorderPen( QBrush( QColor( 200, 200, 200 ) ), 2.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
QPen GraphWidgetUtils::s_connectionPen( QBrush( QColor( 200, 200, 200 ) ), 2.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

float GraphWidgetUtils::g_socketWidth = 10.0f;
float GraphWidgetUtils::g_socketHeight = 10.0f;

///////////////////////////////////////////////////////////////////////////////

/**
 * A utility method for drawing a text with shadow behind it.
 */
void GraphWidgetUtils::drawShadowedText( QPainter* painter, const QRectF& rect, const QString& text, const QTextOption& options )
{
   painter->setPen( s_textShadowPen );
   QRectF textShadowRect = rect;
   textShadowRect.setLeft( textShadowRect.left() + 1.0f );
   textShadowRect.setRight( textShadowRect.right() + 1.0f );
   painter->drawText( textShadowRect, text, options );

   painter->setPen( s_textPen );
   painter->drawText( rect, text, options );
}

///////////////////////////////////////////////////////////////////////////////
