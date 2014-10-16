#ifndef _GRAPH_BUILDER_TRANSACTION_H
#error "This file can only be included from GraphBuilderTransaction.h"
#else

#include <set>


///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
GraphBuilderTransaction< NodeType >::GraphBuilderTransaction( GraphBuilder< NodeType >& graph )
   : m_graph( graph )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
GraphBuilderTransaction< NodeType >::~GraphBuilderTransaction()
{
   // clean remaining data
   uint count = m_connectionsToAdd.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_connectionsToAdd[i];
   }
   m_connectionsToAdd.clear();

   count = m_connectionsToRemove.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_connectionsToRemove[i];
   }
   m_connectionsToRemove.clear();
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::commit()
{
   // apply changes
   {
      // remove connections
      uint count = m_connectionsToRemove.size();
      for ( uint i = 0; i < count; ++i )
      {
         const ConnectionData* data = m_connectionsToRemove[i];
         if ( data->m_inputName.empty() )
         {
            // break all connections between nodes
            m_graph.disconnect( data->m_startNode, data->m_endNode );
         }
         else
         {
            // break only the connection that ends in the specified input socket
            m_graph.disconnect( data->m_endNode, data->m_inputName );
         }
      }

      // remove obsolete nodes
      count = m_nodesToRemove.size();
      for ( uint i = 0; i < count; ++i )
      {
         NodeType* node = m_nodesToRemove[i];

         m_graph.removeNode( *node );
      }

      // add new nodes
      count = m_nodesToAdd.size();
      for ( uint i = 0; i < count; ++i )
      {
         m_graph.addNode( m_nodesToAdd[i] );
      }

      // add connections
      count = m_connectionsToAdd.size();
      for ( uint i = 0; i < count; ++i )
      {
         const ConnectionData* data = m_connectionsToAdd[i];
         m_graph.connect( data->m_startNode, data->m_outputName, data->m_endNode, data->m_inputName );
      }
   }

   // commit implementation specific data
   onCommit();

   // cleanup
   {
      m_nodesToRemove.clear();
      m_nodesToAdd.clear();

      uint count = m_connectionsToAdd.size();
      for ( uint i = 0; i < count; ++i )
      {
         delete m_connectionsToAdd[i];
      }
      m_connectionsToAdd.clear();

      count = m_connectionsToRemove.size();
      for ( uint i = 0; i < count; ++i )
      {
         delete m_connectionsToRemove[i];
      }
      m_connectionsToRemove.clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::addNode( NodeType* node )
{
   putNodeForTransaction( node, m_nodesToAdd );
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::removeNode( NodeType& node )
{
   putNodeForTransaction( &node, m_nodesToRemove );

   // disconnect the node from other nodes
   const std::vector< NodeType* >& nodes = m_graph.getNodes();
   uint nodesCount = nodes.size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      NodeType* otherNode = nodes[i];
      if ( !otherNode || otherNode == &node )
      {
         continue;
      }

      if ( node.isConnectedWith( otherNode ) )
      {
         disconnectNodes( &node, otherNode );
      }
      else if ( otherNode->isConnectedWith( &node ) )
      {
         disconnectNodes( otherNode, &node );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::putNodeForTransaction( NodeType* node, std::vector< NodeType* >& collection ) const
{
   // look for duplicates
   uint count = collection.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( collection[i] == node )
      {
         // this one's already on the list
         return;
      }
   }

   collection.push_back( node );
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
bool GraphBuilderTransaction< NodeType >::connectNodes( NodeType* startNode, const std::string& outputName, NodeType* endNode, const std::string& inputName )
{
   if ( !startNode || !endNode )
   {
      return false;
   }

   TOutputSocket* output = startNode->findOutput( outputName );
   if ( !output )
   {
      return false;
   }

   // looking at a connection from input's perspective, a node is interested in an output 
   // it originates at - it's from where we're gonna get our data
   TInputSocket* input = endNode->findInput( inputName );
   if ( !input )
   {
      return false;
   }

   // verify that the socket types are a match
   const ReflectionType* inputDataType = input->getDataType();
   const ReflectionType* outputDataType = output->getDataType();
   if ( !inputDataType && !outputDataType )
   {
      // if both sockets don't have any specific type associated with them,
      // we assume that they represent a 'void' type and thus can be connected
   }
   else if ( inputDataType && outputDataType )
   {
      if ( !outputDataType->isA( *inputDataType ) )
      {
         return false;
      }
   }
   else
   {
      // one of the types is set, the other is not - connection denied
      return false;
   }

   // check if the input is already connected ( an input can have only one incomming connection )
   if ( input->isConnected() )
   {
      return false;
   }

   m_connectionsToAdd.push_back( new ConnectionData( startNode, outputName, endNode, inputName ) );
   return true;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::disconnectNodes( NodeType* startNode, NodeType* endNode, const std::string& inputName )
{
   if ( !startNode || !endNode )
   {
      return;
   }

   m_connectionsToRemove.push_back( new ConnectionData( startNode, "", endNode, inputName ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::disconnectNodes( NodeType* startNode, NodeType* endNode )
{
   if ( !startNode || !endNode )
   {
      return;
   }

   m_connectionsToRemove.push_back( new ConnectionData( startNode, "", endNode, "" ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void GraphBuilderTransaction< NodeType >::clearGraph()
{
   const uint count = m_graph.m_nodes->size();
   for ( uint i = 0; i < count; ++i )
   {
      removeNode( *(*m_graph.m_nodes)[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_TRANSACTION_H