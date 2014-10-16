/// @file   TamyEditor/BlendTreeTransitionItem.h
/// @brief  an item that represents a BlendTreeStateTransition
#pragma once

#include <QtWidgets\QGraphicsItem>
#include <QtGui\QPen>
#include <QtGui\QBrush>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeStateMachineLayout;
class BlendTreeStateTransition;
class BlendTreeStateItem;

///////////////////////////////////////////////////////////////////////////////

/**
 * An item that represents a BlendTreeMachineStateTransition.
 */
class BlendTreeTransitionItem : public QGraphicsItem
{
public:
   static QPen                      s_transitionPen;
   static QPen                      s_selectedTransitionPen;
   static QPen                      s_arrowPen;
   static QPen                      s_selectedArrowPen;
   static QBrush                    s_arrowBrush;

   BlendTreeStateTransition*        m_transition;

   BlendTreeStateItem*              m_startStateItem;
   BlendTreeStateItem*              m_endStateItem;

   QRectF                           m_connectionBounds;
   QPointF                          m_startTerminatorPos;
   QPointF                          m_endTerminatorPos;

public:
   /**
    * Constructor.
    *
    * @param layout
    * @param transition
    */
   BlendTreeTransitionItem( BlendTreeStateMachineLayout* layout, BlendTreeStateTransition* transition );
   ~BlendTreeTransitionItem();

   /**
    * Returns the represented transition.
    */
   inline BlendTreeStateTransition* getTransition() const { return m_transition; }

   /**
    * Calculates the positions of transition line terminators.
    */
   void calculateTerminatorsPos();

   /**
    * A helper method for calculating terminator positions of a drawn connection.
    *
    * @param startStateItem
    * @param mousePos
    * @param outStartPos
    * @param outEndPos
    */
   static void calculateDrawnTerminatorPos( BlendTreeStateItem* startStateItem, const QPointF& mousePos, QPointF& outStartPos, QPointF& outEndPos );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_connectionBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* styleOptions, QWidget* widget );
   void mousePressEvent( QGraphicsSceneMouseEvent* event );

private:
   /**
    * Calculates the connection bounds.
    */
   void calculateBounds();

   static void calculateTerminatorsPos( const QRectF& startBlockBounds, const QRectF& endBlockBounds, QPointF& outStartPos, QPointF& outEndPos );
};

///////////////////////////////////////////////////////////////////////////////
