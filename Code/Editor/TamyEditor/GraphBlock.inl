#ifndef _GRAPH_BLOCK_H
#error "This file can only be included from GraphBlock.h"
#else

#include "QPropertiesView.h"
#include "GraphBlockSocket.h"


///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
TGraphBlock< TNode >::TGraphBlock( GraphLayout* scene )
   : GraphBlock( scene )
   , m_nodePtr( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
TGraphBlock< TNode >::TGraphBlock( GraphLayout* scene, TNode& node )
   : GraphBlock( scene )
   , m_nodePtr( &node )
{
   // set the properties viewer as the central widget
   QPropertiesView* nodePropertiesRootView = new QPropertiesView();
   m_nodePtr->viewProperties( *nodePropertiesRootView );
   setCentralWidget( nodePropertiesRootView );

   // set the name of the node
   setCaption( node.getName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
TGraphBlock< TNode >::~TGraphBlock()
{
   if ( m_nodePtr )
   {
      m_nodePtr->detachNodeListener( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::initialize( SocketsFactory* socketsFactory )
{
   m_socketsFactory = socketsFactory;

   if ( m_nodePtr )
   {
      m_nodePtr->attachNodeListener( this );
      m_nodePtr->pullStructure( this );

      // initialize block's position
      const Vector& pos = m_nodePtr->getPosition();

      QPointF blockPos( pos.getComponent(0).getFloat(), pos.getComponent(1).getFloat() );
      setPos( blockPos );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
ReflectionObject* TGraphBlock< TNode >::getNode() 
{ 
   return m_nodePtr;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::updateNodeName()
{
   m_nodePtr->setName( m_caption.toStdString().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::onNodeAppearance( TNode* node )
{
   const Vector& pos = node->getPosition();

   QPointF blockPos( pos.getComponent(0).getFloat(), pos.getComponent(1).getFloat() );
   setPos( blockPos );

   // set the name of the node
   setCaption( node->getName().c_str() );

   static_cast< GraphLayout* >( scene() )->onBlockMoved();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::onInputAdded( TNode* node, GBNodeInput< TNode >* input )
{
   ASSERT( m_socketsFactory != NULL );

   GraphBlockSocket* socket = m_socketsFactory->create( *input );
   if ( socket )
   {
      socket->initialize( this, GBSP_INPUT, input->getName().c_str() );
      addSocket( socket );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::onOutputAdded( TNode* node, GBNodeOutput< TNode >* output )
{
   ASSERT( m_socketsFactory != NULL );

   GraphBlockSocket* socket = m_socketsFactory->create( *output );
   if ( socket )
   {
      socket->initialize( this, GBSP_OUTPUT, output->getName().c_str() );
      addSocket( socket );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::onInputRemoved( TNode* node, GBNodeInput< TNode >* input )
{
   removeSocket( GBSP_INPUT, input->getName() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::onOutputRemoved( TNode* node, GBNodeOutput< TNode >* output )
{
   removeSocket( GBSP_OUTPUT, output->getName() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void TGraphBlock< TNode >::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
   GraphBlock::mouseMoveEvent( event );

   if ( m_nodePtr )
   {
      QPointF blockPos = pos();
      Vector nodePos( blockPos.x(), blockPos.y(), 0 );
      m_nodePtr->setPosition( nodePos );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BLOCK_H
