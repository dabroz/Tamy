#include "oal-Sound\OALSoundEntitiesFactory.h"
#include "oal-Sound\OpenALSoundSystem.h"
#include "oal-Sound\OALSound3D.h"
#include "oal-Sound\OALSoundListener.h"


///////////////////////////////////////////////////////////////////////////////

OALSoundEntitiesFactory::OALSoundEntitiesFactory(OpenALSoundSystem& soundSystem)
      : m_soundSystem(soundSystem)
{
}

///////////////////////////////////////////////////////////////////////////////

Sound3D* OALSoundEntitiesFactory::createEmiter(const std::string& name, 
                                               Sound& sound, 
                                               float hearingRadius)
{
   return new OALSound3D(m_soundSystem, name, sound, hearingRadius);
}

///////////////////////////////////////////////////////////////////////////////

SoundListener* OALSoundEntitiesFactory::createListener()
{
   return new OALSoundListener(m_soundSystem);
}

///////////////////////////////////////////////////////////////////////////////