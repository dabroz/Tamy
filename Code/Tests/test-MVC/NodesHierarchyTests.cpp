#include "core-TestFramework\TestFramework.h"
#include "core-TestFramework\MatrixWriter.h"
#include "TypesRegistryInitializer.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Entity, attachingChild )
{
   MVCTESTS_INIT_TYPES_REGISTRY;

   Entity root;
   Entity* child = new Entity();
   
   CPPUNIT_ASSERT( NULL == root.getParent() );
   CPPUNIT_ASSERT( NULL == child->getParent() );

   root.addChild( child );

   CPPUNIT_ASSERT( NULL == root.getParent() );
   CPPUNIT_ASSERT( &root == child->getParent());
};

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, switchingChildren )
{
   MVCTESTS_INIT_TYPES_REGISTRY;

   Entity root;
   Entity differentRoot;
   Entity* child = new Entity();

   CPPUNIT_ASSERT_EQUAL( (uint)0, root.m_children.size() );
   CPPUNIT_ASSERT( NULL == differentRoot.getParent() );
   CPPUNIT_ASSERT_EQUAL( (uint)0, differentRoot.m_children.size()  );

   root.addChild( child );

   CPPUNIT_ASSERT_EQUAL( (uint)1, root.m_children.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)0, differentRoot.m_children.size() );

   CPPUNIT_ASSERT( &root == child->getParent() );

   differentRoot.addChild( child );

   CPPUNIT_ASSERT_EQUAL( (uint)0, root.m_children.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, differentRoot.m_children.size() );

   CPPUNIT_ASSERT( &differentRoot == child->getParent() );
};

///////////////////////////////////////////////////////////////////////////////
