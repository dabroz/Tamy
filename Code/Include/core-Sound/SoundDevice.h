/// @file   core-Sound/SoundDevice.h
/// @brief  device sounds are being played on
#pragma once

#include <vector>
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class Sound;
class SoundChannel;

///////////////////////////////////////////////////////////////////////////////

/**
 * This interface represents a sound device - capable of playing, mixing
 * and doing other low level stuff with the sound
 */
class SoundDevice
{
   DECLARE_ALLOCATOR( SoundDevice, AM_DEFAULT );

private:
   int m_numBuffersUsed;
   std::vector<SoundChannel*> m_activeChannels;

public:
   SoundDevice(int numBuffersUsed = 1);
   virtual ~SoundDevice();

   unsigned int getActiveChannelsCount() const {return m_activeChannels.size();}

   SoundChannel& getChannel(unsigned int idx);

   /**
    * The method returns the number describing how many channels
    * can be used by sounds
    */
   virtual int getChannelsCount() const = 0;

   /**
    * The method assigns a sound to a free channel, allowing it to be played
    */
   SoundChannel& activateSound(Sound& sound);

   /**
    * This method frees a channel from the channel pool
    */
   void deactivateSound(SoundChannel* channel);

   /**
    * The method releases all currently occupied channels
    * on the device, effectively stopping all the sounds being played at the moment
    */
   void releaseAllChannels();

   /**
    * The method returns a list of all currently occupied channels
    */
   const std::vector<SoundChannel*>& getActiveChannels() const {return m_activeChannels;}

   /**
    * Call this method in the main program loop to ensure continuous
    * sound and proper channels management
    */
   void update(float timeElapsed);

protected:
   /**
    * This method retrieves a sound channed
    */
   virtual SoundChannel* createChannel(Sound& sound, int buffersCount) = 0;
};

///////////////////////////////////////////////////////////////////////////////
