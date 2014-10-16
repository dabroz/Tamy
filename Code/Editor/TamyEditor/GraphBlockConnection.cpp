#include "GraphBlockConnection.h"
#include "GraphBlockSocket.h"
#include "GraphWidgetUtils.h"
#include <QtGui\QPainter>


///////////////////////////////////////////////////////////////////////////////

GraphBlockConnection::GraphBlockConnection( GraphBlockSocket* source, GraphBlockSocket* destination )
   : m_source( source )
   , m_destination( destination )
{
   setFlags( QGraphicsItem::ItemIsSelectable );

   if ( m_source && m_destination )
   {
      calculateBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockConnection::calculateBounds()
{
   if ( !m_source || !m_destination )
   {
      return;
   }

   prepareGeometryChange();

   QPointF start = m_source->scenePos();
   QPointF end = m_destination->scenePos();

   setLine( start.rx(), start.ry(), end.rx(), end.ry() );
}

///////////////////////////////////////////////////////////////////////////////

bool GraphBlockConnection::isOk() const
{
   if ( !m_source || !m_source->parentItem() )
   {
      return false;
   }

   if ( !m_destination || !m_destination->parentItem() )
   {
      return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool GraphBlockConnection::doesConnect( const GraphBlock* source, const GraphBlock* destination ) const
{
   GraphBlock* sourceBlock = m_source->getParentBlock();
   GraphBlock* destinationBlock = m_destination->getParentBlock();
   return ( source == sourceBlock ) && ( destination == destinationBlock );
}

///////////////////////////////////////////////////////////////////////////////

QRectF GraphBlockConnection::boundingRect() const
{
   QLineF l = line();

   m_displayedPath = QPainterPath();
   m_displayedPath.moveTo( l.p1() );
   QPointF c1, c2;
   c1 = l.p1();
   c1.setX( c1.x() + 50.0f );

   c2 = l.p2();
   c2.setX( c2.x() - 50.0f );

   m_displayedPath.cubicTo( c1, c2, l.p2() );

   QRectF rect = m_displayedPath.boundingRect();
   if ( rect.height() < 10 )
   {
      rect.setHeight( 10 );
   }
   if ( rect.width() < 10 )
   {
      rect.setWidth( 10 );
   }

   return rect;
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockConnection::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setBrush( Qt::NoBrush );
   painter->setPen( GraphWidgetUtils::s_connectionPen );
   painter->drawPath( m_displayedPath );

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////
