#ifndef _GRAPH_H
#error "This file can only be included from Graph.h"
#else

#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph<NODE>::clear( )
{
   m_nodes.clear();
   m_edges.clear( );
   m_edgesMapping.clear( );
   m_freeEdgeMapings.clear( );
   m_graphRepr.clear();
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
int Graph<NODE>::addNode( const NODE& node )
{
   int idx = m_nodes.size();
   m_nodes.push_back(node);
   m_graphRepr.push_back(EdgeIndices());

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
int Graph<NODE >::connect( int startNodeIdx, int endNodeIdx )
{
   int idx = m_edges.size();
   m_edges.push_back( endNodeIdx );

   int edgeIdx = InvalidIndex;
   if ( m_freeEdgeMapings.empty() )
   {
      edgeIdx = m_edgesMapping.size();
      m_edgesMapping.push_back( idx );
   }
   else
   {
      edgeIdx = m_freeEdgeMapings.back();
      m_freeEdgeMapings.remove( m_freeEdgeMapings.size() - 1 );
      m_edgesMapping[ edgeIdx ] = idx;
   }

   m_graphRepr[startNodeIdx].push_back( edgeIdx );

   return idx;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
bool Graph<NODE >::areConnected( int startNodeIdx, int endNodeIdx ) const
{
   typename const Graph<NODE >::EdgeIndices& edgeIndices = m_graphRepr[startNodeIdx];
   const uint count = edgeIndices.size();
   for ( uint i = 0; i < count; ++i )
   {
      int endIdx = m_edges[edgeIndices[i]];

      if ( endIdx == endNodeIdx )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
unsigned int Graph<NODE >::getEdgesCount() const
{
   return m_edgesMapping.size() - m_freeEdgeMapings.size();
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
int Graph< NODE >::getEdge( int edgeIdx ) const
{
   if ( m_edgesMapping[edgeIdx] != InvalidIndex )
   {
      return m_edges[m_edgesMapping[edgeIdx]];
   }
   else
   {
      ASSERT_MSG( false, "Trying to access a non-existing edge" );
      return InvalidIndex;
   }
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
typename Graph<NODE >::EdgeIndices& Graph<NODE >::getEdges( int nodeIdx )
{
   return m_graphRepr[nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
const typename Graph<NODE >::EdgeIndices& Graph<NODE >::getEdges( int nodeIdx ) const
{
   return m_graphRepr[nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph< NODE >::getIncomingEdges( int nodeIdx, typename Graph< NODE >::EdgeIndices& outEdges ) const
{
   uint count = m_edgesMapping.size();
   for ( uint i = 0; i < count; ++i )
   {
      const int& edgeIdx = m_edgesMapping[i];
      if ( edgeIdx != InvalidIndex && ( int ) ( m_edges[edgeIdx] ) == nodeIdx )
      {
         outEdges.push_back( i );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
int Graph<NODE>::getEdgeIdx( int startNodeIdx, int endNodeIdx ) const
{
   const EdgeIndices& edges = m_graphRepr[startNodeIdx];
   uint count = edges.size();
   for ( uint i = 0; i < count; ++i )
   {
      int edgeIdx = edges[i];
      if ( ( int ) ( getEdge( edgeIdx ) ) == endNodeIdx )
      {
         return edgeIdx;
      }
   }

   return -1;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void Graph< NODE >::disconnect( int nodeIdx )
{
   EdgeIndices& edgeIndices = m_graphRepr[nodeIdx];

   uint count = edgeIndices.size();
   for ( uint i = 0; i < count; ++i )
   {
      int edgeIdx = edgeIndices[i];

      m_edgesMapping[ edgeIdx ] = InvalidIndex;
      m_freeEdgeMapings.push_back( edgeIdx );
   }
   m_graphRepr[nodeIdx].clear();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_H
