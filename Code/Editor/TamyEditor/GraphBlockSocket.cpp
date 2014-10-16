#include "GraphBlockSocket.h"
#include <QFontMetrics>
#include <QtGui\QPainter>
#include <QtGui\QBrush>
#include <QtGui\QPen>
#include <QtWidgets\QGraphicsSceneMouseEvent>

#include "GraphLayout.h"
#include "GraphWidgetUtils.h"


///////////////////////////////////////////////////////////////////////////////

GraphBlockSocket::GraphBlockSocket()
   : QGraphicsItem( NULL )
   , m_parent( NULL )
   , m_blockSide( GBSP_INPUT )
   , m_captionVisible( false )
   , m_name( "" )
   , m_font( "Verdana", 8, QFont::Light )
   , m_hoveredOver( false )
{
   setFlag( QGraphicsItem::ItemIsSelectable, true );
   setFlag( QGraphicsItem::ItemIsMovable, false );

   setAcceptTouchEvents( true );
   setAcceptHoverEvents( true );
   setAcceptedMouseButtons( Qt::LeftButton );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::initialize( GraphBlock* parent, GraphBlockSocketPosition pos, const char* name )
{ 
   m_parent = parent;
   m_blockSide = pos;
   m_name = name;
   setParentItem( parent );

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

float GraphBlockSocket::getNameWidth() const
{
   QFontMetrics metrics( m_font );
   return metrics.width( m_name.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );

   QColor bgColor = getBgColor();
   if ( m_hoveredOver )
   {
      bgColor = bgColor.lighter( 150 );
   }

   painter->setPen( m_hoveredOver ? GraphWidgetUtils::s_highlightedBorderPen : GraphWidgetUtils::s_borderPen );
   painter->setBrush( bgColor );
   painter->drawRect( m_bounds );

   if ( m_captionVisible )
   {
      painter->setFont( m_font );
      painter->setBrush( QColor( 0, 0, 0 ) );
      GraphWidgetUtils::drawShadowedText( painter, m_nameBounds, m_name.c_str(), QTextOption( Qt::AlignCenter ) );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::calculateBounds()
{
   QFontMetrics metrics( m_font );
   float nameWidth = metrics.width( m_name.c_str() );
   float fontHeight = metrics.height();

   float textMarginSize = 3.0f;

   switch( m_blockSide )
   {
   case GBSP_INPUT:
      {
         m_bounds = QRectF( QPointF( 0.0f, -GraphWidgetUtils::g_socketHeight * 0.5f), QSizeF( GraphWidgetUtils::g_socketWidth, GraphWidgetUtils::g_socketHeight ) );
         m_nameBounds = QRectF( QPointF( GraphWidgetUtils::g_socketWidth + textMarginSize, -fontHeight * 0.5f ), QSizeF( nameWidth, fontHeight ) );
         break;
      }

   case GBSP_OUTPUT:
      {
         m_bounds = QRectF( QPointF( 0, -GraphWidgetUtils::g_socketHeight * 0.5f ), QSizeF( GraphWidgetUtils::g_socketWidth, GraphWidgetUtils::g_socketHeight ) );
         m_nameBounds = QRectF( QPointF( -nameWidth - textMarginSize, -fontHeight * 0.5f ), QSizeF( nameWidth, fontHeight ) );
         break;
      }
   }

   if ( m_captionVisible )
   {
      m_totalBounds = m_bounds.united( m_nameBounds );
   }
   else
   {
      m_totalBounds = m_bounds;
   }

   // recalculate bounds of the parent
   if ( m_parent )
   {
      m_parent->calculateBounds();
   }

   prepareGeometryChange();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
   GraphLayout* graphScene = dynamic_cast< GraphLayout* >( scene() );
   if ( graphScene && event->button() == Qt::LeftButton )
   {

      if ( m_blockSide == GBSP_INPUT )
      {
         GraphBlockConnection* existingConnection = graphScene->getConnection( m_parent, m_name.c_str() );
         if ( existingConnection )
         {
            // break the existing connection and renegotiate it
            GraphBlockSocket* sourceSocket = &existingConnection->getSource();

            graphScene->disconnectSocket( *this );
            graphScene->startNegotiatingConnection( *sourceSocket );
         }
      }
      else
      {
         graphScene->startNegotiatingConnection( *this );
      }
   }

   QGraphicsItem::mousePressEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsItem::mouseReleaseEvent( event );

   GraphLayout* graphScene = dynamic_cast< GraphLayout* >( scene() );
   if ( graphScene )
   {
      QPointF pos = event->scenePos();

      // there may be multiple items located  at the position we're pointing at now - scan through all of them
      // and find a socket
      QList< QGraphicsItem* > selectedItems = graphScene->items( pos, Qt::IntersectsItemShape, Qt::AscendingOrder );
      GraphBlockSocket* destinationSocket = NULL;
      foreach( QGraphicsItem* item, selectedItems )
      {
         destinationSocket = dynamic_cast< GraphBlockSocket* >( item );
         if ( destinationSocket )
         {
            break;
         }
      }

      graphScene->finishNegotiatingConnection( destinationSocket );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   QGraphicsItem::hoverEnterEvent( event );
   m_hoveredOver = true;
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlockSocket::hoverLeaveEvent( QGraphicsSceneHoverEvent* event )
{
   QGraphicsItem::hoverLeaveEvent( event );
   m_hoveredOver = false;
}

///////////////////////////////////////////////////////////////////////////////
