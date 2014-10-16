#include "core-TestFramework\TestFramework.h"
#include "core-AI\FSMController.h"
#include "core-AI\FSMState.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class StringController : public FSMController< StringController >
   {
      DECLARE_CLASS()

   public:
      std::string* m_log;

   public:
      StringController( std::string* log = NULL )
         : m_log(log)
      {
      }
   };
   BEGIN_OBJECT( StringController );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class IdleState : public FSMState< StringController >
   {
      DECLARE_CLASS()

   public:
      void activate()
      {
         *fsm().m_log += "idle-Entered; ";
      }

      void deactivate()
      {
         *fsm().m_log += "idle-Exited; ";
      }

      void execute( float timeElapsed )
      {
         *fsm().m_log += "idle-Updated; ";
      }
   };
   BEGIN_OBJECT( IdleState );
   END_OBJECT();

   // -------------------------------------------------------------------------

   class WalkState : public FSMState< StringController >
   {
      DECLARE_CLASS()

   public:
      void activate()
      {
         *fsm().m_log += "walk-Entered; ";
      }

      void deactivate()
      {
         *fsm().m_log += "walk-Exited; ";
      }

      void execute( float timeElapsed )
      {
         *fsm().m_log += "walk-Updated; ";
         transitionTo< IdleState >();
      }
   };
   BEGIN_OBJECT( WalkState );
   END_OBJECT();

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( StringController );
DEFINE_TYPE_ID( IdleState );
DEFINE_TYPE_ID( WalkState );

///////////////////////////////////////////////////////////////////////////////

TEST( StateMachine, basicSetupAndUpdates )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< StringController >( "StringController", new TSerializableTypeInstantiator< StringController >() ); 
   typesRegistry.addSerializableType< IdleState >( "IdleState", new TSerializableTypeInstantiator< IdleState >() ); 
   typesRegistry.addSerializableType< WalkState >( "WalkState", new TSerializableTypeInstantiator< WalkState >() ); 

   std::string log;

   StringController fsm( &log );
   fsm.registerState< IdleState >();
   fsm.begin< IdleState >();

   CPPUNIT_ASSERT_EQUAL( std::string( "" ), log );

   fsm.update( 0 );
   CPPUNIT_ASSERT_EQUAL( std::string( "idle-Entered; idle-Updated; " ), log );
   log = "";

   fsm.update(0);
   CPPUNIT_ASSERT_EQUAL( std::string( "idle-Updated; " ), log );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( StateMachine, transitingBetweenStates )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< StringController >( "StringController", new TSerializableTypeInstantiator< StringController >() ); 
   typesRegistry.addSerializableType< IdleState >( "IdleState", new TSerializableTypeInstantiator< IdleState >() ); 
   typesRegistry.addSerializableType< WalkState >( "WalkState", new TSerializableTypeInstantiator< WalkState >() ); 

   std::string log;

   StringController fsm( &log );
   fsm.registerState< IdleState >();
   fsm.registerState< WalkState >();
   fsm.begin< WalkState >();

   CPPUNIT_ASSERT_EQUAL( std::string( "" ), log );

   fsm.update( 0 );
   CPPUNIT_ASSERT_EQUAL( std::string( "walk-Entered; walk-Updated; " ), log );
   log = "";

   fsm.update(0);
   CPPUNIT_ASSERT_EQUAL( std::string( "walk-Exited; idle-Entered; idle-Updated; " ), log );
   log = "";

   fsm.transitionTo< WalkState >();
   fsm.update(0);
   CPPUNIT_ASSERT_EQUAL( std::string( "idle-Exited; walk-Entered; walk-Updated; " ), log );

   // cleanup
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

