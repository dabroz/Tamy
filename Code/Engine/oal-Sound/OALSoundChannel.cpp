#include "oal-Sound\OALSoundChannel.h"
#include "oal-Sound\OpenALSoundSystem.h"
#include <stdexcept>
#include <cassert>


///////////////////////////////////////////////////////////////////////////////

OALSoundChannel::OALSoundChannel(Sound& sound, 
                                 OpenALSoundSystem& soundSystem, 
                                 int numBuffersUsed)
      : SoundChannel(sound, numBuffersUsed),
      m_soundSystem(soundSystem),
      m_buffersCount(numBuffersUsed),
      m_oalSource(0)
{
   m_oalBuffer = new ALuint[m_buffersCount];
   m_soundSystem.alGenBuffers(m_buffersCount, m_oalBuffer);
   m_soundSystem.alGenSources(1, &m_oalSource);

   if (m_oalSource == 0)
   {
      throw std::runtime_error("Cannot create a sound channel");
   }

   for (int i = 0; i < m_buffersCount; ++i)
   {
      m_freeBuffers.push_back(m_oalBuffer[i]);
   }
}

///////////////////////////////////////////////////////////////////////////////

OALSoundChannel::~OALSoundChannel()
{
   stop();
   m_soundSystem.alSourcei(m_oalSource, AL_BUFFER, 0);
   m_soundSystem.alDeleteSources(1, &m_oalSource);
	m_soundSystem.alDeleteBuffers(m_buffersCount, m_oalBuffer);

   delete [] m_oalBuffer;
   m_oalBuffer = NULL;

   m_buffersCount = 0;

   m_freeBuffers.clear();
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundChannel::update()
{
   // check how many buffers can 
   int buffersProcessed = 0;
   m_soundSystem.alGetSourcei(m_oalSource, AL_BUFFERS_PROCESSED, &buffersProcessed);

   ALuint buffer = 0;
   for (int i = 0; i < buffersProcessed; ++i)
   {
      m_soundSystem.alSourceUnqueueBuffers(m_oalSource, 1, &buffer);

      m_freeBuffers.push_back(buffer);
   }
}

///////////////////////////////////////////////////////////////////////////////

int OALSoundChannel::getActiveBuffersCount() const 
{
   return m_buffersCount - m_freeBuffers.size();
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundChannel::addDataToPlayBuffer(char* data, DWORD size, 
                                          const std::string& format, 
                                          DWORD freq)
{
   if (m_freeBuffers.size() == 0)
   {
      throw std::runtime_error("No more free buffers");
   }

   ALuint buffer = m_freeBuffers.front();
   m_freeBuffers.pop_front();
      
   m_soundSystem.alBufferData(buffer, alGetEnumValue(format.c_str()), 
                              data, size, freq);

	m_soundSystem.alSourceQueueBuffers(m_oalSource, 1, &buffer);

   m_soundSystem.alSourcePlay(m_oalSource);
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundChannel::onCleanBuffers()
{
   ALuint buf;
   int activeBufsCount = getActiveBuffersCount();
   while (activeBufsCount > 0)
   {
      m_soundSystem.alSourceUnqueueBuffers(m_oalSource, 1, &buf);
      activeBufsCount--;
   }

   m_soundSystem.alDeleteBuffers(m_buffersCount, m_oalBuffer);
   m_soundSystem.alGenBuffers(m_buffersCount, m_oalBuffer);

   m_freeBuffers.clear();
   for (int i = 0; i < m_buffersCount; ++i)
   {
      m_freeBuffers.push_back(m_oalBuffer[i]);
   }
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundChannel::startPlaying()
{
   m_soundSystem.alSourcePlay(m_oalSource);
}

///////////////////////////////////////////////////////////////////////////////

void OALSoundChannel::stopPlaying()
{
   m_soundSystem.alSourceStop(m_oalSource);
}

///////////////////////////////////////////////////////////////////////////////
