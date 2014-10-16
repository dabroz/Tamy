/// @file   core/Timer.h
/// @brief  simple timer
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Simple timer class.
 */
class CTimer
{
   DECLARE_ALLOCATOR( CTimer, AM_DEFAULT );

private:
   __int64         m_lastTime;
   float           m_timeScale;
   float           m_timeElapsed;

   unsigned long   m_frameRate;
   unsigned long   m_fpsFrameCount;
   float           m_fpsTimeElapsed;

public:
   CTimer();
   ~CTimer();

   /**
    * Resets the timer.
    */
   void reset();

   /**
    * Updates the timer.
    */
   void tick();

   /**
    * Returns the time that elapsed since the last reset.
    */
   float getTimeElapsed() const;

   /**
    * Returns current time.
    */
   double getCurrentTime() const;

   /**
    * Returns the framerate ( number of ticks / time elapsed )
    */
   unsigned long getFrameRate() const;
};

///////////////////////////////////////////////////////////////////////////////
