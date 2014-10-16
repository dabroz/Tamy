#include "core-TestFramework\TestFramework.h"
#include "core\GenericFactory.h"
#include <map>
#include <string>
#include <typeinfo>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class TemplateObject : public ReflectionObject
   {
      DECLARE_CLASS()

   public:
      virtual ~TemplateObject() {}

      virtual void f() {}
   };
   BEGIN_OBJECT(TemplateObject);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class TemplateObjectA : public TemplateObject
   {
      DECLARE_CLASS()

   public:
      void f() {}
   };
   BEGIN_OBJECT(TemplateObjectA);
      PARENT(TemplateObject);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class TemplateObjectB : public TemplateObject
   {
      DECLARE_CLASS()

   public:
      void f() {}
   };
   BEGIN_OBJECT(TemplateObjectB);
      PARENT(TemplateObject);
   END_OBJECT();

   // -------------------------------------------------------------------------

   class IRepresentation
   {
   public:
      virtual ~IRepresentation() {}

      virtual std::string getID() const = 0;
   };

   // -------------------------------------------------------------------------

   class CommonRepresentation : public IRepresentation
   {
   public:
      CommonRepresentation(TemplateObject& obj) {}

      std::string getID() const {return "CommonRepresentation";}
   };

   // -------------------------------------------------------------------------

   class RepresentationA : public IRepresentation
   {
   public:
      RepresentationA(TemplateObjectA& obj) {}

      std::string getID() const {return "RepresentationA";}
   };

   // -------------------------------------------------------------------------

   class RepresentationB : public IRepresentation 
   {
   public:
      RepresentationB(TemplateObjectB& obj) {}

      std::string getID() const {return "RepresentationB";}
   };
   
} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( TemplateObject );
DEFINE_TYPE_ID( TemplateObjectA );
DEFINE_TYPE_ID( TemplateObjectB );

///////////////////////////////////////////////////////////////////////////////

TEST(GenericFactory, creatingObjects)
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< TemplateObject >( "TemplateObject", NULL );
   typesRegistry.addSerializableType< TemplateObjectA >( "TemplateObjectA", new TSerializableTypeInstantiator< TemplateObjectA >() );
   typesRegistry.addSerializableType< TemplateObjectB >( "TemplateObjectB", new TSerializableTypeInstantiator< TemplateObjectB >() );

   GenericFactory<TemplateObject, IRepresentation> factory;
   factory.associate<TemplateObjectA, RepresentationA>()
          .associate<TemplateObjectB, RepresentationB>();


   TemplateObject* obA = new TemplateObjectA();
   TemplateObject* obB = new TemplateObjectB();

   IRepresentation* rep = factory.create(*obA);
   CPPUNIT_ASSERT(NULL != rep);
   CPPUNIT_ASSERT_EQUAL(std::string("RepresentationA"), rep->getID());
   delete rep;

   rep = factory.create(*obB);
   CPPUNIT_ASSERT(NULL != rep);
   CPPUNIT_ASSERT_EQUAL(std::string("RepresentationB"), rep->getID());
   delete rep;


   delete obA;
   delete obB;

   // clear the types registry
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST(GenericFactory, creatingObjectsForAbstractClasses)
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.addSerializableType< TemplateObject >( "TemplateObject", NULL );
   typesRegistry.addSerializableType< TemplateObjectA >( "TemplateObjectA", new TSerializableTypeInstantiator< TemplateObjectA >() );
   typesRegistry.addSerializableType< TemplateObjectB >( "TemplateObjectB", new TSerializableTypeInstantiator< TemplateObjectB >() );

   GenericFactory<TemplateObject, IRepresentation> factory;
   factory.associate<TemplateObjectA, RepresentationA>()
      .associateAbstract<TemplateObject, CommonRepresentation>();


   TemplateObject* obA = new TemplateObjectA();
   TemplateObject* obB = new TemplateObjectB();

   IRepresentation* rep = factory.create(*obA);
   CPPUNIT_ASSERT(NULL != rep);
   CPPUNIT_ASSERT_EQUAL(std::string("RepresentationA"), rep->getID());
   delete rep;

   rep = factory.create(*obB);
   CPPUNIT_ASSERT(NULL != rep);
   CPPUNIT_ASSERT_EQUAL(std::string("CommonRepresentation"), rep->getID());
   delete rep;


   delete obA;
   delete obB;

   // clear the types registry
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////
