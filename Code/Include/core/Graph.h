/// @file   core\Graph.h
/// @brief  graph representation
#ifndef _GRAPH_H
#define _GRAPH_H

#include "core\Array.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

struct GraphEdge
{
   DECLARE_ALLOCATOR( GraphEdge, AM_ALIGNED_16 );

   int     m_endNodeIdx;
   void*   m_userData;

   GraphEdge( int endNodeIdx )
      : m_endNodeIdx( endNodeIdx )
      , m_userData( NULL )
   {
   }

   GraphEdge( const GraphEdge& rhs )
      : m_endNodeIdx( rhs.m_endNodeIdx )
      , m_userData( rhs.m_userData )
   {
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This is a graph representation. It's a template that allows to use
 * different node and edge types.
 * The only constraint is that an EDGE type had the Graph::Index operator
 * defined that returns an index of the node the edge points to.
 */
template <typename NODE >
class Graph
{
   DECLARE_ALLOCATOR( Graph, AM_ALIGNED_16 );

public:
   enum {
      InvalidIndex = -1
   };

private:
   Array<NODE>                     m_nodes;  // a list of all nodes in the graph

   // a square array - each col  represents a node index.
   // It's guaranteed to have as many columns as there are nodes in the system.
   // Each row in such a column contains an information about an edge that
   // exists from this node. An edge in return contains an information about
   // the node index it connects to at the other end - this way we can go 
   // from one node to another.
   Array< Array< GraphEdge* > >    m_graphRepr;

public:
   /**
    * Constructor.
    */
   Graph();

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Graph( const Graph& rhs );
   virtual ~Graph();

   /**
    * Assignment operator.
    *
    * @param rhs
    */
   void operator=( const Graph& rhs );

   /**
    * Clears the graph.
    */
   void clear();

   /**
    * Adds a new node to the graph.
    *
    * @param node      new node
    * @return          index the node was assigned in the graph.
    */
   int addNode( const NODE& node );

   /**
    * Returns index of the specified node.
    *
    * @param node
    */
   int getNodeIdx( const NODE& node ) const;

   /**
    * The method returns the number of nodes the graph has
    *
    * @return          number of nodes in the graph
    */
   unsigned int getNodesCount() const;

   /**
    * Connects two nodes together.
    *
    * @param startNodeIdx
    * @param endNodeIdx
    * @return          edge was assigned in the graph.
    */
   GraphEdge* connect( int startNodeIdx, int endNodeIdx );

   /**
    * Checks if 2 nodes ( identified by the specified indices ) are connected.
    *
    * @param startNodeIdx
    * @param endNodeIdx
    */
   bool areConnected( int startNodeIdx, int endNodeIdx ) const;

   /**
   * Returns a node  stored in the graph under the specified index.]
   *
   * @param idx       index of a node we want to retrieve
   * @return          a node
   */
   NODE& getNode( int idx );

   /**
    * Returns a node  stored in the graph under the specified index.]
    * (const version)
    *
    * @param idx       index of a node we want to retrieve
    * @return          a node
    */
   const NODE& getNode( int idx ) const;

   /**
    * Looks for the specified node and returns its index.
    *
    * @param node
    * @return index of the node
    */
   int findNodeIdx( const NODE& node ) const;

   /**
    * Returns all edges exiting the node that's registered in the graph
    * under the specified index.
    *
    * @param nodeIdx   index of a node
    * @return          vector containing all edges exiting the specified node
    */
   Array< GraphEdge* >& getEdges( int nodeIdx );

   /**
    * Returns all edges exiting the node that's registered in the graph
    * under the specified index.
    *
    * @param nodeIdx   index of a node
    * @return          vector containing all edges exiting the specified node
    */
   const Array< GraphEdge* >& getEdges( int nodeIdx ) const;

   /**
    * Returns a total number of edges in the graph.
    */
   uint getEdgesCount() const;

   /**
    * Returns indices of nodes that connect TO this node ( this node is the end point to connections
    * that originate in the other nodes ).
    *
    * @param nodeIdx                  index of a node to check
    * @param outConnectedNodeIndices
    */
   void traceIncomingEdges( int nodeIdx, Array<int>& outConnectedNodeIndices ) const;

   /**
    * Returns a number of edges that terminate in the specified node.
    *
    * @param nodeIdx
    */
   uint getIncomingEdgesCount( int nodeIdx ) const;

   /**
    * Returns an index of an edge that connects two nodes
    * indices of which we pass as params.
    *
    * @param startNodeIdx  start node index
    * @param endNodeIdx    end node index
    * @return              graph edge
    */
   GraphEdge* getEdge( int startNodeIdx, int endNodeIdx ) const;

   /**
    * Disconnects all edges exiting the specified node.
    *
    * @param nodeIdx       index of a node we want disconnected
    */
   void disconnect( int nodeIdx );

private:
   void copyFrom( const Graph& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Graph.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_H
