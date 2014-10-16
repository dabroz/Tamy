#include "core-TestFramework\TestFramework.h"
#include "core-Scene\Model.h"
#include "core-Scene\Entity.h"
#include "core\Serializer.h"
#include "core-Scene\ModelView.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   class EntityAMock : public Entity
   {
   private:
      int m_index;

   public:
      EntityAMock() : INIT_ENTITY(EntityAMock), m_index(-1) {}
      EntityAMock(int idx) : INIT_ENTITY(EntityAMock), m_index(idx) {}

      void save(Serializer& serializer)
      {
         serializer.saveInt(m_index);
      }

      void load(Serializer& serializer)
      {
         m_index = serializer.loadInt();
      }

      int getIndex() const {return m_index;}

   protected:
      void registerProperties() {}
   };
   DEFINE_ENTITY(EntityAMock);

   // -------------------------------------------------------------------------

   class EntityBMock : public Entity
   {
   private:
      int m_index;

   public:
      EntityBMock() : INIT_ENTITY(EntityBMock), m_index(-1) {}
      EntityBMock(int idx) : INIT_ENTITY(EntityBMock), m_index(idx) {}

      void save(Serializer& serializer)
      {
         serializer.saveInt(m_index);
      }

      void load(Serializer& serializer)
      {
         m_index = serializer.loadInt();
      }

      int getIndex() const {return m_index;}

   protected:
      void registerProperties() {}
   };
   DEFINE_ENTITY(EntityBMock);

   // -------------------------------------------------------------------------

   class ModelSerializerMock : public Serializer
   {
   private:
      std::vector<int>& m_storage;

   public:
      ModelSerializerMock(std::vector<int>& storage)
         : m_storage(storage)
      {}

      void saveInt(int val)
      {
         m_storage.push_back(val);
      }

      int loadInt()
      {
         int val = m_storage.front();
         m_storage.erase(m_storage.begin());

         return val;
      }

      void saveFloat(float val) {}
      float loadFloat() {return 0;}
      void saveBool(bool val) {}
      bool loadBool() {return false;}
      void saveChar(char val) {}
      char loadChar() {return 0;}
      void saveLong(long val) {}
      long loadLong() {return 0;}
      void saveString(const std::string& str) {}
      void loadString(std::string& str) {}
      void saveMatrix(const D3DXMATRIX& mtx) {}
      void loadMatrix(D3DXMATRIX& mtx) {}
      void saveVec3(const D3DXVECTOR3& vec) {}
      void loadVec3(D3DXVECTOR3& vec) {}
   };

   // -------------------------------------------------------------------------

   class ViewMock : public ModelView
   {
   private:
      int m_entitiesCount;

   public:
      ViewMock() : m_entitiesCount(0) {}

      int getEntitiesObserved() const 
      {
         return m_entitiesCount;
      }

      void onEntityAdded(Entity& entity) 
      {
         m_entitiesCount++;
      }
      void onEntityRemoved(Entity& entity) 
      {
         m_entitiesCount--;
      }

      void resetContents()
      {
         m_entitiesCount = 0;
      }
   };

   // -------------------------------------------------------------------------

   class ViewsCreatingViewMock : public ModelView
   {
   private:
      typedef std::map<Entity*, ModelView*> Views;
      Views m_views;

      Model& m_model;

   public:
      ViewsCreatingViewMock(Model& model) : m_model(model) {}

      ~ViewsCreatingViewMock()
      {
         resetContents();
      }

      void onEntityAdded(Entity& entity) 
      {
         Views::iterator it = m_views.find(&entity);
         if (it == m_views.end())
         {
            ModelView* newView = new ViewMock();
            m_model.attach(*newView);

            m_views.insert(std::make_pair(&entity, newView));
         }
      }

      void onEntityRemoved(Entity& entity) 
      {
         Views::iterator it = m_views.find(&entity);
         if (it != m_views.end())
         {
            delete it->second;
            m_views.erase(it);
         }
      }

   protected:
      void resetContents()
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
   
   class ProgressObserverMock : public Model::ProgressObserver
   {
   private:
      int& m_entriesCount;

   public:
      ProgressObserverMock(int& entriesCount) : m_entriesCount(entriesCount) {}

      void setProgress(float val)
      {
         ++m_entriesCount;
      }
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST(Model, serialization)
{
   Model model;

   model.add(new EntityAMock(5));
   model.add(new EntityBMock(10));

   std::vector<int> storage;
   ModelSerializerMock serializer(storage);
   model.save(serializer);

   CPPUNIT_ASSERT_EQUAL((unsigned int)2, model.getEntitiesCount());
   CPPUNIT_ASSERT(NULL != dynamic_cast<EntityAMock*> (&model.getEntity(0)));
   CPPUNIT_ASSERT(NULL != dynamic_cast<EntityBMock*> (&model.getEntity(1)));

   model.clear();

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model.getEntitiesCount());

   model.load(serializer);

   CPPUNIT_ASSERT_EQUAL((unsigned int)2, model.getEntitiesCount());
   CPPUNIT_ASSERT_EQUAL(5,  dynamic_cast<EntityAMock*> (&model.getEntity(0))->getIndex());
   CPPUNIT_ASSERT_EQUAL(10, dynamic_cast<EntityBMock*> (&model.getEntity(1))->getIndex());
}

///////////////////////////////////////////////////////////////////////////////

TEST(Model, serializationProgress)
{
   Model model;

   model.add(new EntityAMock(1));
   model.add(new EntityAMock(2));
   model.add(new EntityAMock(3));
   model.add(new EntityAMock(4));

   int saveProgressEntries = 0;
   int loadProgressEntries = 0;

   std::vector<int> storage;
   ModelSerializerMock serializer(storage);
   model.save(serializer, new ProgressObserverMock(saveProgressEntries));
   CPPUNIT_ASSERT_EQUAL(4, saveProgressEntries);

   model.clear();

   model.load(serializer, new ProgressObserverMock(loadProgressEntries));
   CPPUNIT_ASSERT_EQUAL(4, loadProgressEntries);
}


///////////////////////////////////////////////////////////////////////////////

TEST(Model, viewingEntities)
{
   Model model;

   ViewMock view1;
   model.attach(view1);

   Entity* entity = new EntityAMock(5);

   CPPUNIT_ASSERT_EQUAL(0, view1.getEntitiesObserved());
   model.add(entity);
   CPPUNIT_ASSERT_EQUAL(1, view1.getEntitiesObserved());
   model.add(new EntityBMock(10));
   CPPUNIT_ASSERT_EQUAL(2, view1.getEntitiesObserved());

   ViewMock view2;
   model.attach(view2);
   CPPUNIT_ASSERT_EQUAL(2, view2.getEntitiesObserved());

   model.remove(*entity);
   CPPUNIT_ASSERT_EQUAL(1, view1.getEntitiesObserved());
   CPPUNIT_ASSERT_EQUAL(1, view2.getEntitiesObserved());

   model.add(new EntityBMock(5));
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

   model->attach(*view);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, model->getViewsCount());

   // let's remove the view without explicitly detaching it
   // from the model and see if model-view link is properly
   // cleaned up
   delete view; 
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model->getViewsCount());
   model->add(new EntityAMock(0));

   // if it hasn't crashed till now, means that that link
   // was cleaned up ok.

   // Now let's attach a new view, and let's delete a model to which
   // the view is attached. Later on - let's delete the view
   // and see if it was properly informed that it's model was 
   // deleted ok.
   view = new ViewMock();
   model->attach(*view);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, model->getViewsCount());

   delete model;
   CPPUNIT_ASSERT_EQUAL(false, view->isAttachedToModel());

   delete view;
   
   // if it hasn't crashed till this point, means that every thing's a-OK :).
}

///////////////////////////////////////////////////////////////////////////////

TEST(Model, reattachingViewBetweenModels)
{
   Model model1;
   Model model2;
   ViewMock view;

   model1.add(new EntityAMock(10));

   model2.add(new EntityBMock(10));
   model2.add(new EntityBMock(10));

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model1.getViewsCount());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model2.getViewsCount());
   CPPUNIT_ASSERT_EQUAL(false, view.isAttachedToModel());
   CPPUNIT_ASSERT_EQUAL(0, view.getEntitiesObserved());

   model1.attach(view);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, model1.getViewsCount());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model2.getViewsCount());
   CPPUNIT_ASSERT_EQUAL(true, view.isAttachedToModel());
   CPPUNIT_ASSERT_EQUAL(1, view.getEntitiesObserved());

   model2.attach(view);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, model1.getViewsCount());
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, model2.getViewsCount());
   CPPUNIT_ASSERT_EQUAL(true, view.isAttachedToModel());
   CPPUNIT_ASSERT_EQUAL(2, view.getEntitiesObserved());
}

///////////////////////////////////////////////////////////////////////////////

TEST(Model, nestedViews)
{
   Model* model = new Model();
   
   ViewsCreatingViewMock view(*model);
   model->attach(view);

   CPPUNIT_ASSERT_EQUAL((unsigned int)1, model->getViewsCount());

   EntityAMock entity1(10);
   model->add(&entity1);
   model->add(new EntityAMock(10));
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, model->getViewsCount());

   model->remove(entity1, false);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, model->getViewsCount());
   
   delete model;
}

///////////////////////////////////////////////////////////////////////////////
