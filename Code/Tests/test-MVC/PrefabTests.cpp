#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   class MockComponent : public Component
   {
      DECLARE_ALLOCATOR( MockComponent, AM_DEFAULT );
      DECLARE_CLASS();

   public:
      MockComponent( const char* name = "MockComponent" )
         : Component( name )
      {}

      MockComponent( const MockComponent& rhs )
         : Component( rhs )
      {}

   };
   BEGIN_OBJECT( MockComponent );
      PARENT( Component );
   END_OBJECT();
}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( MockComponent );

///////////////////////////////////////////////////////////////////////////////

TEST( Prefab, instantiation )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;

   // create a test entity and turn it into a prefab
   Entity originalEntity( "myEntity" );
   {
      originalEntity.addChild( new Entity( "child" ) );
   }
   Matrix& mtx1 = originalEntity.accessLocalMtx();
   mtx1.setTranslation( Vector( 1, 0, 0 ) );

   Prefab prefab;
   prefab.setEntity( &originalEntity );
   
   // instantiate the prefab twice and make sure we get two distinct copies of the original entity
   Entity* prefabbedEntity1 = new Entity(); 
   Entity* prefabbedEntity2 = new Entity(); 
   prefabbedEntity1->setPrefab( &prefab ); 
   prefabbedEntity2->setPrefab( &prefab );

   Model model;
   model.addChild( prefabbedEntity1 );
   model.addChild( prefabbedEntity2 );

   // verify that instantiated entities have all the children
   CPPUNIT_ASSERT_EQUAL( (uint)1, prefabbedEntity1->m_children.size() );
   CPPUNIT_ASSERT_EQUAL( std::string( "child" ), prefabbedEntity1->m_children[0]->getSceneNodeName() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, prefabbedEntity2->m_children.size() );
   CPPUNIT_ASSERT_EQUAL( std::string( "child" ), prefabbedEntity2->m_children[0]->getSceneNodeName() );

   // instantiated entities end up in origin - the transform of the original entity is not shared with them
   Matrix& mtx2 = prefabbedEntity1->accessLocalMtx();
   Matrix& mtx3 = prefabbedEntity2->accessLocalMtx();
   COMPARE_VEC( Vector( 1, 0, 0 ), originalEntity.getLocalMtx().position() );
   COMPARE_VEC( Vector( 0, 0, 0 ), prefabbedEntity1->getLocalMtx().position() );
   COMPARE_VEC( Vector( 0, 0, 0 ), prefabbedEntity2->getLocalMtx().position() );

   // the only thing that is not carried over from the original entity to its instances
   // is the transform - instances can be placed in arbitrary locations in the game world.
   // otherwise there would be no point
   mtx2.setTranslation( Vector( 0, 2, 0 ) );
   mtx3.setTranslation( Vector( 0, 0, 3 ) );
   COMPARE_VEC( Vector( 1, 0, 0 ), originalEntity.getLocalMtx().position() );
   COMPARE_VEC( Vector( 0, 2, 0 ), prefabbedEntity1->getLocalMtx().position() );
   COMPARE_VEC( Vector( 0, 0, 3 ), prefabbedEntity2->getLocalMtx().position() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Prefab, saving )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( MockComponent );
   MVCTESTS_INIT_PATCHES;

   // prepare the resources manager
   MVCTESTS_INIT_RESOURCE_MANAGER( "..\\Data" );
   FilePath modelPath( "prefabTest_1.tsc" );
   FilePath prefabPath( "prefabTest_1.tpf" );

   // create a test entity and turn it into a prefab
   Prefab* prefab = NULL;
   {
      // create a hierarchical entity
      Entity* originalEntity = NULL;
      {
         originalEntity = new Entity( "originalEntity" );
         Entity* childEntity = new Entity( "childEntity" );
         MockComponent* originalEntityComponent = new MockComponent( "originalEntityComponent" );
         MockComponent* childEntityComponent = new MockComponent( "childEntityComponent" );

         originalEntity->addChild( originalEntityComponent );
         originalEntity->addChild( childEntity );
         childEntity->addChild( childEntityComponent );
      }

      prefab = new Prefab( prefabPath );
      prefab->setEntity( originalEntity );
      resMgr.addResource( prefab );

      // we can now safely remove the original entity
      originalEntity->removeReference();
      originalEntity = NULL;
   }

   // save the original model and the prefab
   {
      Model* model = new Model( modelPath );
      {
         Entity* prefabEntity = new Entity( "prefabEntity" );
         prefabEntity->setPrefab( prefab );
         model->addChild( prefabEntity );
      }
      resMgr.addResource( model );

      {
         CPPUNIT_ASSERT_EQUAL( (uint)1, model->getRoot()->m_children.size() );

         SceneNode* child_1 = model->getRoot()->m_children[0];
         CPPUNIT_ASSERT( child_1->isA< Entity >() );

         Entity* pfEntity_1 = static_cast< Entity* >( child_1 );
         CPPUNIT_ASSERT_EQUAL( (uint)2, pfEntity_1->m_children.size() );

         SceneNode* child_1_1 = pfEntity_1->m_children[0];
         CPPUNIT_ASSERT( child_1_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "originalEntityComponent" ), child_1_1->getSceneNodeName() );

         SceneNode* child_1_2 = pfEntity_1->m_children[1];
         CPPUNIT_ASSERT( child_1_2->isA< Entity >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntity" ), child_1_2->getSceneNodeName() );

         Entity* pfEntity_2 = static_cast< Entity* >( child_1_2 );
         CPPUNIT_ASSERT_EQUAL( (uint)1, pfEntity_2->m_children.size() );

         SceneNode* child_2_1 = pfEntity_2->m_children[0];
         CPPUNIT_ASSERT( child_2_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntityComponent" ), child_2_1->getSceneNodeName() );
      }

      model->saveResource();
      prefab->saveResource();
   }

   // restore the prefab
   {
      Prefab* restoredPrefab = static_cast< Prefab* >( resMgr.create( prefabPath ) );
      CPPUNIT_ASSERT( restoredPrefab != NULL );

      Entity* prefabbedEntity = new Entity( "PrefabbedEntity" );
      prefabbedEntity->setPrefab( restoredPrefab );

      Model* testModel = new Model();
      testModel->addChild( prefabbedEntity );

      Entity* rootEntity = testModel->getRoot();
      {
         CPPUNIT_ASSERT_EQUAL( (uint)1, rootEntity->m_children.size() );

         SceneNode* child_1 = rootEntity->m_children[0];
         CPPUNIT_ASSERT( child_1->isA< Entity >() );

         Entity* pfEntity_1 = static_cast< Entity* >( child_1 );
         CPPUNIT_ASSERT_EQUAL( (uint)2, pfEntity_1->m_children.size() );

         SceneNode* child_1_1 = pfEntity_1->m_children[0];
         CPPUNIT_ASSERT( child_1_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "originalEntityComponent" ), child_1_1->getSceneNodeName() );

         SceneNode* child_1_2 = pfEntity_1->m_children[1];
         CPPUNIT_ASSERT( child_1_2->isA< Entity >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntity" ), child_1_2->getSceneNodeName() );

         Entity* pfEntity_2 = static_cast< Entity* >( child_1_2 );
         CPPUNIT_ASSERT_EQUAL( (uint)1, pfEntity_2->m_children.size() );

         SceneNode* child_2_1 = pfEntity_2->m_children[0];
         CPPUNIT_ASSERT( child_2_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntityComponent" ), child_2_1->getSceneNodeName() );
      }

      // cleanup
      testModel->removeReference();
   }


   // remove all registered resources
   resMgr.reset();

   {
      // restore the model
      Model* restoredModel = static_cast< Model* >( resMgr.create( modelPath ) );
      CPPUNIT_ASSERT( restoredModel != NULL );
      {
         CPPUNIT_ASSERT_EQUAL( (uint)1, restoredModel->getRoot()->m_children.size() );

         SceneNode* child_1 = restoredModel->getRoot()->m_children[0];
         CPPUNIT_ASSERT( child_1->isA< Entity >() );

         Entity* pfEntity_1 = static_cast< Entity* >( child_1 );
         CPPUNIT_ASSERT_EQUAL( (uint)2, pfEntity_1->m_children.size() );

         SceneNode* child_1_1 = pfEntity_1->m_children[0];
         CPPUNIT_ASSERT( child_1_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "originalEntityComponent" ), child_1_1->getSceneNodeName() );

         SceneNode* child_1_2 = pfEntity_1->m_children[1];
         CPPUNIT_ASSERT( child_1_2->isA< Entity >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntity" ), child_1_2->getSceneNodeName() );

         Entity* pfEntity_2 = static_cast< Entity* >( child_1_2 );
         CPPUNIT_ASSERT_EQUAL( (uint)1, pfEntity_2->m_children.size() );

         SceneNode* child_2_1 = pfEntity_2->m_children[0];
         CPPUNIT_ASSERT( child_2_1->isExactlyA< MockComponent >() );
         CPPUNIT_ASSERT_EQUAL( std::string( "childEntityComponent" ), child_2_1->getSceneNodeName() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Prefab, prefabEmbeddedInAnotherPrefab )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_INIT_PATCHES;

   // prepare the resources manager
   MVCTESTS_INIT_RESOURCE_MANAGER( "..\\Data" );

   // create test prefabs
   Prefab* prefab1 = NULL;
   {
      Entity* originalEntity = new Entity( "myEntity_1" );
      originalEntity->addChild( new Entity( "child_1" ) );

      FilePath prefabPath( "prefabTest_2.tpf" );
      prefab1 = new Prefab( prefabPath );
      prefab1->setEntity( originalEntity );
      resMgr.addResource( prefab1 );

      // we can now safely remove the original entity
      originalEntity->removeReference();
      originalEntity = NULL;
   }

   Prefab* prefab2 = NULL;
   {
      Entity* originalEntity = new Entity( "myEntity_2" );
      originalEntity->addChild( new Entity( "child_2" ) );

      Entity* childWithPrefab = new Entity( "childWithPrefab_3" );
      originalEntity->addChild( childWithPrefab );
      childWithPrefab->setPrefab( prefab1 );

      FilePath prefabPath( "prefabTest_3.tpf" );
      prefab2 = new Prefab( prefabPath );
      prefab2->setEntity( originalEntity );
      resMgr.addResource( prefab2 );

      // we can now safely remove the original entity
      originalEntity->removeReference();
      originalEntity = NULL;
   }

   // setup a model
   FilePath modelPath( "prefabTest_2.tsc" );
   Model* model = new Model( modelPath );
   {
      Entity* prefabEntity = new Entity( "prefabEntity" );
      model->addChild( prefabEntity );
      prefabEntity->setPrefab( prefab2 );
   }
   resMgr.addResource( model );

   {
      CPPUNIT_ASSERT_EQUAL( (uint)1, model->getRoot()->m_children.size() );

      SceneNode* myEntity_2_Node = model->getRoot()->m_children[0];
      CPPUNIT_ASSERT( myEntity_2_Node->isA< Entity >() );

      Entity* myEntity_2 = static_cast< Entity* >( myEntity_2_Node );
      CPPUNIT_ASSERT_EQUAL( std::string( "prefabEntity" ), myEntity_2->getSceneNodeName() );
      CPPUNIT_ASSERT_EQUAL( (uint)2, myEntity_2->m_children.size() );
      CPPUNIT_ASSERT( myEntity_2->wasInstantiatedFromPrefab() == false );

      SceneNode* child_2_Node = myEntity_2->m_children[0];
      SceneNode* childWithPrefab_3_Node = myEntity_2->m_children[1];
      CPPUNIT_ASSERT( child_2_Node->isA< Entity >() );
      CPPUNIT_ASSERT( childWithPrefab_3_Node->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_2" ), child_2_Node->getSceneNodeName() );
      CPPUNIT_ASSERT_EQUAL( std::string( "childWithPrefab_3" ), childWithPrefab_3_Node->getSceneNodeName() );
      CPPUNIT_ASSERT( child_2_Node->wasInstantiatedFromPrefab() == true );
      CPPUNIT_ASSERT( childWithPrefab_3_Node->wasInstantiatedFromPrefab() == true );

      Entity* childWithPrefab_3 = static_cast< Entity* >( childWithPrefab_3_Node );
      CPPUNIT_ASSERT_EQUAL( (uint)1, childWithPrefab_3->m_children.size() );

      SceneNode* child_1_Node = childWithPrefab_3->m_children[0];
      CPPUNIT_ASSERT( child_1_Node->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_1" ), child_1_Node->getSceneNodeName() );
      CPPUNIT_ASSERT( child_1_Node->wasInstantiatedFromPrefab() == true );
   }

   model->saveResource();
   prefab1->saveResource();
   prefab2->saveResource();

   // remove all registered resources
   resMgr.reset();

   // restore the model
   Model* restoredModel = static_cast< Model* >( resMgr.create( modelPath ) );
   CPPUNIT_ASSERT( restoredModel != NULL );
   {
      CPPUNIT_ASSERT_EQUAL( (uint)1, restoredModel->getRoot()->m_children.size() );

      SceneNode* myEntity_2_Node = restoredModel->getRoot()->m_children[0];
      CPPUNIT_ASSERT( myEntity_2_Node->isA< Entity >() );

      Entity* myEntity_2 = static_cast< Entity* >( myEntity_2_Node );
      CPPUNIT_ASSERT_EQUAL( std::string( "prefabEntity" ), myEntity_2->getSceneNodeName() );
      CPPUNIT_ASSERT_EQUAL( (uint)2, myEntity_2->m_children.size() );

      SceneNode* child_2_Node = myEntity_2->m_children[0];
      SceneNode* childWithPrefab_3_Node = myEntity_2->m_children[1];
      CPPUNIT_ASSERT( child_2_Node->isA< Entity >() );
      CPPUNIT_ASSERT( childWithPrefab_3_Node->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_2" ), child_2_Node->getSceneNodeName() );
      CPPUNIT_ASSERT_EQUAL( std::string( "childWithPrefab_3" ), childWithPrefab_3_Node->getSceneNodeName() );

      Entity* childWithPrefab_3 = static_cast< Entity* >( childWithPrefab_3_Node );
      CPPUNIT_ASSERT_EQUAL( (uint)1, childWithPrefab_3->m_children.size() );

      SceneNode* child_1_Node = childWithPrefab_3->m_children[0];
      CPPUNIT_ASSERT( child_1_Node->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_1" ), child_1_Node->getSceneNodeName() );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Prefab, changingContentsOfAPrefab )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_INIT_PATCHES;

   // create a test entity and turn it into a prefab
   Prefab prefab;
   {
      Entity* originalEntity = new Entity( "myEntity" );
      originalEntity->addChild( new Entity( "child" ) );

      prefab.setEntity( originalEntity );

      // cleanup
      originalEntity->removeReference();
   }

   // instantiate it
   Entity* entity = new Entity();
   entity->setPrefab( &prefab );

   Model model;
   model.addChild( entity );

   // verify all is where it's supposed to be
   {
      CPPUNIT_ASSERT_EQUAL( (uint)1, model.getRoot()->m_children.size() );

      SceneNode* child_1 = model.getRoot()->m_children[0];
      CPPUNIT_ASSERT( child_1->isA< Entity >() );

      Entity* pfEntity_1 = static_cast< Entity* >( child_1 );
      CPPUNIT_ASSERT_EQUAL( (uint)1, pfEntity_1->m_children.size() );

      SceneNode* child_2 = pfEntity_1->m_children[0];
      CPPUNIT_ASSERT( child_2->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child" ), child_2->getSceneNodeName() );
   }

   // now change the prefab's contents
   {
      Entity* newEntity = new Entity( "myEntity_2" );
      newEntity->addChild( new Entity( "child_2" ) );
      newEntity->addChild( new Entity( "child_3" ) );

      prefab.setEntity( newEntity );

      // cleanup
      newEntity->removeReference();
   }

   // inspect the instance to verify that changes were applied
   {
      CPPUNIT_ASSERT_EQUAL( (uint)1, model.getRoot()->m_children.size() );

      SceneNode* child_1 = model.getRoot()->m_children[0];
      CPPUNIT_ASSERT( child_1->isA< Entity >() );

      Entity* pfEntity_1 = static_cast< Entity* >( child_1 );
      CPPUNIT_ASSERT_EQUAL( (uint)2, pfEntity_1->m_children.size() );

      SceneNode* child_2 = pfEntity_1->m_children[0];
      SceneNode* child_3 = pfEntity_1->m_children[1];
      CPPUNIT_ASSERT( child_2->isA< Entity >() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_2" ), child_2->getSceneNodeName() );
      CPPUNIT_ASSERT_EQUAL( std::string( "child_3" ), child_3->getSceneNodeName() );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Prefab, makeUnique )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( MockComponent );
   MVCTESTS_INIT_PATCHES;

   // create a test entity and turn it into a prefab
   Prefab* prefab1 = NULL;
   {
      Entity* originalEntity = new Entity( "myEntity_1" );
      originalEntity->addChild( new Entity( "child_1" ) );

      prefab1 = new Prefab();
      prefab1->setEntity( originalEntity );

      // we can now safely remove the original entity
      originalEntity->removeReference();
      originalEntity = NULL;
   }

   Prefab* prefab2 = NULL;
   {
      Entity* originalEntity = new Entity( "myEntity_2" );
      {
         Entity* child_2 = new Entity( "child_2" );
         {
            MockComponent* child_2_Component = new MockComponent( "child_2_Component" );
            child_2->addChild( child_2_Component );
         }
         originalEntity->addChild( child_2 );
      
         Entity* childWithPrefab = new Entity( "childWithPrefab_3" );
         {
            childWithPrefab->setPrefab( prefab1 );
         }
         originalEntity->addChild( childWithPrefab );
      }
  
      prefab2 = new Prefab();
      prefab2->setEntity( originalEntity );

      // we can now safely remove the original entity
      originalEntity->removeReference();
      originalEntity = NULL;
   }

   Model model;
   Entity* prefabbedEntity = new Entity();
   model.addChild( prefabbedEntity );

   prefabbedEntity->setPrefab( prefab2 );
   {
      CPPUNIT_ASSERT_EQUAL( (uint)2, prefabbedEntity->m_children.size() );
      CPPUNIT_ASSERT( prefabbedEntity->m_prefab == prefab2 );
      CPPUNIT_ASSERT( prefabbedEntity->wasInstantiatedFromPrefab() == false );

      SceneNode* child_2_Node = prefabbedEntity->m_children[0];
      SceneNode* childWithPrefab_3_Node = prefabbedEntity->m_children[1];
      CPPUNIT_ASSERT( child_2_Node->wasInstantiatedFromPrefab() == true );
      CPPUNIT_ASSERT( childWithPrefab_3_Node->wasInstantiatedFromPrefab() == true );

      {
         Entity* child_2_Entity = static_cast< Entity* >( child_2_Node );
         CPPUNIT_ASSERT_EQUAL( (uint)1, child_2_Entity->m_children.size() );

         SceneNode* child_2_Child_Node = child_2_Entity->m_children[0];
         CPPUNIT_ASSERT( child_2_Child_Node->wasInstantiatedFromPrefab() == true );
      }

      {
         Entity* childWithPrefab_3 = static_cast< Entity* >( childWithPrefab_3_Node );
         CPPUNIT_ASSERT( childWithPrefab_3->m_prefab == prefab1 );
         CPPUNIT_ASSERT_EQUAL( (uint)1, childWithPrefab_3->m_children.size() );

         SceneNode* child_1_Node = childWithPrefab_3->m_children[0];
         CPPUNIT_ASSERT( child_1_Node->wasInstantiatedFromPrefab() == true );
      }
   }

   // make the entity unique - at this point a reference to a prefab should disappear, but the entities should stay behind
   prefabbedEntity->makeUnique( false );
   {
      CPPUNIT_ASSERT_EQUAL( (uint)2, prefabbedEntity->m_children.size() );
      CPPUNIT_ASSERT( prefabbedEntity->m_prefab == NULL );
      CPPUNIT_ASSERT( prefabbedEntity->wasInstantiatedFromPrefab() == false );

      SceneNode* child_2_Node = prefabbedEntity->m_children[0];
      SceneNode* childWithPrefab_3_Node = prefabbedEntity->m_children[1];
      CPPUNIT_ASSERT( child_2_Node->wasInstantiatedFromPrefab() == false );
      CPPUNIT_ASSERT( childWithPrefab_3_Node->wasInstantiatedFromPrefab() == false );

      {
         Entity* child_2_Entity = static_cast< Entity* >( child_2_Node );
         CPPUNIT_ASSERT_EQUAL( (uint)1, child_2_Entity->m_children.size() );

         // child without a prefab has also been inlined
         SceneNode* child_2_Child_Node = child_2_Entity->m_children[0];
         CPPUNIT_ASSERT( child_2_Child_Node->wasInstantiatedFromPrefab() == false );
      }

      // embedded prefab entity hasn't been inlined though
      {
         Entity* childWithPrefab_3 = static_cast< Entity* >( childWithPrefab_3_Node );
         CPPUNIT_ASSERT( childWithPrefab_3->m_prefab == prefab1 );
         CPPUNIT_ASSERT_EQUAL( (uint)1, childWithPrefab_3->m_children.size() );

         SceneNode* child_1_Node = childWithPrefab_3->m_children[0];
         CPPUNIT_ASSERT( child_1_Node->wasInstantiatedFromPrefab() == true );
      }
   }

   // cleanup
   prefab1->removeReference();
   prefab2->removeReference();
}

///////////////////////////////////////////////////////////////////////////////
