#include "BlendTreeItemsRenderers.h"
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>
#include <QLinearGradient>


///////////////////////////////////////////////////////////////////////////////

void BlendTreeItemsRenderers::paintEvent( QPainter* painter, const QRectF& globalBounds )
{
   QLinearGradient fillBrush( QPointF(0, 0), QPointF( 0, 36 ) );
   fillBrush.setColorAt( 0, QColor( 255, 126, 48 ) );
   fillBrush.setColorAt( 1.0f, QColor( 253, 93, 14 ) );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );

   QPainterPath path;
   path.addRoundedRect( globalBounds, 4.0f, 4.0f );

   painter->setBrush( fillBrush );
   painter->drawPath( path );

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeItemsRenderers::paintSyncPoint( QPainter* painter, const QRectF& globalBounds, float opaqueness )
{
   QLinearGradient fillBrush( QPointF(0, 0), QPointF( 0, 36 ) );
   fillBrush.setColorAt( 0, QColor( 0, 196, 229, 255 * opaqueness ) );
   fillBrush.setColorAt( 0.1f, QColor( 0, 187, 229, 255 * opaqueness ) );
   QPen pen( QColor( 0, 0, 0, 255 * opaqueness) );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );

   QPainterPath path;
   path.addRoundedRect( globalBounds, 4.0f, 4.0f );

   painter->setBrush( fillBrush );
   painter->setPen( pen );
   painter->drawPath( path );

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////
