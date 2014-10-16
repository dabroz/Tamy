#ifndef _GRAPH_LAYOUT_H
#error "This file can be included only in GraphLayout.h"
#else

#include "GraphBlock.h"
#include "GraphBlockSocket.h"
#include <algorithm>
#include <QtWidgets\QGraphicsSceneMouseEvent>
#include <QtGui\QPainter>
#include <QtWidgets\QMenu>
#include <QtWidgets\QGraphicsLineItem>
#include <QtWidgets\QApplication>
#include <QtGui\QBrush>
#include <QtGui\QImage>
#include <QtGui\QKeyEvent>

// ui utils
#include "MenuUtils.h"


/////////////////////////////////////////////////////////////////////////////////////////

template< typename TNode >
TGraphLayout< TNode >::TGraphLayout( GraphBuilder< TNode >& graphBuilder )
   : m_graphBuilder( graphBuilder )
   , m_sourceSocket( NULL )
   , m_drawnConnection( NULL )
{
   moveToThread( QApplication::instance()->thread() );

   QImage image( ":/TamyEditor/Resources/Icons/Editor/pipelineEditorBackground.png" );
   QBrush bgImageBrush( image );
   setBackgroundBrush( bgImageBrush );

   m_copiedGraph.setGraphBuilderNodes( &m_copiedNodes );

   m_socketsFactory = new SocketsFactory();

   // initialize items descriptors factory
   m_itemDescriptionsFactory = new TypeDescFactory< TNode >( "unknownLayoutNodeIcon.png" );
}

/////////////////////////////////////////////////////////////////////////////////////////

template< typename TNode >
TGraphLayout< TNode >::~TGraphLayout()
{
   for ( List< Callback* >::iterator it = m_callbacks.begin(); !it.isEnd(); ++it )
   {
      Callback* callback = *it;
      delete callback;
   }
   m_callbacks.clear();

   m_copiedGraph.clear();
   m_graphBuilder.detachGraphListener( this );

   delete m_itemDescriptionsFactory;
   m_itemDescriptionsFactory = NULL;

   delete m_socketsFactory;
   m_socketsFactory = NULL;

   // the blocks are managed by an outside resource
   uint count = m_blocks.size();
   for ( uint i = 0; i < count; ++i )
   {
      GraphBlock* block = m_blocks[i];
      block->deinitialize();
      delete block;
   }
   m_blocks.clear();

   count = m_connections.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_connections[i];
   }
   m_connections.clear();

   m_sourceSocket = NULL;
   m_drawnConnection = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::initialize() 
{
   // initialize factories
   initSocketsFactory( *m_socketsFactory );
   
   m_graphBuilder.attachGraphListener( this );
   m_graphBuilder.pullGraphStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::add( const SerializableReflectionType& type, const QPointF& pos )
{
   TNode* node = type.instantiate< TNode >();
   node->setPosition( Vector( pos.x(), pos.y(), 0 ) );
   GraphBuilderTransaction< TNode > transaction( m_graphBuilder );
   transaction.addNode( node );
   transaction.commit();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::remove( GraphBlock* block )
{
   if ( block == NULL )
   {
      return;
   }

   TNode* node = static_cast< TNode* >( block->getNode() );
   if ( !node )
   {
      return;
   }

   if ( canBeRemoved( node ) )
   {
      GraphBuilderTransaction< TNode > transaction( m_graphBuilder );
      transaction.removeNode( *node );
      transaction.commit();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
bool TGraphLayout< TNode >::canBeRemoved( const ReflectionObject* node ) const
{
   for ( List< const SerializableReflectionType* >::const_iterator it = m_nonRemovableNodeTypes.begin(); !it.isEnd(); ++it )
   {
      const SerializableReflectionType* type = *it;
      if ( node->isA( *type ) )
      {
         // it falls under one of the defined non-removable types
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::startNegotiatingConnection( GraphBlockSocket& sourceSocket )
{
   // we can only start connections from the sockets on the right
   ASSERT( sourceSocket.getPosition() == GBSP_OUTPUT );
   
   m_sourceSocket = &sourceSocket;

   // visualize the drawn connection
   ASSERT( m_drawnConnection == NULL);
   m_drawnConnection = new QGraphicsLineItem();
   addItem( m_drawnConnection );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::finishNegotiatingConnection( GraphBlockSocket* destinationSocket )
{
   if ( m_drawnConnection )
   {
      // remove the visualization of the drawn connection
      removeItem( m_drawnConnection );
      m_drawnConnection = NULL;
   }

   if ( m_sourceSocket == NULL || destinationSocket == NULL )
   {
      // there was no source socket memorized to begin with - drop the whole thing
      return;
   }

   // verify that we're trying to connect two distinct sockets that belong to two distinct blocks
   bool belongToTheSameBlock = ( m_sourceSocket->parentItem() == destinationSocket->parentItem() );
   if ( m_sourceSocket == destinationSocket || belongToTheSameBlock )
   {
      return;
   }

   // verify that the connection doesn't exist and that we're connecting to a correctly positioned socket
   if ( destinationSocket->getPosition() == GBSP_INPUT )
   {
      ReflectionObject* sourceNode = m_sourceSocket->getParentBlock()->getNode();
      ReflectionObject* destinationNode = destinationSocket->getParentBlock()->getNode();

      GraphBuilderTransaction< TNode > transaction( m_graphBuilder );
      bool result = transaction.connectNodes( static_cast< TNode* >( sourceNode ), m_sourceSocket->getName(), static_cast< TNode* >( destinationNode ), destinationSocket->getName() );
      transaction.commit();
   }

   // cleanup
   m_sourceSocket = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::disconnectSocket( GraphBlockSocket& socket )
{
   ReflectionObject* node = socket.getParentBlock()->getNode();
   const std::string& socketName = socket.getName();

   // find and remove all connections the socket was involved int
   std::vector< GraphBlockConnection* > connectionsToDelete;

   int count = m_connections.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      GraphBlockConnection* connection = m_connections[i];

      if ( &connection->getSource() == &socket || &connection->getDestination() == &socket )
      {
         connectionsToDelete.push_back( connection );
      }
   }

   // first - run the connections through the higher instance that's aware of the pipeline
   // this graph represents, so that it can clean them up
   GraphBuilderTransaction< TNode > transaction( m_graphBuilder );

   count = connectionsToDelete.size();
   for ( uint i = 0; i < count; ++i )
   {
      GraphBlockConnection* connection = connectionsToDelete[i];

      GraphBlockSocket& sourceSocket = connection->getSource();
      GraphBlockSocket& destSocket = connection->getDestination();
      ReflectionObject* sourceNode = sourceSocket.getParentBlock()->getNode();
      ReflectionObject* destNode = destSocket.getParentBlock()->getNode();
      const std::string& inputName = destSocket.getName();
      transaction.disconnectNodes( static_cast< TNode* >( sourceNode ), static_cast< TNode* >( destNode ), inputName );
   }

   transaction.commit();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::createContextMenu( QMenu* menu, const QPointF& scenePos )
{
   QList< QGraphicsItem* > items = selectedItems();

   if ( items.empty() )
   {
      // empty area was clicked on - no blocks were selected
      createAddNodesMenu( menu, scenePos );
   }
   else if ( items.size() == 1 )
   {
      // a single block was selected
      QGraphicsItem* selectedItem = items[0];

      GraphBlock* block = dynamic_cast< GraphBlock* >( selectedItem );

      for ( List< Callback* >::iterator it = m_callbacks.begin(); !it.isEnd(); ++it )
      {
         Callback* callback = *it;
         if ( callback->canBeExecuted( block ) )
         {
            callback->onCreateContextMenu( this, menu, block );
         }
      }
   }

   // if there are blocks selected, show additional options
   QList< GraphBlock* > selectedBlocks;
   foreach( QGraphicsItem* item, items )
   {
      GraphBlock* block = dynamic_cast< GraphBlock* >( item );
      if ( block )
      {
         selectedBlocks.push_back( block );
      }
   }
   if ( !selectedBlocks.isEmpty() )
   {
      // check if the selected nodes can be removed - the rule is that if only a non-removable node
      // was selected, then we won't be showing the 'Remove' option. Otherwise we'll show it,
      // and the safety mechanism in the removeNode method will prevent the non-removable nodes from getting deleted
      bool canRemoveSelectedNodes = false;
      foreach( GraphBlock* block, selectedBlocks )
      {
         canRemoveSelectedNodes |= canBeRemoved( block->getNode() );
      }

      if ( canRemoveSelectedNodes )
      {
         // an option for removing existing nodes
         QAction* removeNodesAction = new GraphNodeRemoveAction( *this, selectedBlocks );
         menu->addAction( removeNodesAction );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::createAddNodesMenu( QMenu* parentMenu, const QPointF& scenePos )
{
   class AddNodesActionFactory : public MenuActionsFactory
   {
   private:
      GraphLayout*                  m_parentLayout;
      TreeWidgetDescFactory*        m_itemsFactory;
      QPointF                       m_scenePos;

   public:
      AddNodesActionFactory( GraphLayout* parentLayout, TreeWidgetDescFactory* itemsFactory, const QPointF& scenePos )
         : m_parentLayout( parentLayout )
         , m_itemsFactory( itemsFactory )
         , m_scenePos( scenePos )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         const SerializableReflectionType* type = m_itemsFactory->getClass( itemTypeIdx );
         QAction* action = new GraphNodeCreationAction( *m_parentLayout, *type, m_scenePos );
         return action;
      }
   };

   AddNodesActionFactory actionsFactory( this, m_itemDescriptionsFactory, scenePos );
   MenuUtils::itemsListMenu( m_itemDescriptionsFactory, actionsFactory, parentMenu );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::onBlockMoved()
{
   for( std::vector< GraphBlockConnection* >::iterator it = m_connections.begin(); it != m_connections.end(); ++it )
   {
      (*it)->calculateBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::mouseMoveEvent( QGraphicsSceneMouseEvent* mouseEvent )
{
   GraphLayout::mouseMoveEvent( mouseEvent );
   
   updateMousePosition( mouseEvent->scenePos() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::dragMoveEvent( QGraphicsSceneDragDropEvent* event )
{
   GraphLayout::dragMoveEvent( event );

   updateMousePosition( event->scenePos() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::updateMousePosition( const QPointF& pos )
{
   m_currentMousePos = pos;

   if ( m_drawnConnection )
   {
      // update drawn connection
      ASSERT( m_sourceSocket != NULL );

      QPointF start = m_sourceSocket->scenePos();
      m_drawnConnection->setLine( start.rx(), start.ry(), m_currentMousePos.rx(), m_currentMousePos.ry() );
   }

   // expand the items we are currently hovering the mouse cursor over
   {
      // grab all blocks we're currently intersecting ( if we hide them first, some of them might contract, 
      // and if the cursor was hovering above the expanded part - it's not gonna any more, and we don't want that ).
      QList< QGraphicsItem* > itemsToExpand = items( m_currentMousePos, Qt::IntersectsItemShape );

      // determine which blocks to hide...
      for ( List< GraphBlock* >::iterator it = m_expandedBlocks.begin(); !it.isEnd(); ++it )
      {
         GraphBlock* expandedBlock = *it;

         bool visible = false;
         foreach( QGraphicsItem* item, itemsToExpand )
         {
            GraphBlock* candidateBlock = dynamic_cast< GraphBlock* >( item );
            if ( candidateBlock && candidateBlock == expandedBlock )
            {
               // the block should remain expanded
               visible = true;
               break;
            }
         }

         if ( !visible )
         {
            expandedBlock->setExpanded( false );
            it.markForRemoval();
         }
      }

      // ...and which ones to show
      foreach( QGraphicsItem* item, itemsToExpand )
      {
         GraphBlock* candidateBlock = dynamic_cast< GraphBlock* >( item );
         if ( !candidateBlock )
         {
            continue;
         }

         bool visible = false;
         for ( List< GraphBlock* >::iterator it = m_expandedBlocks.begin(); !it.isEnd(); ++it )
         {
            GraphBlock* expandedBlock = *it;
            if ( expandedBlock == candidateBlock )
            {
               // the block is already visible
               visible = true;
            }
         }

         if ( !visible )
         {
            candidateBlock->setExpanded( true );
            m_expandedBlocks.pushBack( candidateBlock );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::keyPressEvent( QKeyEvent* keyEvent )
{
   GraphLayout::keyPressEvent( keyEvent );

   if ( keyEvent->key() == Qt::Key_Delete )
   {
      QList< GraphBlock* > blocks;
      QList< QGraphicsItem* > items = selectedItems();
      foreach( QGraphicsItem* item, items )
      {
         GraphBlock* selectedBlock = dynamic_cast< GraphBlock* >( item );
         if ( selectedBlock )
         {
            blocks.push_back( selectedBlock );
         }
      }

      // delete currently selected blocks
      foreach( GraphBlock* selectedBlock, blocks )
      {
         remove( selectedBlock );
      }
   }
   else if ( keyEvent->matches( QKeySequence::Copy ) )
   {
      // create a copy of the selected part of the graph
      QList< QGraphicsItem* > items = selectedItems();
      List< TNode* > selectedNodes;
      foreach( QGraphicsItem* item, items )
      {
         GraphBlock* selectedBlock = dynamic_cast< GraphBlock* >( item );
         if ( selectedBlock )
         {
            selectedNodes.pushBack( static_cast< TNode* >( selectedBlock->getNode() ) );
         }
      }

      if ( !selectedNodes.empty() )
      {
         Vector worldToLocal;
         worldToLocal.setMul( selectedNodes.front()->getPosition(), Float_Minus1 );

         m_copiedGraph.clear();
         m_copiedGraph.copyRegion( selectedNodes, worldToLocal );
      }
   }
   else if ( keyEvent->matches( QKeySequence::Paste ) )
   {
      if ( !m_copiedNodes.empty() )
      {
         // invalidate previous selection
         clearSelection();

         // we actually have something to paste - so let's
         const Vector localToWorld( m_currentMousePos.x(), m_currentMousePos.y(), 0 );
         m_graphBuilder.copyRegion( m_copiedNodes, localToWorld );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GraphBlock* TGraphLayout< TNode >::getNodeBlock( const TNode& node ) const
{
   uint count = m_blocks.size();
   for ( uint i = 0; i < count; ++i )
   {
      GraphBlock* block = m_blocks[i];
      if ( block->getNode() == & node )
      {
         return block;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GraphBlockConnection* TGraphLayout< TNode >::getConnection( const GraphBlock* endBlock, const char* inputName ) const
{
   uint count = m_connections.size();
   for ( uint i = 0; i < count; ++i )
   {
      GraphBlockConnection* connection = m_connections[i];
      GraphBlockSocket& input = connection->getDestination();
      if ( input.getName() == inputName && input.getParentBlock() == endBlock )
      {
         return connection;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::onNodeCreated( TNode* node )
{
   GraphBlock* block = new TGraphBlock< TNode >( this, *node );
   block->initialize( m_socketsFactory );

   block->setSelected( true );
   m_blocks.push_back( block );
   addItem( block );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::onNodeRemoved( TNode* node )
{
   // Even though some nodes can't be removed, this only affects the user's direct
   // ability to remove a node using a context menu.
   // We still need to be able to remove any node programatically, thus we're not checking
   // the node if it's not removable here


   GraphBlock* block = getNodeBlock( *node );
   
   std::vector< GraphBlock* >::iterator it = std::find( m_blocks.begin(), m_blocks.end(), block );
   if ( it != m_blocks.end() )
   {
      m_blocks.erase( it );
      block->deinitialize();
      removeItem( block );
      delete block;

      List< GraphBlock* >::iterator expandedBlockIt = ListUtils::find( m_expandedBlocks, block );
      if ( !expandedBlockIt.isEnd() )
      {
         expandedBlockIt.markForRemoval();
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::onConnectionCreated( const TNode* startNode, const char* outputName, const TNode* endNode, const char* inputName )
{
   GraphBlock* startBlock = getNodeBlock( *startNode );
   GraphBlock* endBlock = getNodeBlock( *endNode );

   GraphBlockSocket* outputSocket = startBlock->getSocket( GBSP_OUTPUT, outputName );
   GraphBlockSocket* inputSocket = endBlock->getSocket( GBSP_INPUT, inputName );

   GraphBlockConnection* connection = new GraphBlockConnection( outputSocket, inputSocket );
   m_connections.push_back( connection );
   addItem( connection );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphLayout< TNode >::onConnectionRemoved( const TNode* endNode, const char* inputName )
{
   GraphBlock* endBlock = getNodeBlock( *endNode );
   GraphBlockConnection* connection = getConnection( endBlock, inputName );

   std::vector< GraphBlockConnection* >::iterator it = std::find( m_connections.begin(), m_connections.end(), connection );
   if ( it != m_connections.end() )
   {
      m_connections.erase( it );
      removeItem( connection );
      delete connection;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode > template< typename TSpecificNode, typename TSpecificCallback >
void TGraphLayout< TNode >::registerBlockCallback()
{
   m_callbacks.pushBack( new TCallback< TSpecificNode, TSpecificCallback >() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode > template< typename TSpecificNode >
void TGraphLayout< TNode >::defineUnremovableType()
{
   // remove the type from the list of types we can instantiate
   m_itemDescriptionsFactory->removeType< TSpecificNode >();

   // put it on a list of types that can't be removed
   m_nonRemovableNodeTypes.pushBack( &TSpecificNode::getStaticRTTI() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode > template< typename TSpecificNode, typename TSpecificCallback >
bool TGraphLayout< TNode >::TCallback< TSpecificNode, TSpecificCallback >::canBeExecuted( GraphBlock* block ) const
{
   if ( !block )
   {
      return false;
   }

   const SerializableReflectionType& representedNodeType = TSpecificNode::getStaticRTTI();

   TGraphBlock< TNode >* typedBlock = static_cast< TGraphBlock< TNode >* >( block );
   const SerializableReflectionType& checkedNodeType = typedBlock->getNode()->getVirtualRTTI();

   return checkedNodeType.isA( representedNodeType );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode > template< typename TSpecificNode, typename TSpecificCallback >
void TGraphLayout< TNode >::TCallback< TSpecificNode, TSpecificCallback >::onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block ) const
{
   TSpecificCallback::onCreateContextMenu( layout, menu, block );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_LAYOUT_H
