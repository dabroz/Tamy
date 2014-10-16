#pragma once

#include "core-Sound\SoundDevice.h"
#include "SoundChannelMock.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class SoundDeviceMock : public SoundDevice
{
private:
   int m_channelsCount;

public:
   SoundDeviceMock(int channelsCount = 1, int numBuffersUsed = 1) 
      : SoundDevice(numBuffersUsed),
      m_channelsCount(channelsCount)
   {
   }

   int getChannelsCount() const
   {
      return m_channelsCount;
   }

protected:
   SoundChannel* createChannel(Sound& sound, int buffersCount)
   {
      return new SoundChannelMock(sound, buffersCount);
   }
};

///////////////////////////////////////////////////////////////////////////////
