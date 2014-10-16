/// @file   core\Graph.h
/// @brief  graph representation
#ifndef _GRAPH_H
#define _GRAPH_H

#include "core\Array.h"
#include "core\MemoryRouter.h"


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
    enum {InvalidIndex = -1};

    typedef typename Array<int> EdgeIndices;

private:
    Array<NODE>                     m_nodes;  // a list of all nodes in the graph
    Array<int>                    m_edges;  // a list of all edges in the graph

    EdgeIndices                     m_edgesMapping;
    EdgeIndices                     m_freeEdgeMapings;

    // a square array - each col  represents a node index.
    // It's guaranteed to have as many columns as there are nodes in the system.
    // Each row in such a column contains an information about an edge that
    // exists from this node. An edge in return contains an information about
    // the node index it connects to at the other end - this way we can go 
    // from one node to another.
    Array< EdgeIndices >         m_graphRepr;

public:
    virtual ~Graph() {}

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
     * @return          index the edge was assigned in the graph.
     */
    int connect( int startNodeIdx, int endNodeIdx );

    /**
     * Checks if 2 nodes ( identified by the specified indices ) are connected.
     *
     * @param startNodeIdx
     * @param endNodeIdx
     */
    bool areConnected( int startNodeIdx, int endNodeIdx ) const;

    /**
     * The method returns the number of edges the graph has
     *
     * @return          number of edges in the graph
     */
    uint getEdgesCount() const;

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
    * Returns an index of a triangle the specified edge leads to.
    *
    * @param edgeIdx       index of an edge we want to retrieve
    * @return          an edge
    */
    int getEdge( int edgeIdx ) const;

    /**
     * Returns all edges exiting the node that's registered in the graph
     * under the specified index.
     *
     * @param nodeIdx   index of a node
     * @return          vector containing all edges exiting the specified node
     */
    EdgeIndices& getEdges( int nodeIdx );

    /**
     * Returns all edges exiting the node that's registered in the graph
     * under the specified index.
     *
     * @param nodeIdx   index of a node
     * @return          vector containing all edges exiting the specified node
     */
    const EdgeIndices& getEdges( int nodeIdx ) const;

    /**
     * Returns all edges entering the node that's registered in the graph
     * under the specified index.
     *
     * @param nodeIdx   index of a node
     * @param outEdges  vector containing all edges entering the specified node
     */
    void getIncomingEdges( int nodeIdx, typename Graph<NODE>::EdgeIndices& outEdges ) const;

    /**
     * Returns an index of an edge that connects two nodes
     * indices of which we pass as params.
     *
     * @param startNodeIdx  start node index
     * @param endNodeIdx    end node index
     * @return              index of an edge or -1, if there's no edge
     *                      connecting the two nodes
    */
    int getEdgeIdx( int startNodeIdx, int endNodeIdx ) const;

    /**
     * Disconnects all edges exiting the specified node.
     *
     * @param nodeIdx       index of a node we want disconnected
     */
    void disconnect( int nodeIdx );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Graph.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_H
