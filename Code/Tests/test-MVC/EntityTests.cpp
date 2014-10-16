#include "core-TestFramework\TestFramework.h"
#include "TypesRegistryInitializer.h"
#include "core-MVC\EntityUtils.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class ClonableEntityAMock : public Entity
   {
      DECLARE_ALLOCATOR( ClonableEntityAMock, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int m_index;

   public:
      ClonableEntityAMock() : m_index( -1 ) {}
      ClonableEntityAMock( int idx ) : m_index( idx ) {}
      ClonableEntityAMock( const ClonableEntityAMock& rhs ) : Entity( rhs ), m_index( rhs.m_index ) {}

      int getIndex() const { return m_index; }
   };
   BEGIN_OBJECT( ClonableEntityAMock );
      PARENT( Entity );
      PROPERTY_EDIT("index", int, m_index);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class ClonableEntityBMock : public Entity
   {
      DECLARE_ALLOCATOR( ClonableEntityBMock, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int m_index;

   public:
      ClonableEntityBMock() : m_index( -1 ) {}
      ClonableEntityBMock( int idx ) : m_index( idx ) {}
      ClonableEntityBMock( const ClonableEntityBMock& rhs ) : Entity( rhs ), m_index( rhs.m_index ) {}

      int getIndex() const { return m_index; }
   };
   BEGIN_OBJECT( ClonableEntityBMock );
      PARENT( Entity );
      PROPERTY_EDIT("index", int, m_index);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class MockComponentA : public Component
   {
      DECLARE_ALLOCATOR( MockComponentA, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int*        m_attachmentsCount;
      int*        m_otherAttachedComponentsCount;

   public:
      MockComponentA( int* attachmentsCount = NULL, int* otherAttachedComponentsCount = NULL )
         : m_attachmentsCount( attachmentsCount )
         , m_otherAttachedComponentsCount( otherAttachedComponentsCount )
      {}

      MockComponentA( const MockComponentA& rhs )
         : Component( rhs )
         , m_attachmentsCount( rhs.m_attachmentsCount )
         , m_otherAttachedComponentsCount( rhs.m_otherAttachedComponentsCount )
      {}

   protected:
      // ----------------------------------------------------------------------
      // Component implementation
      // ----------------------------------------------------------------------
      void onAttached( Entity* parent )
      {
         Component::onAttached( parent );

         if ( m_attachmentsCount )
         {
            ++(*m_attachmentsCount);
         }
      }

      void onDetached( Entity* parent )
      {
         if ( m_attachmentsCount )
         {
            --(*m_attachmentsCount);
         }

         Component::onDetached( parent );
      }

      void onSiblingAttached( SceneNode* node )
      {
         if ( m_otherAttachedComponentsCount )
         {
            ++(*m_otherAttachedComponentsCount);
         }
      }

      void onSiblingDetached( SceneNode* node )
      {
         if ( m_otherAttachedComponentsCount )
         {
            --(*m_otherAttachedComponentsCount);
         }
      }
   };
   BEGIN_OBJECT( MockComponentA );
      PARENT( Component );
   END_OBJECT();

   // -------------------------------------------------------------------------

   struct MockComponentB : public Component
   {
      DECLARE_ALLOCATOR( MockComponentB, AM_DEFAULT );
      DECLARE_STRUCT()

      int      m_val;

      MockComponentB( int val = 0 ) : m_val( val ) {}

      MockComponentB( const MockComponentB& rhs )
         : Component( rhs )
         , m_val( rhs.m_val )

      {}
   };
   BEGIN_OBJECT( MockComponentB );
      PARENT( Component );
   END_OBJECT();

}

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( MockComponentA );
DEFINE_TYPE_ID( MockComponentB );
DEFINE_TYPE_ID( ClonableEntityAMock );
DEFINE_TYPE_ID( ClonableEntityBMock );

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, cloningSimple )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( ClonableEntityAMock );
   MVCTESTS_REGISTER_TYPE( MockComponentA );
   MVCTESTS_REGISTER_TYPE( MockComponentB );

   int attachmentsCount = 0;
   Entity* entity = new ClonableEntityAMock( 5 );
   entity->addChild( new MockComponentA( &attachmentsCount ) );
   entity->addChild( new MockComponentB( 10 ) );

   // currently there's only one component of type MockComponentA that's attached to something
   CPPUNIT_ASSERT_EQUAL( 1, attachmentsCount );

   Entity* clonedEntity = static_cast< Entity* >( entity->clone() );

   {
      CPPUNIT_ASSERT( clonedEntity->isExactlyA< ClonableEntityAMock >() );

      ClonableEntityAMock* castClonedEntity = static_cast< ClonableEntityAMock* >( clonedEntity );
      CPPUNIT_ASSERT_EQUAL( 5, castClonedEntity->getIndex() );
   }

   // verify that the component was cloned as well
   {
      CPPUNIT_ASSERT_EQUAL( (uint)2, clonedEntity->m_children.size() );
      SceneNode* clonedComponentA = clonedEntity->m_children[0];
      SceneNode* clonedComponentB = clonedEntity->m_children[1];

      CPPUNIT_ASSERT( clonedComponentA->isExactlyA< MockComponentA >() );
      CPPUNIT_ASSERT( clonedComponentB->isExactlyA< MockComponentB >() );

      MockComponentB* castClonedComponent = static_cast< MockComponentB* >( clonedComponentB );
      CPPUNIT_ASSERT_EQUAL( 10, castClonedComponent->m_val );
   }

   // after cloning, there are two such components - the original one, and the clone
   CPPUNIT_ASSERT_EQUAL( 2, attachmentsCount );

   // cleanup
   clonedEntity->removeReference();
   CPPUNIT_ASSERT_EQUAL( 1, attachmentsCount );
   entity->removeReference();
   CPPUNIT_ASSERT_EQUAL( 0, attachmentsCount );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, cloningHierarchy )
{
   // setup reflection types
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( ClonableEntityAMock );
   MVCTESTS_REGISTER_TYPE( ClonableEntityBMock );

   Entity* rootEntity = new ClonableEntityAMock( 1 );
   Entity* firstChildEntity = new ClonableEntityBMock( 2 );
   Entity* secondChildEntity = new ClonableEntityAMock( 3 );

   rootEntity->addChild( firstChildEntity );
   firstChildEntity->addChild( secondChildEntity );

   Entity* clonedRootEntity = static_cast< Entity* >( rootEntity->clone() );

   CPPUNIT_ASSERT_EQUAL( std::string( "ClonableEntityAMock" ), clonedRootEntity->getVirtualRTTI().m_name );
   ClonableEntityAMock* castClonedRootEntity = DynamicCast< ClonableEntityAMock >( clonedRootEntity );
   CPPUNIT_ASSERT_EQUAL( 1, castClonedRootEntity->getIndex() );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, clonedRootEntity->m_children.size() );

   Entity* clonedFirstChildEntity = static_cast< Entity* >( clonedRootEntity->m_children[0] );
   CPPUNIT_ASSERT_EQUAL( std::string( "ClonableEntityBMock" ), clonedFirstChildEntity->getVirtualRTTI().m_name );
   ClonableEntityBMock* castClonedFirstChildEntity = DynamicCast< ClonableEntityBMock >( clonedFirstChildEntity );
   CPPUNIT_ASSERT_EQUAL( 2, castClonedFirstChildEntity->getIndex() );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, clonedFirstChildEntity->m_children.size() );

   Entity* clonedSecondChildEntity = static_cast< Entity* >( clonedFirstChildEntity->m_children[0] );
   CPPUNIT_ASSERT_EQUAL( std::string( "ClonableEntityAMock" ), clonedSecondChildEntity->getVirtualRTTI().m_name );
   ClonableEntityAMock* castClonedSecondChildEntity = DynamicCast< ClonableEntityAMock >( clonedSecondChildEntity );
   CPPUNIT_ASSERT_EQUAL( 3, castClonedSecondChildEntity->getIndex() );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, clonedSecondChildEntity->m_children.size() );

   // cleanup
   rootEntity->removeReference();
   clonedRootEntity->removeReference();
}

///////////////////////////////////////////////////////////////////////////////

TEST( Entity, queryingForComponents )
{
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( MockComponentA );
   MVCTESTS_REGISTER_TYPE( MockComponentB );

   Entity chair;
   MockComponentA* componentA_1 = new MockComponentA();
   MockComponentA* componentA_2 = new MockComponentA();
   MockComponentB* componentB_1 = new MockComponentB();
   chair.addChild( componentA_1 );
   chair.addChild( componentA_2 );
   chair.addChild( componentB_1 );

   // query for the component which we attached multiple instances of
   List< MockComponentA* > componentAs;
   EntityUtils::collectNodesByType< MockComponentA >( &chair, componentAs );

   CPPUNIT_ASSERT_EQUAL( (uint)2, componentAs.size() );
   List< MockComponentA* >::iterator it = componentAs.begin();

   MockComponentA* retrievedComponent = *it;
   CPPUNIT_ASSERT_EQUAL( componentA_2, retrievedComponent ); 

   ++it; retrievedComponent = *it;
   CPPUNIT_ASSERT_EQUAL( componentA_1, retrievedComponent );

   // and now the other - using a simplified API
   MockComponentB* queriedComponentB = EntityUtils::getFirstChild< MockComponentB >( &chair );
   CPPUNIT_ASSERT_EQUAL( componentB_1, queriedComponentB );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Component, attachmentNotifications )
{
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( MockComponentA );

   int attachmentsCount = 0;

   Entity* someEntity = new Entity();
   MockComponentA* componentA = new MockComponentA( &attachmentsCount );
   componentA->addReference();

   // initially the component is detached
   CPPUNIT_ASSERT_EQUAL( 0, attachmentsCount );

   // a notification is received when the component is attached to an entity
   someEntity->addChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 1, attachmentsCount );

   // and another one is received when it's detached to an entity
   someEntity->removeChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 0, attachmentsCount );

   // now let's try the same, but let's see what happens when entity goes out of scope
   someEntity->addChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 1, attachmentsCount );

   someEntity->removeReference();
   // notification was also received in this case
   CPPUNIT_ASSERT_EQUAL( 0, attachmentsCount );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Component, notificationsAboutOtherComponents )
{
   MVCTESTS_INIT_TYPES_REGISTRY;
   MVCTESTS_REGISTER_TYPE( MockComponentA );

   int attachmentsCount = 0;
   int componentA_ExternalAttachmentsCount = 0;
   int componentB_ExternalAttachmentsCount = 0;

   Entity* someEntity = new Entity();
   MockComponentA* componentA = new MockComponentA( &attachmentsCount, &componentA_ExternalAttachmentsCount );
   MockComponentA* componentB = new MockComponentA( &attachmentsCount, &componentB_ExternalAttachmentsCount );
   componentA->addReference();

   // initially the component is detached
   CPPUNIT_ASSERT_EQUAL( 0, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 0, componentB_ExternalAttachmentsCount );

   // first component gets attached - so no external attach notifications are sent just yet
   someEntity->addChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 0, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 0, componentB_ExternalAttachmentsCount );

   // we're attaching the second component now - both components are informed about each other
   someEntity->addChild( componentB );
   CPPUNIT_ASSERT_EQUAL( 1, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 1, componentB_ExternalAttachmentsCount );

   // when removing any component ( let's remove the first one that was attached ) - both components receive detachment notifications
   someEntity->removeChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 0, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 0, componentB_ExternalAttachmentsCount );

   // now lets reattach it
   someEntity->addChild( componentA );
   CPPUNIT_ASSERT_EQUAL( 1, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 1, componentB_ExternalAttachmentsCount );
   
   // and delete the parent entity - both components receive the notifications
   someEntity->removeReference();
   CPPUNIT_ASSERT_EQUAL( 0, componentA_ExternalAttachmentsCount );
   CPPUNIT_ASSERT_EQUAL( 0, componentB_ExternalAttachmentsCount );
}

///////////////////////////////////////////////////////////////////////////////
