#pragma once

#include "core-Sound\Sound3D.h"


///////////////////////////////////////////////////////////////////////////////

class Sound3DMock : public Sound3D
{
private:
   bool m_updated;

public:
   Sound3DMock(const char* name, Sound& sound, float soundHearingRadius = 1000.f) 
         : Sound3D(name, sound, soundHearingRadius), m_updated(false)
   {}

   void update(SoundListener& listener) {m_updated = true;}

   bool wasUpdated()
   {
      bool result = m_updated;
      m_updated = false;
      return result;
   }

protected:
   void onChannelAssigned(SoundChannel& channel)
   {
   }
};

///////////////////////////////////////////////////////////////////////////////