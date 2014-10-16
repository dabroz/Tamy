#include "oal-Sound\OALSoundListener.h"
#include "oal-Sound\OpenALSoundSystem.h"
#include <al.h>
#include <alc.h>
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

OALSoundListener::OALSoundListener(OpenALSoundSystem& soundSystem)
      : m_soundSystem(soundSystem)
{
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundListener::update()
{
   D3DXMATRIX mtx = getGlobalMtx();

   ALfloat listenerOri[] = {mtx._31, mtx._32, mtx._33, 
                            mtx._21, -mtx._22, mtx._23};

   m_soundSystem.alListener3f(AL_POSITION, mtx._41, mtx._42, mtx._42);
   m_soundSystem.alListenerfv(AL_ORIENTATION, listenerOri);
}

///////////////////////////////////////////////////////////////////////////////
