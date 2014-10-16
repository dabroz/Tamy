#include "core-TestFramework\TestFramework.h"
#include "SoundDeviceMock.h"
#include "SoundMock.h"
#include "SoundChannelMock.h"


//////////////////////////////////////////////////////////////////////////////

TEST(SoundDevice, removingSoundThatFinishedPlaying)
{
   SoundDeviceMock soundDevice;
   int soundLength = 2;
   SoundMock sound(soundLength);

   SoundChannelMock& channel = dynamic_cast<SoundChannelMock&> (soundDevice.activateSound(sound));
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());

   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());

   channel.play();
   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundDevice, continuousPlayingOfLoopedSound)
{
   SoundDeviceMock soundDevice;
   int soundLength = 2;
   SoundMock sound(soundLength);

   SoundChannelMock& channel = dynamic_cast<SoundChannelMock&> (soundDevice.activateSound(sound));
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   channel.setLooped(true);

   channel.play();
   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);  // buffer gets reloaded
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);  // ... and so on
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.setLooped(false);
   channel.simulateBufferEnd();

   soundDevice.update(0); // unitl we turn the looping off - after that we play the sound only once more ...
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();
   soundDevice.update(0); // ... but that's it
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundDevice, stoppingTheSoundAllowsToResumeIt)
{
   SoundDeviceMock soundDevice;
   int soundLength = 2;
   SoundMock sound(soundLength);

   SoundChannelMock& channel = dynamic_cast<SoundChannelMock&> (soundDevice.activateSound(sound));
   channel.setLooped(true);


   channel.play();
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);  // buffer gets reloaded
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.stop();
   soundDevice.update(0);  // buffer gets reloaded
   CPPUNIT_ASSERT_EQUAL(false, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.play();
   soundDevice.update(0);
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundDevice, makingSureChannelDoesntRunOutOfData)
{
   int channelsCount = 1;
   int numBuffersUsed = 3;
   SoundDeviceMock soundDevice(channelsCount, numBuffersUsed);
   int soundLength = 8;
   int soundSampleLength = 2;
   SoundMock sound(soundLength);

   SoundChannelMock& channel = dynamic_cast<SoundChannelMock&> (soundDevice.activateSound(sound));
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   channel.setLooped(false);
   channel.setSampleLength(soundSampleLength);

   channel.play();
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(2, channel.getActiveBuffersCount());

   soundDevice.update(0); 
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(3, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);  // buffer gets reloaded
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(3, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);  // that was the last chunk opf data we could get - now we simply run out of data...
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(2, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);
   CPPUNIT_ASSERT_EQUAL(true, channel.isPlaying());
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());

   channel.simulateBufferEnd();

   soundDevice.update(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundDevice, releasingAllChannels)
{
   SoundDeviceMock soundDevice(2);
   SoundMock sound1;
   SoundMock sound2;
   
   SoundChannel& channel1 = soundDevice.activateSound(sound1);
   SoundChannel& channel2 = soundDevice.activateSound(sound2);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, soundDevice.getActiveChannelsCount());

   channel1.play();
   channel2.play();

   CPPUNIT_ASSERT_EQUAL(true, channel1.isPlaying());
   CPPUNIT_ASSERT_EQUAL(true, channel2.isPlaying());

   soundDevice.releaseAllChannels();
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());
}

//////////////////////////////////////////////////////////////////////////////
