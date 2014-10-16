#include "core-Sound\SoundRenderer.h"
#include "core-Sound\SoundDevice.h"
#include "core-Sound\SoundSceneManager.h"
#include "core-Sound\SoundListener.h"
#include "core-Sound\SoundChannel.h"
#include "core-Sound\Sound3D.h"


///////////////////////////////////////////////////////////////////////////////

SoundRenderer::SoundRenderer(SoundDevice& soundDevice)
      : m_soundDevice(soundDevice)
{
}

///////////////////////////////////////////////////////////////////////////////

SoundRenderer::~SoundRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////

void SoundRenderer::addSoundScene(SoundSceneManager& scene)
{
   m_soundScenes.push_back(&scene);
}

///////////////////////////////////////////////////////////////////////////////

void SoundRenderer::removeSoundScene(SoundSceneManager& scene)
{
   unsigned int idx = m_soundScenes.find(&scene);
   if (idx != EOA)
   {
      m_soundScenes.remove(idx);
   }
}

///////////////////////////////////////////////////////////////////////////////

void SoundRenderer::update(float timeElapsed)
{
   unsigned int scenesCount = m_soundScenes.size();
   for (unsigned int sceneIdx = 0; sceneIdx < scenesCount; ++sceneIdx)
   {
      SoundSceneManager& soundScene = *(m_soundScenes[sceneIdx]);

      if (soundScene.hasActiveListener() == false) {continue;}

      // update the listener
      SoundListener& listener = soundScene.getActiveListener();
      listener.update();

      m_soundsToDisable.clear();
      m_soundsToEnable.clear();

      Array<Sound3D*>& activeSounds = soundScene.update(m_soundsToDisable, m_soundsToEnable);
    
      DWORD soundsCount = 0;

      // deactivate sounds that can no longer be heard
      soundsCount = m_soundsToDisable.size();
      for (DWORD i = 0; i < soundsCount; ++i)
      {
         m_soundsToDisable[i]->deassignChannel(m_soundDevice);
      }

      // assign channels to active sounds
      soundsCount = m_soundsToEnable.size();
      for (DWORD i = 0; i < soundsCount; ++i)
      {
         m_soundsToEnable[i]->assignChannel(m_soundDevice);
      }

      // render the sounds
      soundsCount = activeSounds.size();
      for (DWORD i = 0; i < soundsCount; ++i)
      {
         activeSounds[i]->update(listener);
      }

      // update the device - effectively presenting the sounds
      m_soundDevice.update(timeElapsed);
   }
}

///////////////////////////////////////////////////////////////////////////////
