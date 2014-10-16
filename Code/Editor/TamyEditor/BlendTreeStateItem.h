/// @file   TamyEditor/BlendTreeStateItem.h
/// @brief  an item that represents a BlendTreeNode in the context of a BlendTreeStateMachine
#pragma once

#include <QtWidgets\QGraphicsItem>
#include <QtCore\QRectF>
#include <QtGui\QFont>
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class QGraphicsProxyWidget;
class QPropertiesView;
class BlendTreeTransitionItem;

///////////////////////////////////////////////////////////////////////////////

/**
 * An item that represents a BlendTreeNode in the context of a BlendTreeStateMachine.
 */
class BlendTreeStateItem : public QGraphicsItem
{
private:
   BlendTreeNode*                      m_node;

   QPointF                             m_position;
   QRectF                              m_bounds;
   QRectF                              m_captionBounds;
   QRectF                              m_totalBounds;

   QFont                               m_font;

   QGraphicsProxyWidget*               m_centralWidget;
   QPropertiesView*                    m_embeddedWidget;

   List< BlendTreeTransitionItem* >    m_outboundTransitions;
   List< BlendTreeTransitionItem* >    m_inboundTransitions;

public:
   /**
    * Constructor.
    *
    * @param node
    */
   BlendTreeStateItem( BlendTreeNode* node );
   ~BlendTreeStateItem();

   /**
    * Returns the represented node.
    */
   inline BlendTreeNode* getNode() const { return m_node; }

   /**
    * Calculates item's size and bounds.
    */
   void calculateBounds();

   // -------------------------------------------------------------------------
   // Transitions
   // -------------------------------------------------------------------------

   /**
    * Adds a transition this node is involved in.
    * The transitions are managed externally.
    *
    * @param item
    * @param inbound       is transition inbound or outbound
    */
   void addTransition( BlendTreeTransitionItem* item, bool inbound );

   /**
    * Removes a transition this node was involved in.
    * The transitions are managed externally.
    *
    * @param item
    * @param inbound       is transition inbound or outbound
    */
   void removeTransition( BlendTreeTransitionItem* item, bool inbound );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_totalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* styleOptions, QWidget* widget );
   void mousePressEvent( QGraphicsSceneMouseEvent* event );
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

private:
   void updateTransitionsEndpoints();
};

///////////////////////////////////////////////////////////////////////////////
