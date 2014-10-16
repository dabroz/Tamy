#pragma once

#include "core-Sound\SoundChannel.h"
#include <list>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class SoundChannelMock : public SoundChannel
{
private:
   std::list<DWORD> m_buffers;
   bool m_hadBuffersCleaned;
   std::vector<char*> m_data;

   float m_pan;
   float m_vol;

public:
   SoundChannelMock(Sound& sound, int buffersCount = 1) 
         : SoundChannel(sound, buffersCount), m_hadBuffersCleaned(false),
         m_pan(0), m_vol(1) 
   {}

   void update() {}

   DWORD getNumBytesToPlay()
   {
      if (m_buffers.size() > 0)
      {
         return m_buffers.front();
      }
      else
      {
         return 0;
      }
   }

   void simulateBufferEnd()
   {
      m_buffers.pop_front();
      m_data.clear();
   }

   int getActiveBuffersCount() const
   {
      return (int)m_buffers.size();
   }

   bool hadBuffersCleaned()
   {
      bool result = m_hadBuffersCleaned;
      m_hadBuffersCleaned = false;
      return result;
   }

   char* getBufferData(int idx)
   {
      return m_data.at(idx);
   }

   void setPan(float pan) {m_pan = pan;}

   float getPan() const {return m_pan;}

   void setVolume(float vol) {m_vol = vol;}

   float getVolume() const {return m_vol;}

protected:
   void addDataToPlayBuffer(char* data, DWORD size, 
                            const std::string& format, DWORD freq)
   {
      m_buffers.push_back(size);
      m_data.push_back(data);
   }

   void onCleanBuffers()
   {
      m_hadBuffersCleaned = true;
      m_buffers.clear();
      m_data.clear();
   }

   void startPlaying()
   {
   }

   void stopPlaying()
   {
   }
};

///////////////////////////////////////////////////////////////////////////////
