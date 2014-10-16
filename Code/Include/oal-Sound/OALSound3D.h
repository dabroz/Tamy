#pragma once

#include "core-Sound\Sound3D.h"
#include <al.h>
#include <alc.h>


///////////////////////////////////////////////////////////////////////////////

class OpenALSoundSystem;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is an OpenAL implementation of a 3D scene sound
 */
class OALSound3D : public Sound3D
{
private:
   OpenALSoundSystem& m_soundSystem;
   ALuint m_oalSource;

public:
   OALSound3D(OpenALSoundSystem& soundSystem,
              const std::string& name, 
              Sound& sound, 
              float soundHearingRadius);

   void update(SoundListener& listener);

protected:
   void onChannelAssigned(SoundChannel& channel);
};

///////////////////////////////////////////////////////////////////////////////
