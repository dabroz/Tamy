#include "core-TestFramework\TestFramework.h"
#include "core-TestFramework\MatrixWriter.h"
#include "core-MVC\Entity.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Entity, singleNode )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() );

   Entity root;
   
   Matrix matrix;
   matrix.setTranslation( Vector( 5, 0, 0 ) );

   root.setLocalMtx(matrix);
   root.updateTransforms();
   CPPUNIT_ASSERT_EQUAL(matrix, root.getLocalMtx());
   CPPUNIT_ASSERT_EQUAL(matrix, root.getGlobalMtx());
};

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, simpleHierarchy )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() );

   Entity root;
   Entity* child = new Entity();
   root.addChild( child );
   
   Matrix childLocalMatrix = child->getLocalMtx();
   Matrix matrix;
   matrix.setTranslation( Vector( 5, 0, 0 ) );

   root.setLocalMtx( matrix );
   root.updateTransforms();
   CPPUNIT_ASSERT_EQUAL( childLocalMatrix, child->getLocalMtx() );
   CPPUNIT_ASSERT_EQUAL( matrix, child->getGlobalMtx() );
};

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, parentMatrixChangesTwoLevelsAboveTheCheckedNode )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() );

   Entity root;
   Entity* childLevel1 = new Entity();
   Entity* childLevel2 = new Entity();
   root.addChild( childLevel1 );
   childLevel1->addChild( childLevel2 );
   
   Matrix matrix;
   matrix.setTranslation( Vector( 5, 0, 0 ) );

   root.setLocalMtx(matrix);
   root.updateTransforms();
   CPPUNIT_ASSERT_EQUAL(matrix, childLevel2->getGlobalMtx());
};

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, sequenceOfUpdatesInfluencingChild )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() );

   Entity root;
   Entity* child = new Entity();
   root.addChild( child );

   // 1st update
   Matrix rootLocalMatrix; rootLocalMatrix.setTranslation( Vector( 5, 0, 0 ) );
   root.setLocalMtx(rootLocalMatrix);
   root.updateTransforms();
   CPPUNIT_ASSERT_EQUAL(rootLocalMatrix, child->getGlobalMtx());

   // 2nd update
   rootLocalMatrix.setTranslation( Vector( 10, 0, 0 ) );
   root.setLocalMtx(rootLocalMatrix);
   root.updateTransforms();
   CPPUNIT_ASSERT_EQUAL(rootLocalMatrix, child->getGlobalMtx());
};

///////////////////////////////////////////////////////////////////////////////
