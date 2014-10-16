#pragma once

#include "core-Sound\SoundEntitiesFactory.h"


///////////////////////////////////////////////////////////////////////////////

class OpenALSoundSystem;

///////////////////////////////////////////////////////////////////////////////

class OALSoundEntitiesFactory : public SoundEntitiesFactory
{
private:
   OpenALSoundSystem& m_soundSystem;

public:
   OALSoundEntitiesFactory(OpenALSoundSystem& soundSystem);

   Sound3D* createEmiter(const std::string& name, 
                         Sound& sound, 
                         float hearingRadius);

   SoundListener* createListener();
};

///////////////////////////////////////////////////////////////////////////////
