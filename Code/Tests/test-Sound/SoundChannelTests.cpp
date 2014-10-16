#include "core-TestFramework\TestFramework.h"
#include "SoundMock.h"
#include "SoundChannelMock.h"


//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, samplesUnloadedDuringSoundRemoval)
{
   int soundLength = 2;
   int bytesPerSec = 1;
   SoundMock sound(soundLength, bytesPerSec);
   SoundChannelMock channel(sound);

   CPPUNIT_ASSERT_EQUAL(0, channel.getActiveBuffersCount());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(1, channel.getActiveBuffersCount());
   CPPUNIT_ASSERT_EQUAL(1.f, channel.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, playingSoundShorterThanSampleLength)
{
   int soundLength = 2;
   int bytesPerSec = 1;
   int soundSampleLength = 6;
   SoundMock sound(soundLength, bytesPerSec);
   SoundChannelMock channel(sound);

   channel.setSampleLength(soundSampleLength);
   CPPUNIT_ASSERT_EQUAL((DWORD)soundLength, channel.getSampleLength());

   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(2.f, channel.getPosition());

   // the sound is over - we stop playing (we stop reading from the buffer - the offset stays still)
   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(2.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(2.f, channel.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, playingSoundLongerThanSampleLength)
{
   int soundLength = 6;
   int bytesPerSec = 1;
   int soundSampleLength = 2;
   SoundMock sound(soundLength, bytesPerSec);
   SoundChannelMock channel(sound);

   channel.setSampleLength(soundSampleLength);
   CPPUNIT_ASSERT_EQUAL((DWORD)soundSampleLength, channel.getSampleLength());

   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(2.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(4.f, channel.getPosition());

   // the sound is over - we stop playing (we stop reading from the buffer - the offset stays still)
   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(6.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(6.f, channel.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, loopedPlaying)
{
   int soundLength = 6;
   int bytesPerSec = 1;
   DWORD soundSampleLength = 4;
   SoundMock sound(soundLength, bytesPerSec);
   SoundChannelMock channel(sound);

   channel.setSampleLength(soundSampleLength);
   channel.setLooped(true);

   CPPUNIT_ASSERT_EQUAL((DWORD)0, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(soundSampleLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(4.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(soundSampleLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(2.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(soundSampleLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(soundSampleLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(4.f, channel.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, loopedPlayingWithSampleLargeEnoughToEncompassManyLoops)
{
   int soundLength = 3;
   int bytesPerSec = 1;
   DWORD soundSampleLength = 8;
   SoundMock sound(soundLength, bytesPerSec);
   SoundChannelMock channel(sound);

   channel.setSampleLength(soundSampleLength);
   channel.setLooped(true);

   CPPUNIT_ASSERT_EQUAL((DWORD)0, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL((DWORD)soundLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());

   channel.loadNextSample();
   CPPUNIT_ASSERT_EQUAL((DWORD)soundLength, channel.getNumBytesToPlay());
   CPPUNIT_ASSERT_EQUAL(0.f, channel.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, soundInstancing)
{
   int soundLength = 4;
   int bytesPerSec = 1;
   DWORD soundSampleLength = 1;
   SoundMock sound(soundLength, bytesPerSec);

   SoundChannelMock channel1(sound);
   channel1.setSampleLength(soundSampleLength);
   SoundChannelMock channel2(sound);
   channel2.setSampleLength(soundSampleLength);

   CPPUNIT_ASSERT_EQUAL(0.f, channel1.getPosition());
   CPPUNIT_ASSERT_EQUAL(0.f, channel2.getPosition());

   channel1.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(1.f, channel1.getPosition());
   CPPUNIT_ASSERT_EQUAL(0.f, channel2.getPosition());

   channel2.loadNextSample();
   CPPUNIT_ASSERT_EQUAL(1.f, channel1.getPosition());
   CPPUNIT_ASSERT_EQUAL(1.f, channel2.getPosition());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundChannel, subsequentBuffersContainDifferentData)
{
  int soundLength = 5;
  int bytesPerSec = 1;
  DWORD soundSampleLength = 1;
  int numBuffers = 4;
  SoundMock sound(soundLength, bytesPerSec);

  SoundChannelMock channel(sound, numBuffers);
  channel.setSampleLength(soundSampleLength);

  channel.loadNextSample();
  channel.loadNextSample();
  channel.loadNextSample();
  channel.loadNextSample();

  CPPUNIT_ASSERT_EQUAL((int)0, (int)channel.getBufferData(0)[0]);
  CPPUNIT_ASSERT_EQUAL((int)1, (int)channel.getBufferData(1)[0]);
  CPPUNIT_ASSERT_EQUAL((int)2, (int)channel.getBufferData(2)[0]);
  CPPUNIT_ASSERT_EQUAL((int)3, (int)channel.getBufferData(3)[0]);

  channel.loadNextSample();
  CPPUNIT_ASSERT_EQUAL((int)4, (int)channel.getBufferData(0)[0]);
  CPPUNIT_ASSERT_EQUAL((int)1, (int)channel.getBufferData(1)[0]);
  CPPUNIT_ASSERT_EQUAL((int)2, (int)channel.getBufferData(2)[0]);
  CPPUNIT_ASSERT_EQUAL((int)3, (int)channel.getBufferData(3)[0]);
}

//////////////////////////////////////////////////////////////////////////////
