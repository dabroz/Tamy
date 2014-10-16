/// @file   core/GraphUtils.h
/// @brief  algorithms operating on graphs

#ifndef _GRAPH_UTILS_H
#define _GRAPH_UTILS_H

#include "core\Graph.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

#define GRAPH_CELLCOST_BLOCKED 0xFFFF

///////////////////////////////////////////////////////////////////////////////

template< typename NODE >
struct GraphSearchInfo
{
   typedef int( *EdgeCostFunc )( const typename Graph< NODE >& graph, int startVtx, int endVtx );

   // input
   int                                 m_start;                // index of the start node ( dijkstra, aStar )
   int                                 m_end;                  // index of the end node ( aStar only )
   EdgeCostFunc                        m_costFunc;             // node traversal cost estimator
   EdgeCostFunc                        m_distanceCostFunc;     // distance based cost estimator ( aStar only ) - used to tell how far is the specified cell from the end goal

   // some debug statistics
   uint                                m_numIterations;        // how many iterations did it take to complete the search

   GraphSearchInfo()
      : m_numIterations( 0 )
   {}
};

///////////////////////////////////////////////////////////////////////////////

template< typename NODE >
class GraphUtils
{
public:
   typedef int( *EdgeCostFunc )( const typename Graph< NODE >& graph, int startVtx, int endVtx );

public:
   /**
    * Performs a breadth-first search on the specified graph,
    * starting from the specified node.
    *
    * @param graph     graph we want to run BFS on
    * @param start     index of a node in the graph we want to start
    *                  searching from
    * @param operation     operation we want to perform on the nodes
    */
   template< typename OPERATION >
   static void bfs( const typename Graph< NODE >& graph, int start, OPERATION& operation );


   /**
    * Performs the graph simplification using the Dijkstra algorithm.
    *
    * @param inGraph             graph we want to run algorithm on
    * @param inOutSetting        search params
    * @param outGraph            output graph that upon the function's return
    *                            will contain the simplified version of the input 
    *                            graph
    */
   static void dijkstra( const typename Graph< NODE >& inGraph, typename GraphSearchInfo< NODE >& inOutSetting, typename Graph< NODE >& outGraph );

   /**
    * Seraches the graph for the shortest path between 2 nodes using the A* algorithm.
    *
    *
    * @param inGraph             graph we want to run algorithm on
    * @param inOutSetting        search params
    * @param outGraph            output graph that upon the function's return
    *                            will contain the simplified version of the input
    *                            graph
    * @return was the path found?
    */
   static bool aStar( const typename Graph< NODE >& inGraph, typename GraphSearchInfo< NODE >& inOutSetting, List< NODE >& outPath );

   /**
    * Sorts the graph's nodes in their topological order ( preserving the dependencies ).
    *
    * @param outNodesArr         output array that upon the function's return
    *                            will contain the sorted list of the original graph 
    *                            nodes
    * @param inGraph             graph we want to run algorithm on
    */
   static void topologicalSort( Array< int >& outNodesArr, const typename Graph< NODE >& inGraph );

};

///////////////////////////////////////////////////////////////////////////////

#include "core\GraphUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_UTILS_H
