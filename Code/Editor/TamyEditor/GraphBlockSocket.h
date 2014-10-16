/// @file   TamyEditor/GraphBlockSocket.h
/// @brief  pipeline socket representation
#ifndef _PIPELINE_SOCKET_H
#define _PIPELINE_SOCKET_H

#include "GraphBlock.h"
#include <QtWidgets\QGraphicsItem>
#include <QtCore\QRectF>
#include <QtGui\QFont>


///////////////////////////////////////////////////////////////////////////////

enum GraphBlockSocketPosition
{
   GBSP_INPUT,
   GBSP_OUTPUT,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A single socket placed on a graph block.
 *
 * Each socket maintains a list of all the connections it participates
 * in so that when it changes it's position, it can inform the connection
 * that it needs to recalculate its bounds.
 * 
 * The socket is responsible for notifying a block that a new connection
 * has been established - providing it's the source socket.
 * The responsibility of a destination socket is just to inform the connection
 * about the changes in its whereabouts.
 */
class GraphBlockSocket : public QGraphicsItem
{
   friend class GraphBlockConnection;

private:
   GraphBlock*                               m_parent;

   GraphBlockSocketPosition                  m_blockSide;
   std::string                               m_name;

   bool                                      m_captionVisible;
   bool                                      m_hoveredOver;
   QFont                                     m_font;
   QRectF                                    m_bounds;
   QRectF                                    m_nameBounds;
   QRectF                                    m_totalBounds;
   QPointF                                   m_position;

public:
   /**
    * Constructor.
    */
   GraphBlockSocket();

   /**
    * Initializes the socket
    *
    * @param parent  parent block
    * @param pos     socket's position ( left or right )
    * @param name    socket's name
    */
   void initialize( GraphBlock* parent, GraphBlockSocketPosition pos, const char* name );

   /**
    * Shows/hides socket caption.
    */
   inline void setCaptionVisible( bool visible ) {
      m_captionVisible = visible;
   }

   /**
    * Returns the parent graph block.
    */
   inline GraphBlock* getParentBlock() const { return m_parent; }

   /**
    * Returns the socket's name.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Returns the current socket position.
    */
   inline GraphBlockSocketPosition getPosition() const { return m_blockSide; }

   /**
    * Returns the color of the socket.
    */
   virtual QColor getBgColor() const { return QColor( 0, 0, 0 ); }

   /**
    * Returns the width of the socket's name.
    */
   float getNameWidth() const;

   /**
    * Calculate the bounds of the socket.
    */
   void calculateBounds();

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   QRectF boundingRect() const { return m_totalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

protected:
   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void mousePressEvent( QGraphicsSceneMouseEvent* event );
   void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
   void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
   void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );
};

///////////////////////////////////////////////////////////////////////////////

#define DECLARE_PIPELINE_SOCKET( Class, NodeType, r, g, b )                   \
class Class : public GraphBlockSocket                                         \
{                                                                             \
public:                                                                       \
   Class() : GraphBlockSocket() {}                                            \
   Class( GBNodeSocket& socket ) : GraphBlockSocket() {}                      \
   QColor getBgColor() const { return QColor( r, g, b ); }                    \
};

///////////////////////////////////////////////////////////////////////////////


#endif // _PIPELINE_SOCKET_H
