/// @file      TamyEditor/GraphBlock.h
/// @brief     a graphical representation of a graph node
#ifndef _GRAPH_BLOCK_H
#define _GRAPH_BLOCK_H

#include "core\ReflectionObject.h"
#include "core\GenericFactory.h"
#include "core\GraphBuilderSockets.h"
#include "core\GraphBuilderNode.h"
#include <QtWidgets\QGraphicsProxyWidget>
#include <QtWidgets\QGraphicsItem>
#include <QtWidgets\QLineEdit> 
#include <QtGui\QColor>
#include <QtCore\QRectF>
#include <QtGui\QPen>
#include <QtGui\QFont>
#include <set>


///////////////////////////////////////////////////////////////////////////////

class QPainter;
class QGraphicsScene;
class GraphBlockSocket;
enum GraphBlockSocketPosition;
class QLineEdit;
class GraphLayout;

///////////////////////////////////////////////////////////////////////////////

/**
 * A graphical representation of a graph node.
 */
class GraphBlock : public QGraphicsItem
{
protected:
   // sockets
   typedef GenericFactory< GBNodeSocket, GraphBlockSocket >   SocketsFactory;

   std::vector< GraphBlockSocket* >    m_sockets;
   SocketsFactory*                     m_socketsFactory;

   unsigned int                        m_leftSocketsCount;
   unsigned int                        m_rightSocketsCount;

protected:
   QString                             m_caption;

private:
   GraphLayout*                        m_scene;
   bool                                m_contentsVisible;

   QPointF                             m_position;
   QRectF                              m_bounds;
   QRectF                              m_captionBounds;
   QRectF                              m_totalBounds;

   QFont                               m_font;

   QGraphicsProxyWidget*               m_renameWidget;
   QLineEdit*                          m_newNameEditor;

   QGraphicsProxyWidget*               m_centralWidget;
   QWidget*                            m_embeddedWidget;

public:
   /**
    * Constructor.
    *
    * @param scene
    */
   GraphBlock( GraphLayout* scene );
   virtual ~GraphBlock();

   /**
    * Mandatory block initialization method.
    *
    * @param socketsFactory
    */
   virtual void initialize( SocketsFactory* socketsFactory ) = 0;

   /**
    * Deinitializes the block.
    */
   void deinitialize();

   /**
   * Sets the caption of the block.
   *
   * @param caption
   */
   void setCaption( const QString& caption );

   /**
    * Sets a widget in the center of the block.
    *
    * @param widget
    */
   void setCentralWidget( QWidget* widget );

   /**
    * When expanded, the block will show the embedded widget and all the sockets lain out nicely.
    * Contracted, it will try to cover the least amount of space possible, merging all sockets into one etc.
    *
    * @param expanded
    */
   void setExpanded( bool expanded );

   /**
    * Checks if the block overlaps the specified position.
    * 
    * @param pos
    */
   bool doesOverlap( const QPointF& pos ) const;

   /**
    * Returns the represented node instance.
    */
   virtual ReflectionObject* getNode() { return NULL; }

   /**
    * Looks for a socket with the specified name and position.
    *
    * @param position
    * @param name
    */
   GraphBlockSocket* getSocket( GraphBlockSocketPosition position, const std::string& name ) const;

   /**
    * Extracts a list of all socket names located on the specified position.
    *
    * @param position
    * @param outSocketNames
    */
   void getAllSockets( GraphBlockSocketPosition position, std::set< std::string >& outSocketNames ) const;

   /**
    * Gives access to the array of sockets
    */
   const std::vector< GraphBlockSocket* >& accessSockets() const { return m_sockets; }

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_totalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
   void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event );

   // -------------------------------------------------------------------------
   // Block layout operation
   // -------------------------------------------------------------------------
   /**
    * Forces the block to recalculate its layout
    */
   void calculateBounds();

   // -------------------------------------------------------------------------
   // Block layout settings
   // -------------------------------------------------------------------------
   /**
    * Adds a new socket to the block.
    *
    * @param socket
    */
   void addSocket( GraphBlockSocket* socket );

   /**
    * Removes a single socket on the specified position with the specified name, deleting its instance 
    * and getting rid of any connections that use them.
    *
    * @param position
    * @param socketName
    */
   void removeSocket( GraphBlockSocketPosition position, const std::string& socketName );

   /**
    * Removes sockets on the specified position with the specified names in bulk, deleting their instances
    * and getting rid of any connections that use them.
    *
    * @param position
    * @param socketNames
    */
   void removeSockets( GraphBlockSocketPosition position, const std::set< std::string >& socketNames );

protected:
   /**
    * A template method the implementation of which ( one that knows the type of the node ) will update its name.
    */
   virtual void updateNodeName() = 0;

private:
   void removeSingleSocket( GraphBlockSocketPosition position, const std::string& socketName );
   void calculateBoundsWithContents();
   void calculateEmptyBlockBounds();
};

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
class TGraphBlock : public GraphBlock, public GraphBuilderNodeListener< TNode >
{
protected:
   TNode*                                                m_nodePtr;
   
public:
   /**
    * Default constructor.
    */
   TGraphBlock( GraphLayout* scene = NULL );

   /**
    * Constructor.
    *
    * @param scene
    * @param node       represented node
    */
   TGraphBlock( GraphLayout* scene, TNode& node );
   virtual ~TGraphBlock();

   /**
    * Sets an instance of sockets factory this block should use to create its sockets
    */
   void initialize( SocketsFactory* socketsFactory );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

   // -------------------------------------------------------------------------
   // GraphBlock implementation
   // -------------------------------------------------------------------------
   ReflectionObject* getNode();
   void updateNodeName();

   // -------------------------------------------------------------------------
   // GraphBuilderNodeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAppearance( TNode* node );
   void onInputAdded( TNode* node, GBNodeInput< TNode >* input );
   void onOutputAdded( TNode* node, GBNodeOutput< TNode >* output );
   void onInputRemoved( TNode* node, GBNodeInput< TNode >* input );
   void onOutputRemoved( TNode* node, GBNodeOutput< TNode >* output );

private:
   void initSocketsFactory();
};

///////////////////////////////////////////////////////////////////////////////

class BlockCaptionEditor : public QLineEdit
{
   Q_OBJECT

private:
   GraphBlock*       m_block;

public:
   BlockCaptionEditor( GraphBlock* block );
   ~BlockCaptionEditor();

public slots:
   void blockRenamed();
};

///////////////////////////////////////////////////////////////////////////////

#include "GraphBlock.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BLOCK_H
