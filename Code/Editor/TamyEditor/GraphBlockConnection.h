/// @file    TamyEditor/GraphBlockConnection.h
/// @brief  graph connection representation
#pragma once

#include <QtWidgets\QGraphicsLineItem>
#include <QtGui\QPainterPath>
#include <QtCore\QRectF>


///////////////////////////////////////////////////////////////////////////////

class GraphBlockSocket;
class GraphBlock;

///////////////////////////////////////////////////////////////////////////////

/**
 * Connection between two graph block sockets.
 *
 * The connection manages the information kept in the participating socket
 * (it's sort of a mediator in this context).
 */
class GraphBlockConnection : public QGraphicsLineItem
{
private:
   GraphBlockSocket*       m_source;
   GraphBlockSocket*       m_destination;

   mutable QPainterPath    m_displayedPath;

public:
   /**
    * Constructor.
    *
    * @param source           source socket
    * @param destination      destination socket
    */
   GraphBlockConnection( GraphBlockSocket* source = NULL, GraphBlockSocket* destination = NULL );

   /**
    * Calculates the bounds of the connection.
    */
   void calculateBounds();

   /**
    * Returns the source socket.
    */
   inline GraphBlockSocket& getSource() { return *m_source; }

   /**
    * Returns the destination socket.
    */
   inline GraphBlockSocket& getDestination() { return *m_destination; }

   /**
    * Checks if this connection connects the specified blocks.
    *
    * @param source
    * @param destination
    */
   bool doesConnect( const GraphBlock* source, const GraphBlock* destination ) const;

   /**
    * Tells if the connection actually connects two blocks ( it may so happen that one
    * of them wasn't deserialized ).
    */
   bool isOk() const;

   // -------------------------------------------------------------------------
   // QGraphicsLineItem implementation
   // -------------------------------------------------------------------------
   QRectF boundingRect() const;
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );
};

///////////////////////////////////////////////////////////////////////////////
