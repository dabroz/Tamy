#include "core-TestFramework\TestFramework.h"
#include "core\Graph.h"
#include "core\GraphUtils.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct MockNode 
   {
      int m_distFromStart;

      MockNode( int distFromStart )
         : m_distFromStart( distFromStart )
      {
      }
   };

   typedef Graph<MockNode> MockGraph;

   static int edgeCostFuncMock( const Graph<MockNode>& graph, int start, int end )
   {
      int cost = abs( graph.getNode( end ).m_distFromStart - graph.getNode( start ).m_distFromStart );
      return cost;
   }

   static int edgeCostFuncMock2( const Graph<MockNode>& graph, int start, int end )
   {
      // we'll use the cost based on the index numbers here - just for fun
      int cost = abs( start + end );
      return cost;
   }

}  // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

TEST(GraphDijkstra, simpleGraph)
{
   MockGraph graph;
   graph.addNode(MockNode( 1 ));
   graph.addNode(MockNode( 2 ));
   graph.addNode(MockNode( 3 ));
   graph.addNode(MockNode( 4 ));
   graph.connect(0, 1);
   graph.connect(0, 2);
   graph.connect(2, 1);
   graph.connect(1, 3);

   GraphSearchInfo< MockNode > searchParams;
   searchParams.m_start = 0;
   searchParams.m_costFunc = &edgeCostFuncMock;

   MockGraph result;
   GraphUtils< MockNode >::dijkstra( graph, searchParams, result );
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, result.getEdgesCount());

   CPPUNIT_ASSERT(result.getEdgeIdx(0, 1) != MockGraph::InvalidIndex);
   CPPUNIT_ASSERT(result.getEdgeIdx(0, 2) != MockGraph::InvalidIndex);
   CPPUNIT_ASSERT(result.getEdgeIdx(1, 3) != MockGraph::InvalidIndex);
}

///////////////////////////////////////////////////////////////////////////////

TEST(GraphDijkstra, complex)
{
   MockGraph graph;
   graph.addNode(MockNode( 1 ));
   graph.addNode(MockNode( 2 ));
   graph.addNode(MockNode( 3 ));
   graph.addNode(MockNode( 4 ));
   graph.addNode(MockNode( 5 ));
   graph.addNode(MockNode( 6 ));
   graph.connect(0, 1);
   graph.connect(0, 2);
   graph.connect(2, 1);
   graph.connect(1, 3);
   graph.connect(2, 3);
   graph.connect(2, 5);
   graph.connect(3, 4);
   graph.connect(4, 5);

   GraphSearchInfo< MockNode > searchParams;
   searchParams.m_start = 3;
   searchParams.m_costFunc = &edgeCostFuncMock2;

   MockGraph result;
   GraphUtils< MockNode >::dijkstra( graph, searchParams, result );

   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.getEdgesCount());

   CPPUNIT_ASSERT(result.getEdgeIdx(0, 1) != MockGraph::InvalidIndex);
   CPPUNIT_ASSERT(result.getEdgeIdx(1, 2) != MockGraph::InvalidIndex);

}

///////////////////////////////////////////////////////////////////////////////
