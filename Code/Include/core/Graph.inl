#ifndef _GRAPH_H
#error "This file can only be included from Graph.h"
#else

#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
Graph<NODE>::Graph()
{
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
Graph<NODE>::Graph( const Graph& rhs )
{
   copyFrom( rhs );
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
Graph<NODE>::~Graph()
{
   clear();
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph<NODE>::operator=( const Graph& rhs )
{
   clear();
   copyFrom( rhs );
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph<NODE>::copyFrom( const Graph& rhs )
{
   m_nodes = rhs.m_nodes;

   const uint nodesCount = rhs.m_graphRepr.size();
   m_graphRepr.resize( nodesCount, Array< GraphEdge*>() );
   for ( uint i = 0; i < nodesCount; ++i )
   {
      const Array< GraphEdge* >& rhsEdges = rhs.m_graphRepr[i];
      Array< GraphEdge* >& lhsEdges = m_graphRepr[i];

      const uint edgesCount = rhsEdges.size();
      lhsEdges.resize( edgesCount, NULL );
      for ( uint j = 0; j < edgesCount; ++j )
      {
         lhsEdges[j] = new GraphEdge( *rhsEdges[j] );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph<NODE>::clear( )
{
   const uint nodesCount = m_graphRepr.size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      Array< GraphEdge* >& egdes = m_graphRepr[i];
      const uint edgesCount = egdes.size();
      for ( uint j = 0; j < edgesCount; ++j )
      {
         delete egdes[j];
      }
   }

   m_nodes.clear();
   m_graphRepr.clear();
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
int Graph<NODE>::addNode( const NODE& node )
{
   int idx = m_nodes.size();
   m_nodes.push_back(node);
   m_graphRepr.push_back( Array< GraphEdge* >() );

   return idx;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
int Graph< NODE >::getNodeIdx( const NODE& node ) const
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      const NODE& checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         return i;
      }
   }

   return InvalidIndex;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
unsigned int Graph< NODE >::getNodesCount() const
{
   return m_nodes.size();
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
GraphEdge* Graph<NODE >::connect( int startNodeIdx, int endNodeIdx )
{
   GraphEdge* edge = new GraphEdge( endNodeIdx );
   m_graphRepr[startNodeIdx].push_back( edge );

   return edge;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
bool Graph<NODE >::areConnected( int startNodeIdx, int endNodeIdx ) const
{
   const Array< GraphEdge* >& edges = m_graphRepr[startNodeIdx];
   const uint count = edges.size();
   for ( uint i = 0; i < count; ++i )
   {
      int endIdx = edges[i]->m_endNodeIdx;

      if ( endIdx == endNodeIdx )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
NODE& Graph<NODE >::getNode( int idx )
{
   return m_nodes[idx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
const NODE& Graph<NODE >::getNode( int idx ) const
{
   return m_nodes[idx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
int Graph< NODE >::findNodeIdx( const NODE& node ) const
{
   const uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_nodes[i] == node )
      {
         return i;
      }
   }

   // the node is not a part of this graph
   return InvalidIndex;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
Array< GraphEdge* >& Graph<NODE >::getEdges( int nodeIdx )
{
   return m_graphRepr[nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
const Array< GraphEdge* >& Graph<NODE >::getEdges( int nodeIdx ) const
{
   return m_graphRepr[nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
uint Graph<NODE >::getEdgesCount() const
{
   uint totalNumEdges = 0;

   const uint nodesCount = m_graphRepr.size();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      const Array< GraphEdge* >& egdes = m_graphRepr[i];
      totalNumEdges += egdes.size();
   }

   return totalNumEdges;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
uint Graph< NODE >::getIncomingEdgesCount( int endNodeIdx ) const
{
   uint incomingEdgesCount = 0;
   const uint count = m_graphRepr.size();
   for ( uint startNodeIdx = 0; startNodeIdx < count; ++startNodeIdx )
   {
      GraphEdge* edge = getEdge( startNodeIdx, endNodeIdx );
      if ( edge != NULL )
      {
         ++incomingEdgesCount;
      }
   }

   return incomingEdgesCount;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph< NODE >::traceIncomingEdges( int endNodeIdx, Array<int>& outConnectedNodeIndices ) const
{
   const uint count = m_graphRepr.size();
   for ( uint startNodeIdx = 0; startNodeIdx < count; ++startNodeIdx )
   {
      GraphEdge* edge = getEdge( startNodeIdx, endNodeIdx );
      if ( edge != NULL )
      {
         outConnectedNodeIndices.push_back( startNodeIdx );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
GraphEdge* Graph<NODE>::getEdge( int startNodeIdx, int endNodeIdx ) const
{
   const Array< GraphEdge* >& edges = m_graphRepr[startNodeIdx];
   const uint count = edges.size();
   for ( uint i = 0; i < count; ++i )
   {
      GraphEdge* edge = edges[i];
      if ( edge->m_endNodeIdx == endNodeIdx )
      {
         return edge;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph< NODE >::disconnect( int nodeIdx )
{
   Array< GraphEdge* >& removedEdges = m_graphRepr[nodeIdx];

   const uint removedIndicesCount = removedEdges.size();
   for ( uint i = 0; i < removedIndicesCount; ++i )
   {
      delete removedEdges[i];
   }

   removedEdges.clear();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_H
