/// @file   core\TimeControllerTrack.h
/// @brief  time controller track
#pragma once

#include "core\MemoryRouter.h"
#include "core\TimeDependent.h"
#include "core\TimeEvent.h"


///////////////////////////////////////////////////////////////////////////////

class TimeControllerTrack
{
   DECLARE_ALLOCATOR( TimeControllerTrack, AM_DEFAULT );

public:
   TimeDependent&                m_object;

private:
   float                         m_speed;
   float                         m_updateFreq;

   float                         m_timeline;
   float                         m_lastUpdate;
   float                         m_nextUpdate;

   bool                          m_play;

public:
   /**
    * Constructor.
    *
    * @param object
    */
   TimeControllerTrack( TimeDependent& object );
   ~TimeControllerTrack();

   /**
    * Updates the state of the track.
    *
    * @param timeElapsed   time elapsed since the last time this method was called.
    */
   void update( float timeElapsed );

   /**
    * This allows to set the track's speed. The speed
    * determines what time value will the objects residing
    * on the track's time line receive.
    *
    * For instance - if the speed = 2 and one sec has passed,
    * the objects will be updated with the timeElapsed value = 2.
    *
    * Track's internal time line is not affected by this parameter.
    * This means that the events are not influenced by the track's speed.
    * It considers only TimeDependent objects !!!
    */
   float getSpeed() const { return m_speed; }

   /**
    * Allows to adjust the speed with which the track is running.
    *
    * @param multiplier       speed multiplier
    */
   void setSpeed( float multiplier );

   /**
    * Allows to set an execution frequency limit.
    *
    * @param timesPerSec      how many times per second can the track be updated
    */
   void setLimit( float timesPerSec );

   /**
    * Returns current track time.
    */
   inline float getTime() const { return m_timeline; }

   /**
    * Resets the track's contents ( events and objects )
    * as well as resets its settings ( speed ).
    */
   void reset();

private:

   friend class TimeController;

   /**
    * Freezes the track when it was scheduled for removal.
    */
   void play( bool enable );
};

///////////////////////////////////////////////////////////////////////////////
