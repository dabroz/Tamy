#include "core-Sound\SoundDevice.h"
#include "core-Sound\Sound.h"
#include "core-Sound\SoundChannel.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

SoundDevice::SoundDevice(int numBuffersUsed)
      : m_numBuffersUsed(numBuffersUsed)
{
}

///////////////////////////////////////////////////////////////////////////////

SoundDevice::~SoundDevice() 
{
   releaseAllChannels();
}

///////////////////////////////////////////////////////////////////////////////

SoundChannel& SoundDevice::getChannel(unsigned int idx)
{
   return *(m_activeChannels.at(idx));
}

///////////////////////////////////////////////////////////////////////////////

SoundChannel& SoundDevice::activateSound(Sound& sound)
{
   if (getChannelsCount() == m_activeChannels.size())
   {
      ASSERT_MSG( false, "All sound channels are currently occupied" );
   }

   SoundChannel* channel = createChannel(sound, m_numBuffersUsed);
   m_activeChannels.push_back(channel);
   return *channel;
}

///////////////////////////////////////////////////////////////////////////////

void SoundDevice::deactivateSound( SoundChannel* channel )
{
   for ( std::vector<SoundChannel*>::iterator it = m_activeChannels.begin(); it != m_activeChannels.end(); ++it )
   {
      if ( *it == channel )
      {
         m_activeChannels.erase(it);
         break;
      }
   }

   channel->stop();
   delete channel;
}

///////////////////////////////////////////////////////////////////////////////

void SoundDevice::releaseAllChannels()
{
   while(m_activeChannels.size() > 0)
   {
      SoundChannel* channel = m_activeChannels.back();
      channel->stop();
      delete channel;
      m_activeChannels.pop_back();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SoundDevice::update(float timeElapsed)
{
   std::vector<SoundChannel*>::iterator it = m_activeChannels.begin();
   while(it != m_activeChannels.end())
   {
      SoundChannel* channel = *it;
      if (channel->isPlaying() == false) 
      {
         ++it; 
         continue;
      }

      channel->update();
      if (channel->getActiveBuffersCount() < m_numBuffersUsed) 
      {
         channel->loadNextSample();
      }
      if (channel->getActiveBuffersCount() == 0)
      {
         channel->stop();
         it = m_activeChannels.erase(it);
         delete channel;
      }
      else
      {
         ++it;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
