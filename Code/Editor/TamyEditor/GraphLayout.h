/// @file   TamyEditor/GraphLayout.h
/// @brief  graph layout interface
#ifndef _GRAPH_LAYOUT_H
#define _GRAPH_LAYOUT_H

#include <vector>
#include <QPointF>
#include "core.h"
#include "GraphBlock.h"
#include <QtWidgets\QGraphicsScene>
#include <QtWidgets\QAction>
#include "GraphBlockConnection.h"
#include "TypeDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

class QGraphicsLineItem;

///////////////////////////////////////////////////////////////////////////////

/**
 * An abstract graph layout interface. It stores a list of blocks, however those
 * are managed by a solid class implementing this interface.
 * The class should be a resource that will store the layout data and provide
 * a link with a model resource the blocks represent.
 */
class GraphLayout : public QGraphicsScene
{
   Q_OBJECT

public:
   virtual ~GraphLayout() {}

   /**
    * Adds a new block to the layout.
    *
    * @param block
    */
   virtual void add( const SerializableReflectionType& type, const QPointF& pos ) = 0;

   /**
    * Removes a block from the layout.
    *
    * @param block
    */
   virtual void remove( GraphBlock* block ) = 0;

   /**
    * Breaks all connections to or from the specified socket.
    *
    * @param socket
    */
   virtual void disconnectSocket( GraphBlockSocket& socket ) = 0;

   /**
    * Creates a context menu for the graph.
    *
    * @param menu       menu to fill in with entries
    * @param scenePos   position on the scene clicked
    */
   virtual void createContextMenu( QMenu* menu, const QPointF& scenePos ) = 0;

   /**
    * Opens the process of connecting two blocks through their sockets.
    *
    * @param sourceSocket
    */
   virtual void startNegotiatingConnection( GraphBlockSocket& sourceSocket ) = 0;

   /**
    * Finishes the process of connecting two blocks through their sockets.
    *
    * @param destinationSocket
    */
   virtual void finishNegotiatingConnection( GraphBlockSocket* destinationSocket ) = 0;

   /**
    * Finds a connection that ends in the specified input of the specified node.
    *
    * @param endNode
    * @param inputName
    */
   virtual GraphBlockConnection* getConnection( const GraphBlock* endNode, const char* inputName ) const = 0;

   /**
    * Called when a block is moved so that we can update bounds of all attached connections.
    */
   virtual void onBlockMoved() = 0;
};

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
class TGraphLayout : public GraphLayout, public GraphBuilderListener< TNode >
{
private:
   struct Callback
   {
      virtual ~Callback() {}

      virtual bool canBeExecuted( GraphBlock* block ) const = 0;

      virtual void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block ) const = 0;
   };

   template< typename TSpecificNode, typename TSpecificCallback >
   struct TCallback : public Callback
   {
      bool canBeExecuted( GraphBlock* block ) const;
      void onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block ) const;
   };

protected:
   typedef GenericFactory< GBNodeSocket, GraphBlockSocket >    SocketsFactory;

private:
   GraphBuilder< TNode >&                             m_graphBuilder;
   List< Callback* >                                  m_callbacks;

   SocketsFactory*                                    m_socketsFactory;
   TypeDescFactory< TNode >*                          m_itemDescriptionsFactory;    // types of nodes we can add form a context menu
   List< const SerializableReflectionType* >          m_nonRemovableNodeTypes;      // types of nodes that won't have a "Remove" option enabled in their context menus

   std::vector< GraphBlock* >                         m_blocks;
   std::vector< GraphBlockConnection* >               m_connections;       // connections between the blocks

   GraphBlockSocket*                                  m_sourceSocket;      // memorized graph socket instance where a newly created connection starts at
   QGraphicsLineItem*                                 m_drawnConnection;

   QPointF                                            m_currentMousePos;

   List< GraphBlock* >                                m_expandedBlocks;


   // clipboard
   std::vector< TNode* >                              m_copiedNodes;
   GraphBuilder< TNode >                              m_copiedGraph;

public:
   /**
    * Constructor.
    *
    * @param graphBuilder
    */
   TGraphLayout( GraphBuilder< TNode >& graphBuilder );
   virtual ~TGraphLayout();

   /**
    * Registers a callback that will be triggered whenever the user
    * executes a certain action on a block that represents a node of the specified type.
    */
   template< typename TSpecificNode, typename TSpecificCallback >
   void registerBlockCallback();

   /**
    * Defines a block type that cannot be added or removed using the context menus.
    */
   template< typename TSpecificNode >
   void defineUnremovableType();

   /**
    * Returns the current mouse cursor position over the layout - expressed in scene coordinates.
    */
   inline const QPointF& getCurrentMousePos() const {
      return m_currentMousePos;
   }

   // -------------------------------------------------------------------------
   // GraphLayout implementation
   // -------------------------------------------------------------------------
   void add( const SerializableReflectionType& type, const QPointF& pos );
   void remove( GraphBlock* block );
   void disconnectSocket( GraphBlockSocket& socket );
   void createContextMenu( QMenu* menu, const QPointF& scenePos );
   void startNegotiatingConnection( GraphBlockSocket& sourceSocket );
   void finishNegotiatingConnection( GraphBlockSocket* destinationSocket );
   GraphBlockConnection* getConnection( const GraphBlock* endNode, const char* inputName ) const;
   void onBlockMoved();

   // -------------------------------------------------------------------------
   // GraphBuilderObserver implementation
   // -------------------------------------------------------------------------
   void onNodeCreated( TNode* node );
   void onNodeRemoved( TNode* node );
   void onConnectionCreated( const TNode* startNode, const char* outputName, const TNode* endNode, const char* inputName );
   void onConnectionRemoved( const TNode* endNode, const char* inputName );
   void onGraphCleared();

protected:
   /**
    * Initializes the widget.
    */
   void initialize();

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   /**
    * Called in order to setup a sockets factory.
    *
    * @param factory
    */
   virtual void initSocketsFactory( SocketsFactory& factory ) = 0;

   // -------------------------------------------------------------------------
   // QGraphicsScene implementation
   // -------------------------------------------------------------------------
   void mouseMoveEvent( QGraphicsSceneMouseEvent* mouseEvent );
   void dragMoveEvent( QGraphicsSceneDragDropEvent* event );
   void keyPressEvent( QKeyEvent* keyEvent );

private:
   /**
    * Finds a block that corresponds to the specified node.
    */
   GraphBlock* getNodeBlock( const TNode& node ) const;

   /**
    * Checks if the specified node can be removed.
    *
    * @param node
    */
   bool canBeRemoved( const ReflectionObject* node ) const;

   /**
    * Creates a menu that allows to add new nodes to the layout.
    *
    * @param parentMenu
    * @param scenePos
    */
   void createAddNodesMenu( QMenu* parentMenu, const QPointF& scenePos );

   /**
   * Updates the mouse position and expands the items the mouse is pointing at.
   *
   * @param pos        position of mouse in the scene coordinates
   */
   void updateMousePosition( const QPointF& pos );

};

///////////////////////////////////////////////////////////////////////////////

class GraphNodeCreationAction : public QAction
{
   Q_OBJECT

private:
   GraphLayout&                        m_parent;
   const SerializableReflectionType&   m_type;
   QPointF                             m_pos;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param type        type of the node added
    * @param pos         position in graph onto which the node should be added
    */
   GraphNodeCreationAction( GraphLayout& parent, const SerializableReflectionType& type, const QPointF& pos );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class GraphNodeRemoveAction : public QAction
{
   Q_OBJECT

private:
   GraphLayout&               m_parent;
   QList< GraphBlock* >    m_blocks;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blocks       blocks to remove
    */
   GraphNodeRemoveAction( GraphLayout& parent, const QList< GraphBlock* >& blocks );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

#include "GraphLayout.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_LAYOUT_H
