#ifndef _GRAPH_BUILDER_H
#error "This file can only be included from GraphBuilder.h"
#else

#include "core\GraphBuilderNode.h"
#include <algorithm>
#include "core\Algorithms.h"
#include "core\LocalList.h"
#include "core\ListUtils.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
GraphBuilder< NodeType >::GraphBuilder()
   : m_nodes( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
GraphBuilder< NodeType >::~GraphBuilder()
{
   // reset nodes' host graph
   if ( m_nodes )
   {
      uint count = m_nodes->size();
      for ( uint i = 0; i < count; ++i )
      {
         NodeType* node = (*m_nodes)[i];
         node->setHost( NULL );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::clear()
{
   if ( m_nodes )
   {
      // remove all nodes
      uint count = m_nodes->size();
      for ( uint i = 0; i < count; ++i )
      {
         NodeType* node = (*m_nodes)[i];
         node->setHost( NULL );
         node->removeReference();
      }
      m_nodes->clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::attachGraphListener( GraphBuilderListener< NodeType >* listener )
{
   // check for duplicates
   ListenersList::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      return;
   }

   m_listeners.pushBack( listener );
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::detachGraphListener( GraphBuilderListener< NodeType >* listener )
{
   ListenersList::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::pullGraphStructure( GraphBuilderListener< NodeType >* listener )
{
   std::map< GBNodeOutput< NodeType >*, NodeType* > outputsOwnersMap;

   // notify about the nodes
   uint nodesCount = m_nodes->size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* node = (*m_nodes)[i];
      if ( !node )
      {
         continue;
      }

      listener->onNodeCreated( node );

      // map the outputs of the node
      const std::vector< GBNodeOutput< NodeType >* >& outputs = node->getOutputs();

      uint outputsCount = outputs.size();
      for ( uint j = 0; j < outputsCount; ++j )
      {
         GBNodeOutput< NodeType >* output = outputs[j];
         outputsOwnersMap.insert( std::make_pair( output, node ) );
      }
   }

   // notify about the connections
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* inputOwner = (*m_nodes)[i];
      if ( !inputOwner )
      {
         continue;
      }

      const std::vector< GBNodeInput< NodeType >* >& inputs = inputOwner->getInputs();

      uint inputsCount = inputs.size();
      for ( uint j = 0; j < inputsCount; ++j )
      {
         GBNodeInput< NodeType >* input = inputs[j];
         GBNodeOutput< NodeType >* connectedOutput = input->getOutput();
         if ( !connectedOutput )
         {
            continue;
         }

         // get the output owner
         NodeType* outputOwner = outputsOwnersMap[connectedOutput];
         listener->onConnectionCreated( outputOwner, connectedOutput->getName().c_str(), inputOwner, input->getName().c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::removeObsoleteSockets()
{
   std::map< GBNodeOutput< NodeType >*, NodeType* > outputsOwnersMap;

   // notify about the nodes
   uint nodesCount = m_nodes->size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* node = (*m_nodes)[i];
      if ( !node )
      {
         continue;
      }

      // map the outputs of the node
      const std::vector< GBNodeOutput< NodeType >* >& outputs = node->getOutputs();

      uint outputsCount = outputs.size();
      for ( uint j = 0; j < outputsCount; ++j )
      {
         GBNodeOutput< NodeType >* output = outputs[j];
         outputsOwnersMap.insert( std::make_pair( output, node ) );
      }
   }

   // notify about the connections
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* inputOwner = (*m_nodes)[i];
      if ( !inputOwner )
      {
         continue;
      }

      const std::vector< GBNodeInput< NodeType >* >& inputs = inputOwner->getInputs();

      uint inputsCount = inputs.size();
      for ( uint j = 0; j < inputsCount; ++j )
      {
         GBNodeInput< NodeType >* input = inputs[j];
         GBNodeOutput< NodeType >* connectedOutput = input->getOutput();
         if ( !connectedOutput )
         {
            continue;
         }

         // get the output owner
         NodeType* outputOwner = outputsOwnersMap[connectedOutput];
         if ( !outputOwner )
         {
            connectedOutput->removeReference();
            input->disconnect();
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::setGraphBuilderNodes( std::vector< NodeType* >* nodes )
{
   // reset old nodes' host graph
   if ( m_nodes )
   {
      uint count = m_nodes->size();
      for ( uint i = 0; i < count; ++i )
      {
         NodeType* node = (*m_nodes)[i];
         if ( node )
         {
            node->setHost( NULL );
         }
      }
   }

   m_nodes = nodes;

   // parent new nodes
   if ( m_nodes )
   {
      uint count = m_nodes->size();
      for ( uint i = 0; i < count; ++i )
      {
         NodeType* node = (*m_nodes)[i];
         if ( node )
         {
            node->setHost( this );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::addNode( NodeType* node )
{
   node->setHost( this );
   m_nodes->push_back( node );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->onNodeCreated( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::removeNode( NodeType& node )
{
   std::vector< NodeType* >::iterator it = std::find( m_nodes->begin(), m_nodes->end(), &node );
   if ( it == m_nodes->end() )
   {
      ASSERT_MSG( false, "Trying to remove a non-existing graph node" );
      return;
   }

   node.setHost( NULL );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->onNodeRemoved( &node );
   }
   
   // remove the node from the graph
   m_nodes->erase( it );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->afterNodeRemoved( &node );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::connect( NodeType* startNode, const std::string& outputName, NodeType* destNode, const std::string& inputName )
{
   // looking at a connection from input's perspective, a node is interested in a node 
   // the connection connects this node to - it's a node that's gonna get 
   // activated after this node's update is finished
   GBNodeOutput< NodeType >* output = startNode->findOutput( outputName );
   ASSERT_MSG( output, "Output socket not found" );

   // looking at a connection from input's perspective, a node is interested in an output 
   // it originates at - it's from where we're gonna get our data
   GBNodeInput< NodeType >* input = destNode->findInput( inputName );
   ASSERT_MSG( input, "Input socket not found" );

   // check if the nodes are not already connected
   if ( input->getOutput() == output )
   {
      // the nodes are already connected
      return;
   }

   output->connect( *destNode );
   input->connect( *output );

   // inform the listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->onConnectionCreated( startNode, outputName.c_str(), destNode, inputName.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::disconnect( NodeType* destNode, const std::string& inputName )
{
   // disconnect the other node's input
   GBNodeInput< NodeType >* input = destNode->findInput( inputName );
   if ( !input )
   {
      // input could have been removed - and since there's nothing to do here, bail
      return;
   }

   // notify the listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->onConnectionRemoved( destNode, inputName.c_str() );
   }

   // find the connected output
   GBNodeOutput< NodeType >* output = input->getOutput();

   // disconnect the input
   input->disconnect();

   // disconnect the output
   if ( output )
   {
      output->disconnect( *destNode );
   }

   // notify the listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->afterConnectionRemoved( destNode, inputName.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::disconnect( NodeType* startNode, NodeType* destNode )
{
   const std::vector< GBNodeOutput< NodeType >* >& outputs = startNode->getOutputs();

   uint outputsCount = outputs.size();
   for ( uint i = 0; i < outputsCount; ++i )
   {
      GBNodeOutput< NodeType >* output = outputs[i];
      if ( output )
      {
         output->disconnect( *destNode );
      }
   }

   // collect all connected inputs by filtering out the ones that are not connected
   // to startNode
   const std::vector< GBNodeInput< NodeType >* >& inputs = destNode->getInputs();
   int inputsCount = inputs.size();
   for ( int i = inputsCount - 1; i >= 0; --i )
   {
      GBNodeInput< NodeType >* input = inputs[i];

      for ( uint j = 0; j < outputsCount; ++j )
      {
         GBNodeOutput< NodeType >* output = outputs[j];
         if ( output && input->getOutput() == output )
         {
            // notify the listeners
            for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
            {
               GraphBuilderListener< NodeType >* listener = *it;
               listener->onConnectionRemoved( destNode, input->getName().c_str() );
            }

            // found a connection - disconnect it
            input->disconnect();
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::buildGraph( Graph< NodeType* >& outGraph ) const
{
   // fill the list with nodes that don't have any predecessors
   LocalList< NodeType* > nodesToAnalyze;
   uint nodesCount = m_nodes->size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* checkedNode = (*m_nodes)[i];
      outGraph.addNode( checkedNode );

      bool hasPredecessors = false;
      for ( uint j = 0; j < nodesCount; ++j )
      {
         NodeType* node = (*m_nodes)[j];
         if ( node->isConnectedWith( checkedNode ) )
         {
            hasPredecessors = true;
            break;
         }
      }

      if ( !hasPredecessors )
      {
         nodesToAnalyze.pushBack( checkedNode );
      }
   }

   if ( nodesToAnalyze.empty() )
   {
      return;
   }

   // go through the nodes and add them to the graph
   std::set< NodeType* > analyzedNodes;
   while( !nodesToAnalyze.empty() )
   {
      NodeType* checkedNode = nodesToAnalyze.front();
      nodesToAnalyze.popFront();

      // make sure the node's not already in the cache - we don't allow the same node to be analyzed twice
      // with this algorithm
      std::set< NodeType* >::const_iterator analyzedNodeIt = analyzedNodes.find( checkedNode );
      if ( analyzedNodeIt != analyzedNodes.end() )
      {
         continue;
      }
      analyzedNodes.insert( checkedNode );


      int nodeIdx = outGraph.getNodeIdx( checkedNode );

      std::vector< NodeType* > successors;
      checkedNode->getSubsequentNodes( successors );
      for ( std::vector< NodeType* >::const_iterator it = successors.begin(); it != successors.end(); ++it )
      {
         NodeType* successorNode = *it;
         if ( !successorNode )
         {
            continue;
         }
         int successorNodeIdx = outGraph.getNodeIdx( successorNode );

         // create a connection in the graph
         outGraph.connect( nodeIdx, successorNodeIdx );

         // put the successor up for analysis
         nodesToAnalyze.pushBack( successorNode );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::getPredecessors( NodeType* checkedNode, std::vector< NodeType* >& outPredecessors ) const
{
   uint count = m_nodes->size();
   for ( uint i = 0; i < count; ++i )
   {
      NodeType* node = (*m_nodes)[i];
      if ( node->isConnectedWith( checkedNode ) )
      {
         outPredecessors.push_back( node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::notifyConnectionBroken( const NodeType* endNode, const char* inputName ) const
{
   // notify listeners
   for ( ListenersList::const_iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderListener< NodeType >* listener = *it;
      listener->onConnectionRemoved( endNode, inputName );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
struct NodePair
{
   DECLARE_ALLOCATOR( NodePair, AM_DEFAULT );

   const NodeType*   m_original;
   NodeType*         m_cloned;

   NodePair( const NodeType* original, NodeType* cloned )
      : m_original( original )
      , m_cloned( cloned )
   {
   }

   static bool isEqual( const NodePair& rhs, const NodeType* const& lhs )
   {
      return rhs.m_original == lhs;
   }
};

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::copyRegion( const List< NodeType* >& nodesInRegion, const Vector& offset )
{
   ASSERT( m_nodes != NULL );

   // locate and clone the nodes 
   GraphBuilderTransaction< NodeType > transaction( *this );

   // add nodes to the graph
   List< NodePair< NodeType > > clonedNodes;
   Vector newNodePos;
   for ( List< NodeType* >::const_iterator it = nodesInRegion.begin(); !it.isEnd(); ++it )
   {
      NodeType* node = *it;

      const SerializableReflectionType& type = node->getVirtualRTTI();
      NodeType* nodeCopy = type.instantiate( *node );
      
      // update node's position
      newNodePos.setAdd( nodeCopy->getPosition(), offset );
      nodeCopy->setPosition( newNodePos );

      clonedNodes.pushBack( NodePair< NodeType >( node, nodeCopy ) );
      transaction.addNode( nodeCopy );
   }

   // add connections to any nodes that are also a part of the region
   for ( List< NodePair< NodeType > >::iterator it = clonedNodes.begin(); !it.isEnd(); ++it )
   {
      const NodePair< NodeType >& destNodePair = *it;
      const NodeType* originalNode = destNodePair.m_original;

      const std::vector< GBNodeInput< NodeType >* >& inputs = originalNode->getInputs();
      uint count = inputs.size();
      for ( uint i = 0; i < count; ++i )
      {
         GBNodeInput< NodeType >* input = inputs[i];
         GBNodeOutput< NodeType >* connectedOutput = input->getOutput();
         if ( !connectedOutput )
         {
            continue;
         }

         const NodeType* connectedNode = getParentNode( nodesInRegion, connectedOutput );
         
         List< NodePair< NodeType > >::iterator existingNodeIt = ListUtils::find< NodePair< NodeType >, const NodeType*, NodePair< NodeType > >( clonedNodes, connectedNode );
         if( !existingNodeIt.isEnd() )
         {
            NodePair< NodeType >& srcNodePair = *existingNodeIt;

            // build a connection
            transaction.connectNodes( srcNodePair.m_cloned, connectedOutput->getName(), destNodePair.m_cloned, input->getName() );
         }
      }
   }

   transaction.commit();
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilder< NodeType >::copyRegion( const std::vector< NodeType* >& nodesInRegion, const Vector& offset )
{
   ASSERT( m_nodes != NULL );

   // locate and clone the nodes 
   GraphBuilderTransaction< NodeType > transaction( *this );

   // add nodes to the graph
   List< NodePair< NodeType > > clonedNodes;
   Vector newNodePos;
   uint nodesCount = nodesInRegion.size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* node = nodesInRegion[i];
      if ( !node )
      {
         continue;
      }

      const SerializableReflectionType& type = node->getVirtualRTTI();
      NodeType* nodeCopy = type.instantiate( *node );

      // update node's position
      newNodePos.setAdd( nodeCopy->getPosition(), offset );
      nodeCopy->setPosition( newNodePos );

      clonedNodes.pushBack( NodePair< NodeType >( node, nodeCopy ) );
      transaction.addNode( nodeCopy );
   }

   // add connections to any nodes that are also a part of the region
   for ( List< NodePair< NodeType > >::iterator it = clonedNodes.begin(); !it.isEnd(); ++it )
   {
      const NodePair< NodeType >& destNodePair = *it;
      const NodeType* originalNode = destNodePair.m_original;

      const std::vector< GBNodeInput< NodeType >* >& inputs = originalNode->getInputs();
      uint count = inputs.size();
      for ( uint i = 0; i < count; ++i )
      {
         GBNodeInput< NodeType >* input = inputs[i];
         GBNodeOutput< NodeType >* connectedOutput = input->getOutput();
         if ( !connectedOutput )
         {
            continue;
         }
         const NodeType* connectedNode = getParentNode( nodesInRegion, connectedOutput );

         List< NodePair< NodeType > >::iterator existingNodeIt = ListUtils::find< NodePair< NodeType >, const NodeType*, NodePair< NodeType > >( clonedNodes, connectedNode );
         if( !existingNodeIt.isEnd() )
         {
            NodePair< NodeType >& srcNodePair = *existingNodeIt;

            // build a connection
            transaction.connectNodes( srcNodePair.m_cloned, connectedOutput->getName(), destNodePair.m_cloned, input->getName() );
         }
      }
   }

   transaction.commit();
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
NodeType* GraphBuilder< NodeType >::getParentNode( const std::vector< NodeType* >& nodes, GBNodeOutput< NodeType >* output )
{
   uint count = nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      NodeType* node = nodes[i];
      GBNodeOutput< NodeType >* foundOutput = node->findOutput( output->getName() );
      if ( foundOutput == output )
      {
         return node;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
NodeType* GraphBuilder< NodeType >::getParentNode( const List< NodeType* >& nodes, GBNodeOutput< NodeType >* output )
{
   for ( List< NodeType* >::const_iterator it = nodes.begin(); !it.isEnd(); ++it )
   {
      NodeType* node = *it;
      GBNodeOutput< NodeType >* foundOutput = node->findOutput( output->getName() );
      if ( foundOutput == output )
      {
         return node;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType > template< typename TSpecificNode >
TSpecificNode* GraphBuilder< NodeType >::findNodeByType()
{
   const uint count = m_nodes->size();
   for ( uint i = 0; i < count; ++i )
   {
      NodeType* node = ( *m_nodes )[i];
      if ( node->isA< TSpecificNode >() )
      {
         return static_cast< TSpecificNode* >( node );
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_H
