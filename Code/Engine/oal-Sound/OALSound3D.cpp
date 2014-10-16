#include "oal-Sound\OALSound3D.h"
#include "oal-Sound\OALSoundChannel.h"
#include "oal-Sound\OpenALSoundSystem.h"
#include <cassert>


///////////////////////////////////////////////////////////////////////////////

OALSound3D::OALSound3D(OpenALSoundSystem& soundSystem, 
                       const std::string& name, 
                       Sound& sound, 
                       float soundHearingRadius)
      : Sound3D(name, sound, soundHearingRadius),
      m_soundSystem(soundSystem),
      m_oalSource(0)
{
}

///////////////////////////////////////////////////////////////////////////////

void OALSound3D::update(SoundListener& listener)
{
   if (m_oalSource == 0) {return;}

   D3DXMATRIX mtx = getGlobalMtx();

   ALfloat sourceOri[] = {mtx._31, mtx._32, mtx._33, 
                          mtx._21, mtx._22, mtx._23};

   m_soundSystem.alSource3f(m_oalSource, AL_POSITION, mtx._41, mtx._42, mtx._42);
   m_soundSystem.alSourcefv(m_oalSource, AL_ORIENTATION, sourceOri);
}

///////////////////////////////////////////////////////////////////////////////

void OALSound3D::onChannelAssigned(SoundChannel& channel)
{
   OALSoundChannel* oalChannel = dynamic_cast<OALSoundChannel*> (&channel);
   assert(oalChannel != NULL);

   m_oalSource = oalChannel->getOALSource();
   m_soundSystem.alSourcei(m_oalSource, AL_SOURCE_RELATIVE, AL_FALSE);
   m_soundSystem.alSourcef(m_oalSource, AL_MAX_DISTANCE, m_soundHearingRadius);
   m_soundSystem.alSourcef(m_oalSource, AL_ROLLOFF_FACTOR, 2);
   m_soundSystem.alSourcef(m_oalSource, AL_REFERENCE_DISTANCE, m_soundHearingRadius * 0.1f);
}

///////////////////////////////////////////////////////////////////////////////
