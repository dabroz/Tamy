#include "core\TimeControllerTrack.h"
#include "core\TimeDependent.h"
#include <stdexcept>
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

TimeControllerTrack::TimeControllerTrack( TimeDependent& object )
   : m_speed( 1 )
   , m_updateFreq( 0 )
   , m_timeline( 0 )
   , m_lastUpdate( 0 )
   , m_nextUpdate( 0 )
   , m_object( object )
   , m_play( true )
{
}

///////////////////////////////////////////////////////////////////////////////

TimeControllerTrack::~TimeControllerTrack()
{
   reset();
}

///////////////////////////////////////////////////////////////////////////////

void TimeControllerTrack::play( bool enable )
{
   m_play = enable;
}

///////////////////////////////////////////////////////////////////////////////

void TimeControllerTrack::setSpeed( float multiplier )
{
   if (multiplier < 0) {multiplier = 0;}

   m_speed = multiplier;
}

///////////////////////////////////////////////////////////////////////////////

void TimeControllerTrack::setLimit( float timesPerSec )
{
   m_updateFreq = 1.f / timesPerSec;
}

///////////////////////////////////////////////////////////////////////////////

void TimeControllerTrack::update( float timeElapsed )
{
   if ( !m_play )
   {
      return;
   }

   // determine if this is the right time to
   // run an update on this track, or should we wait
   m_timeline += timeElapsed;
   if (m_timeline >= m_nextUpdate)
   {
      timeElapsed = (m_timeline - m_lastUpdate);
      m_lastUpdate = m_timeline;

      if (m_updateFreq > 0)
      {
         while (m_nextUpdate <= m_timeline) 
         {
            m_nextUpdate += m_updateFreq;
         }
      }
      else
      {
         m_nextUpdate = m_timeline;
      }
   }
   else
   {
      return;
   }


   // update the objects on the track
   float timeForObjects = timeElapsed * m_speed;
   m_object.update( timeForObjects );
}

///////////////////////////////////////////////////////////////////////////////

void TimeControllerTrack::reset()
{
   m_speed = 1;
}

///////////////////////////////////////////////////////////////////////////////
