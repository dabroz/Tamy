#include "BlendTreeStateItem.h"
#include "BlendTreeStateMachineLayout.h"
#include "BlendTreeTransitionItem.h"
#include "core-AI\BlendTreeNode.h"
#include "GraphWidgetUtils.h"
#include "QPropertiesView.h"
#include <QtWidgets\QGraphicsProxyWidget>
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>
#include <QtWidgets\QGraphicsSceneMouseEvent>
#include "core\Vector.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeStateItem::BlendTreeStateItem( BlendTreeNode* node )
   : m_node( node )
   , m_bounds( QPointF( 0, 0 ), QSizeF( 100, 100 ) )
   , m_captionBounds( QPointF( 0, 0 ), QSizeF( 100, 0 ) )
   , m_font( "Verdana", 15, QFont::Light )
   , m_centralWidget( new QGraphicsProxyWidget( this ) )
   , m_embeddedWidget( NULL )
{
   m_font.setStyle( QFont::StyleNormal );
   m_font.setStyleHint( QFont::AnyStyle );
   m_font.setStyleStrategy( QFont::PreferAntialias );

   setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );

   // embed node's properties viewer in the item
   m_embeddedWidget = new QPropertiesView();
   m_embeddedWidget->set( *node );
   m_centralWidget->setWidget( m_embeddedWidget );

   // set block's initial position
   const Vector& blockPos = m_node->getPosition();
   setPos( blockPos[0], blockPos[1] );

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateItem::~BlendTreeStateItem()
{
   m_node = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::calculateBounds()
{
   QFontMetrics metrics( m_font );
   float fontHeight = metrics.height();

   // calculate the size of the caption
   float captionWidth = metrics.width( m_node->getName().c_str() );
   float captionHeight = fontHeight + 6;

   // calculate the bounds
   QRectF centralWidgetBounds = m_centralWidget->subWidgetRect( m_embeddedWidget );

   float blockWidth = max2( (float)centralWidgetBounds.width(), 100.0f ); // make sure the block isn't too narrow
   float blockHeight = captionHeight + centralWidgetBounds.height();

   // set the caption bounds
   m_captionBounds.setWidth( blockWidth );
   m_captionBounds.setHeight( captionHeight );
   m_totalBounds = m_captionBounds;

   // set the block bounds
   m_bounds.setTop( captionHeight );
   m_bounds.setWidth( blockWidth );
   m_bounds.setHeight( blockHeight );

   // calculate the bounds of this block
   m_totalBounds = m_totalBounds.united( m_bounds );

   // set the position of the central widget
   m_centralWidget->setPos( ( blockWidth - centralWidgetBounds.width() ) * 0.5f, captionHeight );

   // include the size of the sockets in the block's bounding box
   m_totalBounds = QRectF( m_totalBounds.left() - 11, m_totalBounds.top(), m_totalBounds.right() + 21, m_totalBounds.bottom() ); 
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::addTransition( BlendTreeTransitionItem* item, bool inbound )
{
   List< BlendTreeTransitionItem* >& transitionsList = inbound ? m_inboundTransitions : m_outboundTransitions;

   // verify that we don't have this transition on our list already
   List< BlendTreeTransitionItem* >::iterator it = ListUtils::find( transitionsList, item );
   if ( it.isEnd() )
   {
      transitionsList.pushBack( item );
   }

   // recalculate transition's bounds
   item->calculateTerminatorsPos();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::removeTransition( BlendTreeTransitionItem* item, bool inbound )
{
   List< BlendTreeTransitionItem* >& transitionsList = inbound ? m_inboundTransitions : m_outboundTransitions;

   // verify that we don't have this transition on our list already
   List< BlendTreeTransitionItem* >::iterator it = ListUtils::find( transitionsList, item );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* styleOptions, QWidget* widget )
{
   QLinearGradient fillBrush( QPointF(0, 0), QPointF(0, m_captionBounds.height()));
   fillBrush.setColorAt( 0, isSelected() ? QColor( 255, 226, 96 ) : QColor( 200, 200, 200 ) );
   fillBrush.setColorAt( 0.5f, QColor( 70, 70, 70 ) );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );
   painter->setFont( m_font );

   const float edgeRadius = 3.0f;

   // draw the block's layout
   {
      painter->setPen( GraphWidgetUtils::s_borderPen );

      painter->setBrush( QColor( 150, 150, 150 ) );
      painter->setOpacity( 0.5f );
      painter->drawRoundedRect( m_bounds, edgeRadius, edgeRadius );
   }

   // draw the header
   {

      QPainterPath path;

      QRectF topBB = m_captionBounds;
      topBB.setHeight( m_captionBounds.height() + edgeRadius );

      QRectF bottomBB = m_captionBounds;
      bottomBB.setTop( m_captionBounds.height() );
      bottomBB.setHeight( edgeRadius );

      path.addRoundedRect( topBB, edgeRadius, edgeRadius );
      path.addRect( bottomBB );

      painter->setOpacity( 1.0f );
      painter->setPen( Qt::NoPen );
      painter->setBrush( fillBrush );
      painter->drawPath( path );

      
      GraphWidgetUtils::drawShadowedText( painter, m_captionBounds, m_node->getName().c_str(), QTextOption( Qt::AlignCenter ) );
   }


   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsItem::mousePressEvent( event );

   // show node's properties
   BlendTreeStateMachineLayout* layout = static_cast< BlendTreeStateMachineLayout* >( scene() );
   layout->objectSelected( m_node );

   if ( event->button() == Qt::MiddleButton )
   {
      // start drawing a transition
      layout->startDrawingTransition( this );

      // consume the event
      event->accept();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsItem::mouseMoveEvent( event );

   QPointF blockPos = pos();
   Vector nodePos( blockPos.x(), blockPos.y(), 0 );
   m_node->setPosition( nodePos );

   updateTransitionsEndpoints();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateItem::updateTransitionsEndpoints()
{
   // inbound
   QPointF startPos, endPos;
   for ( List< BlendTreeTransitionItem* >::iterator it = m_inboundTransitions.begin(); !it.isEnd(); ++it )
   {
      BlendTreeTransitionItem* transitionItem = *it;
      transitionItem->calculateTerminatorsPos();
   }

   // outbound
   for ( List< BlendTreeTransitionItem* >::iterator it = m_outboundTransitions.begin(); !it.isEnd(); ++it )
   {
      BlendTreeTransitionItem* transitionItem = *it;
      transitionItem->calculateTerminatorsPos();
   }
}

///////////////////////////////////////////////////////////////////////////////
