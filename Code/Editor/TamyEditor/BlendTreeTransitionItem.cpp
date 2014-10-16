#include "BlendTreeTransitionItem.h"
#include "BlendTreeStateItem.h"
#include "BlendTreeStateMachineLayout.h"
#include "BlendTreeStateMachineEditor.h"
#include "core-AI\BlendTreeStateTransition.h"
#include <QtGui\QPainterPath>
#include <QtGui\QPainter>
#include "core\Vector.h"
#include "core\VectorUtil.h"


///////////////////////////////////////////////////////////////////////////////

QPen BlendTreeTransitionItem::s_transitionPen( QBrush( QColor( 170, 170, 170 ) ), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin );
QPen BlendTreeTransitionItem::s_selectedTransitionPen( QBrush( QColor( 255, 226, 96 ) ), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin );
QPen BlendTreeTransitionItem::s_arrowPen( QBrush( QColor( 170, 170, 170 ) ), 2, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin );
QPen BlendTreeTransitionItem::s_selectedArrowPen( QBrush( QColor( 255, 226, 96 ) ), 2, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin );
QBrush BlendTreeTransitionItem::s_arrowBrush( QColor( 140, 140, 140 ) );

///////////////////////////////////////////////////////////////////////////////

BlendTreeTransitionItem::BlendTreeTransitionItem( BlendTreeStateMachineLayout* layout, BlendTreeStateTransition* transition )
   : m_transition( transition )
   , m_startStateItem( NULL ) 
   , m_endStateItem( NULL )
{
   setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );

   m_startStateItem = layout->findStateItem( transition->m_startState );
   m_endStateItem = layout->findStateItem( transition->m_endState );

   ASSERT( m_startStateItem != NULL );
   ASSERT( m_endStateItem != NULL );

   calculateTerminatorsPos();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeTransitionItem::~BlendTreeTransitionItem()
{
   m_transition = NULL;
   m_startStateItem = NULL;
   m_endStateItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::calculateTerminatorsPos()
{
   QRectF startStateBounds = m_startStateItem->sceneBoundingRect();
   QRectF endStateBounds = m_endStateItem->sceneBoundingRect(); 
   calculateTerminatorsPos( startStateBounds, endStateBounds, m_startTerminatorPos, m_endTerminatorPos );
  
   calculateBounds();

   // redraw the transition
   update();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::calculateDrawnTerminatorPos( BlendTreeStateItem* startStateItem, const QPointF& mousePos, QPointF& outStartPos, QPointF& outEndPos )
{
   QRectF startStateBounds = startStateItem->sceneBoundingRect();
   QRectF endStateBounds( mousePos, mousePos );

   calculateTerminatorsPos( startStateBounds, endStateBounds, outStartPos, outEndPos );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::calculateTerminatorsPos( const QRectF& startBlockBounds, const QRectF& endBlockBounds, QPointF& outStartPos, QPointF& outEndPos )
{
   QPointF startStateCenter = startBlockBounds.center();
   QPointF endStateCenter = endBlockBounds.center();

   // If we have a two-way connection between two nodes ( an outbound and an inbound connection ),
   // they will overlap. The easiest way to get rid of that is to introduce a bias that depends on the direction the connection
   // is going in
   float biasVal = 10.0f;

   // use vertical boundaries ONLY if the blocks haven't exceeded each other's vertical limits - otherwise
   // select the closest horizontal boundary and attach the connection to it
   if ( startBlockBounds.bottom() <= endBlockBounds.top() )
   {
      // the blocks aren't adjusted vertically, so we need to go with their horizontal boundaries
      outStartPos = QPointF( startStateCenter.x() + biasVal, startBlockBounds.bottom() );
      outEndPos = QPointF( endStateCenter.x() + biasVal, endBlockBounds.top() );
   }
   else if ( endBlockBounds.bottom() <= startBlockBounds.top() )
   {
      // the blocks aren't adjusted vertically, so we need to go with their horizontal boundaries
      outStartPos = QPointF( startStateCenter.x() - biasVal, startBlockBounds.top() );
      outEndPos = QPointF( endStateCenter.x() - biasVal, endBlockBounds.bottom() );
   }
   else if ( startBlockBounds.right() <= endBlockBounds.left() )
   {
      // we know that the two blocks are vertically aligned - and we also know
      // that the start block is to the left of the end block )

      outStartPos = QPointF( startBlockBounds.right(), startStateCenter.y() + biasVal );
      outEndPos = QPointF( endBlockBounds.left(), endStateCenter.y() + biasVal );
   }
   else
   {
      // we know that the two blocks are vertically aligned - and we also know
      // that the start block is to the right of the end block )

      outStartPos = QPointF( startBlockBounds.left(), startStateCenter.y() - biasVal );
      outEndPos = QPointF( endBlockBounds.right(), endStateCenter.y() - biasVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::calculateBounds()
{
   float top, bottom, left, right;
   if ( m_startTerminatorPos.y() < m_endTerminatorPos.y() )
   {
      top = m_startTerminatorPos.y();
      bottom = m_endTerminatorPos.y();
   }
   else
   {
      bottom = m_startTerminatorPos.y();
      top = m_endTerminatorPos.y();
   }

   if ( m_startTerminatorPos.x() < m_endTerminatorPos.x() )
   {
      left = m_startTerminatorPos.x();
      right = m_endTerminatorPos.x();
   }
   else
   {
      right = m_startTerminatorPos.x();
      left = m_endTerminatorPos.x();
   }

   m_connectionBounds.setTopLeft( QPointF( left, top ) );
   m_connectionBounds.setBottomRight( QPointF( right, bottom ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* styleOptions, QWidget* widget )
{   
   painter->save();

   // calculate arrow points
   QPointF barPtA, barPtB, barOrig;
   {
      static FastFloat barSize = FastFloat::fromFloat( 20.0f );
      static FastFloat barHalfSize = FastFloat::fromFloat( 10.0f );

      Vector startPos( m_startTerminatorPos.x(), m_startTerminatorPos.y(), 0 );
      Vector endPos( m_endTerminatorPos.x(), m_endTerminatorPos.y(), 0 );

      Vector lineDisp;
      lineDisp.setSub( endPos, startPos );

      FastFloat lineLen = lineDisp.length();
      Vector lineDir;
      if ( lineLen <= Float_1e_3 )
      {
         lineDir = Vector_OX;
         lineLen = Float_1;
      }
      else
      {
         lineDir.setNormalized( lineDisp );
      }

      Vector barDir;
      VectorUtil::calculatePerpendicularVector( lineDir, barDir );

      lineLen.sub( barSize );
      Vector barOrigin;
      barOrigin.setMulAdd( lineDir, lineLen, startPos );

      barDir.mul( barHalfSize );

      Vector barPointA, barPointB;
      barPointA.setAdd( barOrigin, barDir );
      barPointB.setSub( barOrigin, barDir );

      barPtA = QPointF( barPointA[0], barPointA[1] );
      barPtB = QPointF( barPointB[0], barPointB[1] );
      barOrig = QPointF( barOrigin[0], barOrigin[1] );
   }

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );
   painter->setOpacity( 0.6f );

   // draw the line
   {
      QPainterPath path;
      path.moveTo( m_startTerminatorPos );
      path.lineTo( barOrig );

      painter->setPen( isSelected() ? s_selectedTransitionPen : s_transitionPen );
      painter->drawPath( path );
   }

   // draw the arrow
   { 
      QPainterPath path;
      path.moveTo( m_endTerminatorPos );
      path.lineTo( barPtA );
      path.lineTo( barPtB );
      path.lineTo( m_endTerminatorPos );
      path.setFillRule( Qt::WindingFill );

      painter->setPen( isSelected() ? s_selectedArrowPen : s_arrowPen );
      painter->setBrush( s_arrowBrush );
      painter->drawPath( path );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeTransitionItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsItem::mousePressEvent( event );

   // show node's properties
   BlendTreeStateMachineLayout* layout = static_cast< BlendTreeStateMachineLayout* >( scene() );
   layout->objectSelected( m_transition );
}

///////////////////////////////////////////////////////////////////////////////
