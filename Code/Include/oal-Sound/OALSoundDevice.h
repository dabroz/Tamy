#pragma once

#include "core-Sound\SoundDevice.h"
#include "oal-Sound\SoundDeviceInfo.h"
#include <al.h>
#include <alc.h>


///////////////////////////////////////////////////////////////////////////////

class OpenALSoundSystem;
class OALSoundChannel;

///////////////////////////////////////////////////////////////////////////////

class OALSoundDevice : public SoundDevice
{
private:
   SoundDeviceInfo m_deviceDesc;
   OpenALSoundSystem& m_soundSystem;
   ALCdevice* m_device;
   ALCcontext* m_context;

public:
   OALSoundDevice(const SoundDeviceInfo& deviceDesc,
                  OpenALSoundSystem& soundSystem,
                  int numBuffersUsed = 1);
   ~OALSoundDevice();

   int getChannelsCount() const;

protected:
   SoundChannel* createChannel(Sound& sound, int buffersCount);
};

///////////////////////////////////////////////////////////////////////////////
