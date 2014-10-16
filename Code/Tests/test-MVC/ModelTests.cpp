#include "core-TestFramework\TestFramework.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core-MVC\ModelView.h"
#include <d3dx9.h>



///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   class EntityAMock : public Entity
   {
      DECLARE_ALLOCATOR( EntityAMock, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int m_index;

   public:
      EntityAMock() : m_index( -1 ) {}
      EntityAMock( int idx ) : m_index( idx ) {}
      EntityAMock( const EntityAMock& rhs ) : Entity( rhs ), m_index( rhs.m_index ) {}

      int getIndex() const { return m_index; }
   };
   BEGIN_OBJECT(EntityAMock);
      PARENT(Entity);
      PROPERTY_EDIT("index", int, m_index);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class EntityBMock : public Entity
   {
      DECLARE_ALLOCATOR( EntityBMock, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int m_index;

   public:
      EntityBMock() : m_index( -1 ) {}
      EntityBMock( int idx ) : m_index( idx ) {}
      EntityBMock( const EntityBMock& rhs ) : Entity( rhs ), m_index( rhs.m_index ) {}

      int getIndex() const { return m_index; }

   };
   BEGIN_OBJECT(EntityBMock);
      PARENT(Entity);
      PROPERTY_EDIT("index", int, m_index);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class ViewMock : public ModelView
   {
      DECLARE_ALLOCATOR( ViewMock, AM_DEFAULT );

   private:
      int m_entitiesCount;

   public:
      ViewMock() : m_entitiesCount(0) {}

      int getEntitiesObserved() const 
      {
         return m_entitiesCount;
      }

      void onNodeAdded( SceneNode* node) 
      {
         m_entitiesCount++;
      }
      
      void onNodeRemoved( SceneNode* node) 
      {
         m_entitiesCount--;
      }

      void onNodeChanged( SceneNode* node )
      {
      }

      void resetContents( Model& model )
      {
         m_entitiesCount = 0;
      }
   };

   // -------------------------------------------------------------------------

   class ViewsCreatingViewMock : public ModelView
   {
      DECLARE_ALLOCATOR( ViewsCreatingViewMock, AM_DEFAULT );

   private:
      typedef std::map< SceneNode*, ModelView* > Views;
      Views m_views;

      Model& m_model;

   public:
      ViewsCreatingViewMock( Model& model ) : m_model( model ) {}

      ~ViewsCreatingViewMock()
      {
         resetContents( m_model );
      }

      void onNodeAdded( SceneNode* node ) 
      {
         Views::iterator it = m_views.find( node );
         if ( it == m_views.end() )
         {
            ModelView* newView = new ViewMock();
            m_model.attachListener( newView );

            m_views.insert( std::make_pair( node, newView ) );
         }
      }

      void onNodeRemoved( SceneNode* node ) 
      {
         Views::iterator it = m_views.find( node );
         if ( it != m_views.end() )
         {
            delete it->second;
            m_views.erase( it );
         }
      }

      void onNodeChanged( SceneNode* node )
      {
      }

   protected:
      void resetContents( Model& model )
      {
         for (Views::iterator it = m_views.begin();
            it != m_views.end(); ++it)
         {
            delete it->second;
         }
         m_views.clear();
      }
   };

   // -------------------------------------------------------------------------

   class EntityCMock : public Entity
   {
      DECLARE_ALLOCATOR( EntityCMock, AM_DEFAULT );
      DECLARE_CLASS()

   private:
      int&     m_attachedToModelCount;
      int&     m_attachedToEntityCount;

   public:
      EntityCMock( int& attachedToModelCount, int& attachedToEntityCount )
         : m_attachedToModelCount( attachedToModelCount )
         , m_attachedToEntityCount( attachedToEntityCount )
      {}

      void onAttachToModel( Model* model )
      {
         Entity::onAttachToModel( model );

         m_attachedToModelCount++;
      }

      void onDetachFromModel( Model* model )
      {
         m_attachedToModelCount--;

         Entity::onDetachFromModel( model );
      }

      void onAttached( Entity* parent )
      {
         Entity::onAttached( parent );

         m_attachedToEntityCount++;
      }

      void onDetached( Entity* parent )
      {
         m_attachedToEntityCount--;

         Entity::onDetached( parent );
      }
   };
   BEGIN_OBJECT(EntityCMock);
      PARENT(Entity);
   END_OBJECT();

} // anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( EntityAMock );
DEFINE_TYPE_ID( EntityBMock );
DEFINE_TYPE_ID( EntityCMock );

///////////////////////////////////////////////////////////////////////////////

TEST(Model, viewingEntities)
{
   Model model;

   ViewMock view1;
   model.attachListener( &view1 );

   Entity* entity = new EntityAMock(5);

   CPPUNIT_ASSERT_EQUAL(0, view1.getEntitiesObserved());

   model.addChild( entity );
   CPPUNIT_ASSERT_EQUAL(1, view1.getEntitiesObserved());

   model.addChild( new EntityBMock(10) );
   CPPUNIT_ASSERT_EQUAL(2, view1.getEntitiesObserved());

   ViewMock view2;
   model.attachListener( &view2 );
   model.pullStructure( &view2 );
   CPPUNIT_ASSERT_EQUAL(2, view2.getEntitiesObserved());

   entity->remove();
   CPPUNIT_ASSERT_EQUAL(1, view1.getEntitiesObserved());
   CPPUNIT_ASSERT_EQUAL(1, view2.getEntitiesObserved());

   model.addChild( new EntityBMock(5) );
   CPPUNIT_ASSERT_EQUAL(2, view1.getEntitiesObserved());
   CPPUNIT_ASSERT_EQUAL(2, view2.getEntitiesObserved());

   model.clear();
   CPPUNIT_ASSERT_EQUAL(0, view1.getEntitiesObserved());
   CPPUNIT_ASSERT_EQUAL(0, view2.getEntitiesObserved());
}

///////////////////////////////////////////////////////////////////////////////

TEST(Model, attachingAndDetachingViews)
{
   Model* model = new Model();
   ViewMock* view = new ViewMock();

   model->attachListener( view );

   // let's remove the view without explicitly detaching it
   // from the model and see if model-view link is properly
   // cleaned up
   delete view; 

   model->addChild( new EntityAMock(0) );

   // if it hasn't crashed till now, means that that link
   // was cleaned up ok.

   // Now let's attach a new view, and let's delete a model to which
   // the view is attached. Later on - let's delete the view
   // and see if it was properly informed that it's model was 
   // deleted ok.
   view = new ViewMock();
   model->attachListener( view );

   delete model;
   CPPUNIT_ASSERT_EQUAL( false, view->isAttachedToModel() );

   delete view;
   
   // if it hasn't crashed till this point, means that every thing's a-OK :).
}

///////////////////////////////////////////////////////////////////////////////

TEST( Model, attachingToMultipleModels )
{
   Model model1;
   Model model2;
   ViewMock view;

   model1.addChild( new EntityAMock(10) );

   model2.addChild( new EntityBMock(10) );
   model2.addChild( new EntityBMock(10) );
   CPPUNIT_ASSERT_EQUAL( false, view.isAttachedToModel() );
   CPPUNIT_ASSERT_EQUAL( 0, view.getEntitiesObserved() );

   model1.attachListener( &view );
   model1.pullStructure( &view );
   CPPUNIT_ASSERT_EQUAL( true, view.isAttachedToModel() );
   CPPUNIT_ASSERT_EQUAL( 1, view.getEntitiesObserved() );

   model2.attachListener( &view );
   model2.pullStructure( &view );
   CPPUNIT_ASSERT_EQUAL( true, view.isAttachedToModel() );
   CPPUNIT_ASSERT_EQUAL( 3, view.getEntitiesObserved() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Model, attachingChildEntity )
{
   // A child entity can be added to a parent entity
   // either before that one was added to a model, or afterwards.
   // Either way - the child should be registered as owned by that model,
   // and in both cases - it should NOT end up in the model's m_managedEntities list,
   // as it is managed only by its immediate parent - the parent entity it was added to

   Model model;

   Entity* parent1 = new EntityAMock(1);
   Entity* parent2 = new EntityAMock(2);

   Entity* child1 = new EntityAMock(1);
   Entity* child2 = new EntityAMock(2);

   // the first case is adding a child to a parent before that one was added to the mode
   parent1->addChild( child1 );
   model.addChild( parent1 );

   // the second one is adding the parent entity to the model first, and then adding the child entity to it
   model.addChild( parent2 );
   parent2->addChild( child2 );

   CPPUNIT_ASSERT_EQUAL( 1, parent1->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( 1, parent2->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( 1, child1->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( 1, child2->getReferencesCount() );
   CPPUNIT_ASSERT_EQUAL( &model, parent1->getHostModel() );
   CPPUNIT_ASSERT_EQUAL( &model, parent2->getHostModel() );
   CPPUNIT_ASSERT_EQUAL( &model, child1->getHostModel() );
   CPPUNIT_ASSERT_EQUAL( &model, child2->getHostModel() );

   // when we attach a view to the model, we should still be able to observe all 4 entities
   ViewMock view;
   model.attachListener( &view );
   model.pullStructure( &view );
   CPPUNIT_ASSERT_EQUAL( 4, view.getEntitiesObserved() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Model, attachmentAndDetachmentNotifications )
{
   int attachedToModelCount = 0;
   int attachedToEntityCount = 0;

   Model* model = new Model();
   EntityCMock* root = new EntityCMock( attachedToModelCount, attachedToEntityCount );
   EntityCMock* child = new EntityCMock( attachedToModelCount, attachedToEntityCount );

   CPPUNIT_ASSERT_EQUAL( 0, attachedToModelCount );
   CPPUNIT_ASSERT_EQUAL( 0, attachedToEntityCount );

   model->addChild( root );
   CPPUNIT_ASSERT_EQUAL( 1, attachedToModelCount );
   CPPUNIT_ASSERT_EQUAL( 1, attachedToEntityCount );

   root->addChild( child );
   CPPUNIT_ASSERT_EQUAL( 2, attachedToModelCount );
   CPPUNIT_ASSERT_EQUAL( 2, attachedToEntityCount );

   model->removeReference();
   CPPUNIT_ASSERT_EQUAL( 0, attachedToModelCount );
   CPPUNIT_ASSERT_EQUAL( 0, attachedToEntityCount );
}

///////////////////////////////////////////////////////////////////////////////

