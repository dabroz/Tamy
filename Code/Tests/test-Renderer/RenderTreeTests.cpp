#include "core-TestFramework\TestFramework.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderTree.h"
#include "core-MVC\Entity.h"
#include "core\ReflectionObject.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class RendererImplementationMock : public RendererImplementation
   {
      DECLARE_ALLOCATOR( RendererImplementationMock, AM_DEFAULT );
   };

   // -------------------------------------------------------------------------

   class GeometryMock : public GeometryComponent
   {
      DECLARE_ALLOCATOR( GeometryMock, AM_ALIGNED_16 );

   private:
      std::string    m_id;

   public:
      GeometryMock( const std::string& id ) 
         : m_id( std::string( "RenderGeometry_" ) + id + ";" )
      {}

      void render( Renderer& renderer, VertexShaderConfigurator* externalConfigurator )
      {
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( RenderTree, basics )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL );
   typesRegistry.addSerializableType< Entity >( "Entity", NULL );
   typesRegistry.addSerializableType< GeometryComponent >( "GeometryComponent", NULL );
   typesRegistry.addSerializableType< Material >( "Material", NULL );
   typesRegistry.addSerializableType< RenderState >( "RenderState", NULL );

   Material mat1;
   Material mat2;

   GeometryMock* g1 = new GeometryMock( "1" );
   GeometryMock* g2 = new GeometryMock( "2" );

   g1->setMaterial( &mat1 );
   g2->setMaterial( &mat2 );

   Array< GeometryComponent* > geometry( 2 );
   geometry.push_back( g1 );
   geometry.push_back( g2 );
   
   // setup the renderer
   RendererImplementationMock* rendererImpl = new RendererImplementationMock();
   Renderer renderer( rendererImpl );

   RenderTree* renderTree = renderer.buildRenderTree( geometry );
   {

      StateTreeNode* analyzedNode = renderTree->m_root;
      CPPUNIT_ASSERT( analyzedNode != NULL );
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat1 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );
         {
            CPPUNIT_ASSERT( analyzedNode->m_geometryNode->m_next == NULL );
            CPPUNIT_ASSERT( &analyzedNode->m_geometryNode->m_geometry == g1 );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling != NULL );
         analyzedNode = analyzedNode->m_sibling;
      }

      // child state 2 that renders geometry2 using state2
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat2 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );
         {
            CPPUNIT_ASSERT( analyzedNode->m_geometryNode->m_next == NULL );
            CPPUNIT_ASSERT( &analyzedNode->m_geometryNode->m_geometry == g2 );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling == NULL );
      }
   }
   
   // cleanup
   delete renderTree;
   delete g1;
   delete g2;
}

///////////////////////////////////////////////////////////////////////////////

TEST( RenderTree, statesBatching )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", NULL );
   typesRegistry.addSerializableType< GeometryComponent >( "GeometryComponent", NULL );
   typesRegistry.addSerializableType< Material >( "Material", NULL );
   typesRegistry.addSerializableType< RenderState >( "RenderState", NULL );

   Material mat1;
   Material mat2;

   GeometryMock* g1 = new GeometryMock( "1" );
   GeometryMock* g2 = new GeometryMock( "2" );
   GeometryMock* g3 = new GeometryMock( "3" );

   g1->setMaterial( &mat1 );      // it uses state 1
   g2->setMaterial( &mat2 );      // uses a different state - state 2
   g3->setMaterial( &mat1 ); // it also uses state 1

   Array< GeometryComponent* > geometry( 3 );
   geometry.push_back( g1 );
   geometry.push_back( g2 );
   geometry.push_back( g3 );

   // setup the renderer
   RendererImplementationMock* rendererImpl = new RendererImplementationMock();
   Renderer renderer( rendererImpl );

   RenderTree* renderTree = renderer.buildRenderTree( geometry );
   {
      // child state 1 that renders using state1
      StateTreeNode* analyzedNode = renderTree->m_root;
      CPPUNIT_ASSERT( analyzedNode != NULL );
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat1 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );

         // two geometry nodes are rendered using this state - g1...
         GeometryNode* analyzedGeometryNode = analyzedNode->m_geometryNode;
         {
            CPPUNIT_ASSERT( &analyzedGeometryNode->m_geometry == g3 );

            CPPUNIT_ASSERT( analyzedGeometryNode->m_next != NULL );
            analyzedGeometryNode = analyzedGeometryNode->m_next;
         }
         // .. and g1
         {
            CPPUNIT_ASSERT( &analyzedGeometryNode->m_geometry == g1 );

            CPPUNIT_ASSERT( analyzedGeometryNode->m_next == NULL );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling != NULL );
         analyzedNode = analyzedNode->m_sibling;
      }

      // child state 2 that renders geometry2 using state2
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat2 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );
         {
            CPPUNIT_ASSERT( analyzedNode->m_geometryNode->m_next == NULL );
            CPPUNIT_ASSERT( &analyzedNode->m_geometryNode->m_geometry == g2 );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling == NULL );
      }
   }

   // cleanup
   delete renderTree;
   delete g1;
   delete g2;
   delete g3;
}

///////////////////////////////////////////////////////////////////////////////

TEST( RenderTree, manySingleStatesBatching )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", NULL );
   typesRegistry.addSerializableType< GeometryComponent >( "GeometryComponent", NULL );
   typesRegistry.addSerializableType< Material >( "Material", NULL );
   typesRegistry.addSerializableType< RenderState >( "RenderState", NULL );

   Material mat1;
   Material mat2;
   Material mat3;

   GeometryMock* g1 = new GeometryMock( "1" );
   GeometryMock* g2 = new GeometryMock( "2" );
   GeometryMock* g3 = new GeometryMock( "3" );
   GeometryMock* g4 = new GeometryMock( "4" );

   g1->setMaterial( &mat1 );
   g2->setMaterial( &mat2 );
   g3->setMaterial( &mat3 );
   g4->setMaterial( &mat3 );

   Array< GeometryComponent* > geometry( 4 );
   geometry.push_back( g1 );
   geometry.push_back( g2 );
   geometry.push_back( g3 );
   geometry.push_back( g4 );

   // setup the renderer
   RendererImplementationMock* rendererImpl = new RendererImplementationMock();
   Renderer renderer( rendererImpl );   

   RenderTree* renderTree = renderer.buildRenderTree( geometry );
   {
      // child state 1 that renders using state1
      StateTreeNode* analyzedNode = renderTree->m_root;
      CPPUNIT_ASSERT( analyzedNode != NULL );
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat1 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );

         // g1 is the rendered geometry node
         {
            CPPUNIT_ASSERT( &analyzedNode->m_geometryNode->m_geometry == g1 );
            CPPUNIT_ASSERT( analyzedNode->m_geometryNode->m_next == NULL );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling != NULL );
         analyzedNode = analyzedNode->m_sibling;
      }

      // child state 2 that renders using state2
      CPPUNIT_ASSERT( analyzedNode != NULL );
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat2 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );

         // g2 is the rendered geometry node
         {
            CPPUNIT_ASSERT( &analyzedNode->m_geometryNode->m_geometry == g2 );
            CPPUNIT_ASSERT( analyzedNode->m_geometryNode->m_next == NULL );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling != NULL );
         analyzedNode = analyzedNode->m_sibling;
      }

      // child state 3 that renders using state3, and it renders 2 geometry pieces
      CPPUNIT_ASSERT( analyzedNode != NULL );
      {
         CPPUNIT_ASSERT( analyzedNode->m_state->m_material == &mat3 );
         CPPUNIT_ASSERT( analyzedNode->m_geometryNode != NULL );

         //the geometry pieces rendered are g4 ...
         GeometryNode* analyzedGeometryNode = analyzedNode->m_geometryNode;
         {
            CPPUNIT_ASSERT( &analyzedGeometryNode->m_geometry == g4 );
            CPPUNIT_ASSERT( analyzedGeometryNode->m_next != NULL );

            analyzedGeometryNode = analyzedGeometryNode->m_next;
         }

         // ...and g3
         {
            CPPUNIT_ASSERT( &analyzedGeometryNode->m_geometry == g3 );
            CPPUNIT_ASSERT( analyzedGeometryNode->m_next == NULL );
         }

         CPPUNIT_ASSERT( analyzedNode->m_sibling == NULL );
      }
   }

   // cleanup
   delete renderTree;
   delete g1;
   delete g2;
   delete g3;
   delete g4;
}

///////////////////////////////////////////////////////////////////////////////
