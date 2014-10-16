#include "core-TestFramework\TestFramework.h"
#include "core\Graph.h"
#include "core\GraphUtils.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct MockNode {};

   typedef Graph< MockNode > MockGraph;

}  // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

TEST(GraphTopologicalSort, simpleGraph)
{
   MockGraph graph;
   graph.addNode( MockNode() );
   graph.addNode( MockNode() );
   graph.addNode( MockNode() );
   graph.addNode (MockNode() );
   graph.connect( 0, 1 );
   graph.connect( 0, 2 );
   graph.connect( 2, 1 );
   graph.connect( 1, 3 );

   Array< int > result;
   GraphUtils< MockNode >::topologicalSort( result, graph );

   CPPUNIT_ASSERT_EQUAL( (unsigned int)4, result.size() );
   CPPUNIT_ASSERT_EQUAL( 0, result[0] );
   CPPUNIT_ASSERT_EQUAL( 2, result[1] );
   CPPUNIT_ASSERT_EQUAL( 1, result[2] );
   CPPUNIT_ASSERT_EQUAL( 3, result[3] );
}

///////////////////////////////////////////////////////////////////////////////

TEST(GraphTopologicalSort, complex)
{
   MockGraph graph;
   graph.addNode( MockNode() ); // 0th - start node
   graph.addNode( MockNode() ); // 1st
   graph.addNode( MockNode() ); // 2nd
   graph.addNode( MockNode() ); // 3rd
   graph.addNode( MockNode() ); // 4th
   graph.addNode( MockNode() ); // 5th
   graph.addNode( MockNode() ); // 6th - another start node
   graph.connect( 0, 1 );
   graph.connect( 0, 2 );
   graph.connect( 1, 3 );
   graph.connect( 3, 4 );
   graph.connect( 4, 5 );
   graph.connect( 2, 5 );
   graph.connect( 6, 2 ); // 6th node is connected to the 2 node, but it's a start node
   
   Array< int > result;

   GraphUtils< MockNode >::topologicalSort( result, graph );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)7, result.size() );
   CPPUNIT_ASSERT_EQUAL( 0, result[0] );
   CPPUNIT_ASSERT_EQUAL( 6, result[1] );
   CPPUNIT_ASSERT_EQUAL( 1, result[2] );
   CPPUNIT_ASSERT_EQUAL( 2, result[3] );
   CPPUNIT_ASSERT_EQUAL( 3, result[4] );
   CPPUNIT_ASSERT_EQUAL( 4, result[5] );
   CPPUNIT_ASSERT_EQUAL( 5, result[6] );
}

///////////////////////////////////////////////////////////////////////////////
