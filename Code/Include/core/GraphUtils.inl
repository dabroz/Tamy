#ifndef _GRAPH_UTILS_H
#error "This file can only be included from GraphUtils.h"
#else

#include <algorithm>
#include <vector>
#include "core/List.h"
#include "core/Array.h"
#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   template<typename GRAPH>
   class NodesComparator
   {
   private:
      Array<int>& m_nodesCosts;

   public:
      NodesComparator( Array<int>& nodesCosts )
         : m_nodesCosts(nodesCosts)
      {}

      /**
       * We need to sort in the descending order - algorithm requirements.
       */
      bool operator()( int rhs, int lhs ) const
      {
         return m_nodesCosts[rhs] > m_nodesCosts[lhs];
      }
   };

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

template < typename NODE > template< typename OPERATION >
void GraphUtils< NODE >::bfs( const typename Graph< NODE >& graph, int start, OPERATION& operation )
{
   typedef Graph< NODE > GRAPH;

   List<int> nodesQueue;
   nodesQueue.pushBack(start);

   while (nodesQueue.size())
   {
      int currNodeIdx = nodesQueue.front();
      nodesQueue.popFront();

      operation(graph.getNode(currNodeIdx));

      const GRAPH::EdgeIndices& edges = graph.getEdges(currNodeIdx);
      uint count = edges.size();
      for ( uint i = 0; i < count; ++i )
      {
         int edgeIdx = edges[i];
         nodesQueue.pushBack(graph.getEdge(edgeIdx));
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void GraphUtils< NODE >::dijkstra( const typename Graph< NODE >& inGraph, typename GraphSearchInfo< NODE >& inOutSetting, typename Graph< NODE >& outGraph )
{
   if (outGraph.getNodesCount() > 0)
   {
      ASSERT_MSG( false, "Output graph needs to be passed empty");
   }

   // -------------------------------------------------------------------------
   // helper types
   // -------------------------------------------------------------------------
   typedef typename Graph< NODE > GRAPH;
   typedef typename GRAPH::EdgeIndices EdgesList;
   typedef std::vector< int > NodesList;

   // reset statistics
   inOutSetting.m_numIterations = 0;

   // -------------------------------------------------------------------------
   // algorithm
   // -------------------------------------------------------------------------

   Array<int> nodesCosts;
   nodesCosts.resize( inGraph.getNodesCount(), 0xFFFF );
   NodesList nodesParents( inGraph.getNodesCount(), typename GRAPH::InvalidIndex );
   NodesComparator<GRAPH> comparator(nodesCosts);

   NodesList closedList;
   NodesList openList;

   openList.push_back( inOutSetting.m_start );
   nodesCosts[inOutSetting.m_start] = 0;

   while (openList.size())
   {
      // gather statistics
      ++inOutSetting.m_numIterations;

      // find the cheapest node in the open nodes list and 'promote' it
      // to the closed list
      std::sort(openList.begin(), openList.end(), comparator);
      int currNodeIdx = openList.back();
      openList.pop_back();

      closedList.push_back(currNodeIdx);

      // analyze the cheapest node's neighbors
      const EdgesList& adjacentEdges = inGraph.getEdges( currNodeIdx );
      uint count = adjacentEdges.size();
      for ( uint i = 0; i < count; ++i )
      {
         const int edgeIdx = adjacentEdges[i];
         const int adjacentNodeIdx = inGraph.getEdge( edgeIdx );

         // if the node's already in the closed list - discard it
         if ( std::find( closedList.begin(), closedList.end(), adjacentNodeIdx ) != closedList.end() )
         {
            continue;
         }

         const int newCost = nodesCosts[currNodeIdx] + inOutSetting.m_costFunc( inGraph, currNodeIdx, adjacentNodeIdx );

         // if the node's already in the open list, relax it, 
         // and if it's not - add it
         NodesList::iterator openNodeIt = std::find( openList.begin(), openList.end(), adjacentNodeIdx );
         if (openNodeIt != openList.end())
         {
            if (nodesCosts[*openNodeIt] > newCost) 
            {
               nodesCosts[*openNodeIt] = newCost;
               nodesParents[*openNodeIt] = currNodeIdx;
            }
         }
         else
         {
            openList.push_back( adjacentNodeIdx );
            nodesCosts[adjacentNodeIdx] = newCost;
            nodesParents[adjacentNodeIdx] = currNodeIdx;
         }
      }
   }

   // -------------------------------------------------------------------------
   // output graph formulation
   // -------------------------------------------------------------------------
   NodesList indexRemap( inGraph.getNodesCount(), typename GRAPH::InvalidIndex );
   int newNodeIndex = 0;
   for ( NodesList::iterator it = closedList.begin(); it != closedList.end(); ++it, ++newNodeIndex )
   {
      outGraph.addNode( inGraph.getNode( *it ) );
      indexRemap[*it] = newNodeIndex;
   }
   
   unsigned int nodesCount = closedList.size();
   for (unsigned int newEndNodeIdx = 0; newEndNodeIdx < nodesCount; ++newEndNodeIdx)
   {
      int oldEndNodeIdx = closedList[newEndNodeIdx];
      int oldStartNodeIdx = nodesParents[oldEndNodeIdx];
      if (oldStartNodeIdx == typename GRAPH::InvalidIndex) {continue;}

      int newStartNodeIdx = indexRemap[oldStartNodeIdx];
      outGraph.connect( newStartNodeIdx, newEndNodeIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
bool GraphUtils< NODE >::aStar( const typename Graph< NODE >& inGraph, typename GraphSearchInfo< NODE >& inOutSettings, List< NODE >& outPath )
{
   // -------------------------------------------------------------------------
   // helper types
   // -------------------------------------------------------------------------
   typedef typename Graph< NODE > GRAPH;
   typedef typename GRAPH::EdgeIndices EdgesList;
   typedef std::vector< int > NodesList;

   // reset statistics
   inOutSettings.m_numIterations = 0;

   // -------------------------------------------------------------------------
   // algorithm
   // -------------------------------------------------------------------------
   Array<int> nodesCosts;
   nodesCosts.resize( inGraph.getNodesCount(), 0xFFFF );
   NodesList nodesParents( inGraph.getNodesCount(), typename GRAPH::InvalidIndex );
   NodesComparator<GRAPH> comparator( nodesCosts );

   NodesList closedList;
   NodesList openList;

   bool wasPathFound = false;

   // start the search
   openList.push_back( inOutSettings.m_start );
   while ( !openList.empty() )
   {
      // gather some statistics
      ++inOutSettings.m_numIterations;

      // find the cheapest node in the open nodes list and 'promote' it
      // to the closed list
      std::sort( openList.begin(), openList.end(), comparator );
      int currNodeIdx = openList.back();
      openList.pop_back();

      closedList.push_back( currNodeIdx );

      if ( currNodeIdx == inOutSettings.m_end )
      {
         // we reached the end
         wasPathFound = true;
         break;
      }

      // analyze the cheapest node's neighbors
      const EdgesList& adjacentEdges = inGraph.getEdges( currNodeIdx );
      const uint outgoingEdgesCount = adjacentEdges.size();
      for ( uint i = 0; i < outgoingEdgesCount; ++i )
      {
         const int edgeIdx = adjacentEdges[i];
         const int adjacentNodeIdx = inGraph.getEdge( edgeIdx );

         // if the node's already in the closed list - discard it
         if ( std::find( closedList.begin(), closedList.end(), adjacentNodeIdx ) != closedList.end() )
         {
            continue;
         }

         // the cost in this case comprises of:
         //   1. the cost of traversal to from the current node to the adjacent node
         //   2. the cost of traversal from the adjacent node to the goal node ( the closer we get to the node, the lower the costs ) - this will be a distance function
         const int newCost = inOutSettings.m_costFunc( inGraph, currNodeIdx, adjacentNodeIdx ) + inOutSettings.m_distanceCostFunc( inGraph, currNodeIdx, inOutSettings.m_end );

         // if the node's already in the open list, relax it, 
         // and if it's not - add it
         NodesList::iterator openNodeIt = std::find( openList.begin(), openList.end(), adjacentNodeIdx );
         if ( openNodeIt != openList.end() )
         {
            if ( nodesCosts[*openNodeIt] > newCost )
            {
               nodesCosts[*openNodeIt] = newCost;
               nodesParents[*openNodeIt] = currNodeIdx;
            }
         }
         else
         {
            openList.push_back( adjacentNodeIdx );
            nodesCosts[adjacentNodeIdx] = newCost;
            nodesParents[adjacentNodeIdx] = currNodeIdx;
         }
      }
   }

   // now that we've reached the end, plot the path
   if ( wasPathFound == true )
   {
      for ( int currNodeIdx = inOutSettings.m_end; currNodeIdx != inOutSettings.m_start && currNodeIdx != typename GRAPH::InvalidIndex; currNodeIdx = nodesParents[currNodeIdx] )
      {
         outPath.pushBack( inGraph.getNode( currNodeIdx ) );
         currNodeIdx = nodesParents[currNodeIdx];
      }
      outPath.pushBack( inGraph.getNode( inOutSettings.m_start ) );
   }

   return wasPathFound;
}

///////////////////////////////////////////////////////////////////////////////

template < typename NODE >
void GraphUtils< NODE >::topologicalSort( Array< int >& outNodesArr, const typename Graph< NODE >& inGraph )
{
   // -------------------------------------------------------------------------
   // helper types
   // -------------------------------------------------------------------------
   typedef typename Graph< NODE > GRAPH;
   typedef typename GRAPH::EdgeIndices EdgesList;
   typedef Array< int > NodesList;

   // -------------------------------------------------------------------------
   // algorithm
   // -------------------------------------------------------------------------
   GRAPH tempGraph = inGraph;

   // find nodes that don't have any incoming connections, and add them as start nodes 
   List< int > nodesToAnalyze;
   int nodesCount = ( int ) tempGraph.getNodesCount();
   for ( int i = 0; i < nodesCount; ++i )
   {
      EdgesList incomingEdgeIndices;
      tempGraph.getIncomingEdges( i, incomingEdgeIndices );
      if ( incomingEdgeIndices.empty() )
      {
         nodesToAnalyze.pushBack( i );
      }
   }

   uint count;
   while( !nodesToAnalyze.empty() )
   {
      // remove the node from the temporary list of nodes
      int nodeIdx = nodesToAnalyze.front();
      nodesToAnalyze.popFront();

      // if the node's already in the final list - skip it
      uint finalListNodeIdx = outNodesArr.find( nodeIdx );
      if ( finalListNodeIdx != EOA )
      {
         continue;
      }
      // add the node to the final list of nodes
      outNodesArr.push_back( nodeIdx );

      // gather subsequent nodes
      const EdgesList& outEdgeIndices = tempGraph.getEdges( nodeIdx );
      NodesList nextNodes;
      count = outEdgeIndices.size();
      for ( uint i = 0; i < count; ++i )
      {
         int edgeIdx = outEdgeIndices[i];

         int idx = tempGraph.getEdge( edgeIdx );
         nextNodes.push_back( idx );
      }

      // remove the edges from the graph
      tempGraph.disconnect( nodeIdx );

      // analyze the subsequent nodes - and if they don't have any incoming edges,
      // add them to the temporary list
      count = nextNodes.size();
      for ( uint i = 0; i < count; ++i )
      {
         int nextNodeIdx = nextNodes[i];

         EdgesList incomingEdgeIndices;
         tempGraph.getIncomingEdges( nextNodeIdx, incomingEdgeIndices );
         if ( incomingEdgeIndices.empty() )
         {
            nodesToAnalyze.pushBack( nextNodeIdx );
         }
      }
   }

   if ( tempGraph.getEdgesCount() > 0 )
   {
      ASSERT_MSG( false, "The graph is not a DAG" );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_UTILS_H
