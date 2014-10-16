#include "core-TestFramework\TestFramework.h"
#include "core\TimeController.h"
#include "core\TimeDependent.h"
#include "core\TimeEvent.h"
#include "core\TimeControllerTrack.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   class UpdatableMock : public TimeDependent
   {
   private:
      int               m_timesCalled;
      float             m_lastTime;

      // scheduled detachment related data
      TimeController*   m_controller;
      UpdatableMock*    m_updatableToDetach;

   public:
      UpdatableMock() 
         : m_timesCalled( 0 )
         , m_lastTime( 0 ) 
         , m_controller( NULL )
         , m_updatableToDetach( NULL )
      {}

      void update( float timeElapsed )
      {
         m_lastTime = timeElapsed;
         ++m_timesCalled;

         if ( m_controller && m_updatableToDetach )
         {
            m_controller->remove( *m_updatableToDetach );
            m_controller = NULL;
            m_updatableToDetach = NULL;
         }
      }

      int getTimesCalled()
      {
         int result = m_timesCalled;
         m_timesCalled = 0;
         return result;
      }

      float getLastTime() const 
      {
         return m_lastTime;
      }

      void scheduleDetachmentOf( TimeController* controller, UpdatableMock* updatable )
      {
         m_controller = controller;
         m_updatableToDetach = updatable;
      }
   };

   // -------------------------------------------------------------------------

   class SpeedChangeEventMock : public TimeEvent
   {
   private:
      float m_newSpeed;
      TimeControllerTrack& m_track;

   public:
      SpeedChangeEventMock(const std::string& id,
         float newSpeed,
         TimeControllerTrack& track)
         : TimeEvent(id),
         m_newSpeed(newSpeed),
         m_track(track)
      {}

      void execute()
      {
         m_track.setSpeed(m_newSpeed);
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( TimeController, multipleTracks )
{
   TimeController controller;

   UpdatableMock updatable1;
   UpdatableMock updatable2;
   controller.add( updatable1 );
   controller.add( updatable2 );

   controller.update(1);
   CPPUNIT_ASSERT_EQUAL(1.f, updatable1.getLastTime());
   CPPUNIT_ASSERT_EQUAL(1.f, updatable2.getLastTime());

   controller.update(2);
   CPPUNIT_ASSERT_EQUAL(2.f, updatable1.getLastTime());
   CPPUNIT_ASSERT_EQUAL(2.f, updatable2.getLastTime());
};

///////////////////////////////////////////////////////////////////////////////

TEST(TimeControllerTrack, timeScaling)
{
   UpdatableMock updatable1;
   UpdatableMock updatable2;
   TimeController controller;

   TimeControllerTrack& track1 = controller.add( updatable1 );
   TimeControllerTrack& track2 = controller.add( updatable2 );

   track1.update(1);
   track2.update(1);
   CPPUNIT_ASSERT_EQUAL(1.f, updatable1.getLastTime());
   CPPUNIT_ASSERT_EQUAL(1.f, updatable2.getLastTime());

   track1.setSpeed(0.5f);

   track1.update(1);
   track2.update(1);
   CPPUNIT_ASSERT_EQUAL(0.5f, updatable1.getLastTime());
   CPPUNIT_ASSERT_EQUAL(1.f, updatable2.getLastTime());

};

///////////////////////////////////////////////////////////////////////////////

TEST(TimeControllerTrack, framerateLimitation)
{
   UpdatableMock updatable;
   TimeController controller;
   TimeControllerTrack& track = controller.add( updatable );

   // this limit means that the track can be updated maximally twice per second
   track.setLimit(2);

   // let's check it - first a single update per second...
   track.update(1);
   CPPUNIT_ASSERT_EQUAL(1, updatable.getTimesCalled());

   // ... two regular updates per second
   track.update(0.5f);
   track.update(0.5f);
   CPPUNIT_ASSERT_EQUAL(2, updatable.getTimesCalled());

   // ... four regular updates per second
   track.update(0.25f);
   track.update(0.25f);
   track.update(0.25f);
   track.update(0.25f);
   CPPUNIT_ASSERT_EQUAL(2, updatable.getTimesCalled());
   CPPUNIT_ASSERT_EQUAL(0.5f, updatable.getLastTime());

   // irregular updates...
   track.update(0.2f);
   CPPUNIT_ASSERT_EQUAL(0, updatable.getTimesCalled());

   track.update(0.4f);
   CPPUNIT_ASSERT_EQUAL(1, updatable.getTimesCalled());
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6f, updatable.getLastTime(), 0.0001f);

   track.update(0.1f);
   CPPUNIT_ASSERT_EQUAL(0, updatable.getTimesCalled());

   track.update(0.2f);
   CPPUNIT_ASSERT_EQUAL(0, updatable.getTimesCalled());

   track.update(0.2f);
   CPPUNIT_ASSERT_EQUAL(1, updatable.getTimesCalled());
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5f, updatable.getLastTime(), 0.0001f);
};

///////////////////////////////////////////////////////////////////////////////

TEST( TimeController, updatableRemoval )
{
   TimeController controller;

   UpdatableMock updatable1;
   UpdatableMock updatable2;

   controller.add( updatable1 );
   controller.add( updatable2 );

   CPPUNIT_ASSERT_EQUAL( 0, updatable1.getTimesCalled() );
   CPPUNIT_ASSERT_EQUAL( 0, updatable2.getTimesCalled() );

   controller.update( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 1, updatable1.getTimesCalled() );
   CPPUNIT_ASSERT_EQUAL( 1, updatable2.getTimesCalled() );

   // schedule the entity that runs earlier to detach the one that runs after it
   updatable1.scheduleDetachmentOf( &controller, &updatable2 );

   // during this update, when updatable1 gets updated, it will remove updatable2 from the controller,
   // and that one will no longer be updated
   controller.update( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 1, updatable1.getTimesCalled() );
   CPPUNIT_ASSERT_EQUAL( 0, updatable2.getTimesCalled() );

   controller.update( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 1, updatable1.getTimesCalled() );
   CPPUNIT_ASSERT_EQUAL( 0, updatable2.getTimesCalled() );
}

///////////////////////////////////////////////////////////////////////////////
