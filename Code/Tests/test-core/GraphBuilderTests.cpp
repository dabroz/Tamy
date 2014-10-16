#include "core-TestFramework\TestFramework.h"
#include "core\ReflectionObject.h"
#include "core\Graph.h"
#include "core\GraphBuilder.h"
#include "core\GraphBuilderTransaction.h"
#include "core\GraphUtils.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class GBMockNode;
   class MockOutput : public GBNodeOutput< GBMockNode >
   {
      DECLARE_CLASS()

   public:
      MockOutput() : GBNodeOutput< GBMockNode >( "Output" ) {}
   };
   BEGIN_OBJECT( MockOutput );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class MockInput : public GBNodeInput< GBMockNode >
   {
      DECLARE_CLASS()

   public:
      MockInput() : GBNodeInput< GBMockNode >( "Input" ) {}
   };
   BEGIN_OBJECT( MockInput );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class GBMockNode : public GraphBuilderNode< GBMockNode >
   {
      DECLARE_ALLOCATOR( GBMockNode, AM_DEFAULT );
      DECLARE_CLASS();

   public:
      int         m_idx;

   public:
      GBMockNode( int idx ) : m_idx( idx ) {}
      virtual ~GBMockNode() {}
   };
   BEGIN_ABSTRACT_OBJECT( GBMockNode );
      PARENT( ReflectionObject );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class GBMStartNode : public GBMockNode
   {
      DECLARE_ALLOCATOR( GBMStartNode, AM_DEFAULT );
      DECLARE_CLASS();

   public:
      GBMStartNode( int idx ) : GBMockNode( idx ) 
      {
         std::vector< GBNodeOutput< GBMockNode >* > outputs;
         outputs.push_back( new MockOutput() );

         defineOutputs( outputs );
      }
   };
   BEGIN_ABSTRACT_OBJECT( GBMStartNode );
      PARENT( GBMockNode );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class GBMEndNode : public GBMockNode
   {
      DECLARE_ALLOCATOR( GBMEndNode, AM_DEFAULT );
      DECLARE_CLASS();

   public:
      GBMEndNode( int idx ) : GBMockNode( idx ) 
      {
         std::vector< GBNodeInput< GBMockNode >* > inputs;
         inputs.push_back( new MockInput() );

         defineInputs( inputs );
      }
   };
   BEGIN_ABSTRACT_OBJECT( GBMEndNode );
      PARENT( GBMockNode );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class GBMUtilNode : public GBMockNode
   {
      DECLARE_ALLOCATOR( GBMUtilNode, AM_DEFAULT );
      DECLARE_CLASS();

   public:
      int         m_idx;

   public:
      GBMUtilNode( int idx ) : GBMockNode( idx )
      {
         std::vector< GBNodeInput< GBMockNode >* > inputs;
         inputs.push_back( new MockInput() );
         defineInputs( inputs );

         std::vector< GBNodeOutput< GBMockNode >* > outputs;
         outputs.push_back( new MockOutput() );
         defineOutputs( outputs );
      }
   };
   BEGIN_ABSTRACT_OBJECT( GBMUtilNode );
      PARENT( GBMockNode );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class GraphBuilderMock : public GraphBuilder< GBMockNode >
   {
   public:
      // -----------------------------------------------------------------
      // GraphBuilder implementation
      // -----------------------------------------------------------------
      void onNodeAdded( GBMockNode* node ) {}
      void onNodeRemoved( GBMockNode& node ) {}
   };

}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( MockOutput );
DEFINE_TYPE_ID( MockInput );
DEFINE_TYPE_ID( GBMockNode );
DEFINE_TYPE_ID( GBMStartNode );
DEFINE_TYPE_ID( GBMEndNode );
DEFINE_TYPE_ID( GBMUtilNode );

///////////////////////////////////////////////////////////////////////////////

TEST( GraphBuilderInstantiation, simpleGraph )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< MockOutput >( "MockOutput", new TSerializableTypeInstantiator< MockOutput >() );
   typesRegistry.addSerializableType< MockInput >( "MockInput", new TSerializableTypeInstantiator< MockInput >() );
   typesRegistry.addSerializableType< GBMStartNode >( "GBMStartNode", NULL );
   typesRegistry.addSerializableType< GBMUtilNode >( "GBMUtilNode", NULL );
   typesRegistry.addSerializableType< GBMEndNode >( "GBMEndNode", NULL );

   GraphBuilderMock builder;

   std::vector< GBMockNode* > nodes;
   builder.setGraphBuilderNodes( &nodes );

   GBMockNode* node[] = { new GBMStartNode( 0 ), new GBMUtilNode( 1 ), new GBMEndNode( 2 ) };
   GraphBuilderTransaction< GBMockNode > transaction( builder );
   transaction.addNode( node[0] );
   transaction.addNode( node[1] );
   transaction.addNode( node[2] );
   transaction.commit();
   transaction.connectNodes( node[0], "Output", node[1], "Input" );
   transaction.connectNodes( node[1], "Output", node[2], "Input" );

   Graph< GBMockNode* > graph;
   builder.buildGraph( graph );

   Array< int > result;
   GraphUtils< GBMockNode* >::topologicalSort( result, graph );

   CPPUNIT_ASSERT_EQUAL( (unsigned int)3, result.size() );
   CPPUNIT_ASSERT_EQUAL( node[0], graph.getNode( result[0] ) );
   CPPUNIT_ASSERT_EQUAL( node[1], graph.getNode( result[1] ) );
   CPPUNIT_ASSERT_EQUAL( node[2], graph.getNode( result[2] ) );

   // clear the types registry
   typesRegistry.clear();
   
   for ( uint i = 0; i < 3; ++i )
   {
      delete node[i];
   }
}

///////////////////////////////////////////////////////////////////////////////
