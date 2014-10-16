#include "core-Sound\SoundChannel.h"
#include "core-Sound\Sound.h"
#include <stdexcept>
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

SoundChannel::SoundChannel(Sound& sound, int buffersCount)
      : m_sound(sound),
      m_nextFreeBuf(0),
      m_sampleLength(0xffff),
      m_isPlaying(false),
      m_looped(false),
      m_currSoundFreq(0),
      m_currPeriodicPos(0),
      m_soundLength(0)
{
   for (int i = 0; i < buffersCount; ++i)
   {
      m_data.push_back(new char[m_sampleLength]);
   }
   
   m_currSoundFormat = m_sound.getFormat();
   m_currSoundFreq = m_sound.getFrequency();

   m_soundLength = m_sound.getLength();
   m_currPeriodicPos = 0;

   DWORD newBufSize = m_sound.getBytesPerSec();
   setSampleLength(newBufSize);
}

///////////////////////////////////////////////////////////////////////////////

SoundChannel::~SoundChannel()
{
   unsigned int count = m_data.size();
   for (unsigned int i = 0; i < count; ++i)
   {
      delete [] m_data[i];
   }
   m_data.clear();
   m_nextFreeBuf = 0;

   m_sampleLength = 0;

   m_isPlaying = false;
   m_looped = false;

   m_currSoundFormat = "";
   m_currSoundFreq = 0;
   m_currPeriodicPos = 0;
   m_soundLength = 0;
}

///////////////////////////////////////////////////////////////////////////////

Sound& SoundChannel::getSound() const
{
   return m_sound;
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::loadNextSample()
{
   DWORD len = m_sampleLength;
   DWORD offset = 0;

   char* currBuf = m_data[m_nextFreeBuf];
   m_nextFreeBuf = (m_nextFreeBuf + 1) % m_data.size();
   memset(currBuf, 0, m_sampleLength);

   DWORD bytesRead; 
   while ((len > 0) && (m_currPeriodicPos < m_soundLength))
   {
      bytesRead = m_sound.getData(m_currPeriodicPos, currBuf + offset, len);
      len -= bytesRead;
      offset += bytesRead;
      setPosition(m_currPeriodicPos + bytesRead);
   };

   if (offset > 0)
   {
      addDataToPlayBuffer(currBuf, m_sampleLength, m_currSoundFormat, m_currSoundFreq);
   }
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::play()
{
   m_isPlaying = true;
   startPlaying();
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::stop()
{
   stopPlaying();
   m_isPlaying = false;
}

///////////////////////////////////////////////////////////////////////////////

bool SoundChannel::isPlaying() const
{
   return m_isPlaying;
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::cleanBuffers()
{
   onCleanBuffers();

   int buffersCount = m_data.size();
   for (int i = 0; i < buffersCount; ++i)
   {
      char* data = m_data[i];
      delete data;
   }

   for (int i = 0; i < buffersCount; ++i)
   {
      m_data[i] = new char[m_sampleLength];
   }
   m_nextFreeBuf = 0;
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::setSampleLength(DWORD len)
{
   m_sampleLength = (len < m_soundLength) ? len : m_soundLength;
   m_sampleLength -= m_sampleLength % m_sound.getBlockAlignment();

   cleanBuffers();
}

///////////////////////////////////////////////////////////////////////////////

DWORD SoundChannel::getSampleLength() const
{
   return m_sampleLength;
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::setLooped(bool enable)
{
   m_looped = enable;
}

///////////////////////////////////////////////////////////////////////////////

float SoundChannel::getPosition() const
{
   return (float)m_currPeriodicPos / (float)m_sound.getBytesPerSec();
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::setPosition(float pos)
{
   m_currPeriodicPos = (DWORD)(pos * (float)m_sound.getBytesPerSec()) % m_soundLength;
}

///////////////////////////////////////////////////////////////////////////////

void SoundChannel::setPosition(DWORD pos)
{
   if (m_looped)
   {
      m_currPeriodicPos = pos % m_soundLength;
   }
   else
   {
      m_currPeriodicPos = pos;
      if (m_currPeriodicPos > m_soundLength) {m_currPeriodicPos = m_soundLength;}
   }
}

///////////////////////////////////////////////////////////////////////////////

float SoundChannel::getSoundLength() const
{
   return (float)m_soundLength / (float)m_sound.getBytesPerSec();
}

///////////////////////////////////////////////////////////////////////////////
