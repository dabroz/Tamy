#include "core-Sound\WavFile.h"
#include "core\File.h"
#include <ks.h>
#include <ksmedia.h>
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


///////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 4)

struct WAVEFILEHEADER
{
   char			szRIFF[4];
   unsigned long	ulRIFFSize;
   char			szWAVE[4];
};

struct RIFFCHUNK
{
   char			szChunkName[4];
   unsigned long	ulChunkSize;
};

struct WAVEFMT
{
   unsigned short	usFormatTag;
   unsigned short	usChannels;
   unsigned long	ulSamplesPerSec;
   unsigned long	ulAvgBytesPerSec;
   unsigned short	usBlockAlign;
   unsigned short	usBitsPerSample;
   unsigned short	usSize;
   unsigned short  usReserved;
   unsigned long	ulChannelMask;
    GUID            guidSubFormat;
};

#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////

WavFile::WavFile(File* file)
: Sound(file->getName())
, m_data(NULL)
, m_dataSize(0)
, m_file(file)
, m_dataOffset(0)
{
   parseFile();
}

///////////////////////////////////////////////////////////////////////////////

WavFile::~WavFile()
{
   delete [] m_data;
   m_data = NULL;

   m_dataSize = 0;

   delete m_file;
   m_file = NULL;

   m_dataOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////

void WavFile::parseFile()
{
   RIFFCHUNK		riffChunk;
   WAVEFMT			waveFmt;

   // read Wave file header
   WAVEFILEHEADER	waveFileHeader;
   m_file->read((byte*)&waveFileHeader, sizeof(WAVEFILEHEADER));
   if ((_strnicmp(waveFileHeader.szRIFF, "RIFF", 4) != 0) || 
       (_strnicmp(waveFileHeader.szWAVE, "WAVE", 4) != 0))
   {
      ASSERT_MSG( false, "This is not a valid *.wav file" );
      return;
   }


   while (m_file->read((byte*)&riffChunk, sizeof(RIFFCHUNK)) == sizeof(RIFFCHUNK))
   {
      if (!_strnicmp(riffChunk.szChunkName, "fmt ", 4))
      {
         if (riffChunk.ulChunkSize <= sizeof(WAVEFMT))
         {
            m_file->read((byte*)&waveFmt, riffChunk.ulChunkSize);
         
            // determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
            if (waveFmt.usFormatTag == WAVE_FORMAT_PCM)
            {
               m_type = WF_EX;
               memcpy(&m_ext.Format, &waveFmt, sizeof(PCMWAVEFORMAT));
            }
            else if (waveFmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)
            {
               m_type = WF_EXT;
               memcpy(&m_ext, &waveFmt, sizeof(WAVEFORMATEXTENSIBLE));
            }
         }
         else
         {
            m_file->seek(riffChunk.ulChunkSize, std::ios_base::cur);
         }
      }
      else if (!_strnicmp(riffChunk.szChunkName, "data", 4))
      {
         m_dataSize = riffChunk.ulChunkSize;
         m_dataOffset = m_file->tell();
         m_file->seek(riffChunk.ulChunkSize, std::ios_base::cur);
      }
      else
      {
         m_file->seek(riffChunk.ulChunkSize, std::ios_base::cur);
      }

      // ensure that we are correctly aligned for next chunk
      if (riffChunk.ulChunkSize & 1)
      {
         m_file->seek(1, std::ios_base::cur);
      }
   }

   if ((m_dataSize == 0) || (m_dataOffset == 0) || ((m_type != WF_EX) && (m_type != WF_EXT)))
   {
      std::string fileName = m_file->getName();
      delete m_file; m_file = NULL;
      ASSERT_MSG( false, "Error occured during parsing" );
      return;
   }


   // recognize the data format
   m_format = recognizeFormat();

   m_file->seek(m_dataOffset, std::ios_base::beg);
}

///////////////////////////////////////////////////////////////////////////////

std::string WavFile::recognizeFormat()
{
   if (m_type == WF_EX)
   {
      if (m_ext.Format.nChannels == 1)
      {
         if      (m_ext.Format.wBitsPerSample == 16) {return "AL_FORMAT_MONO16";}
         else if (m_ext.Format.wBitsPerSample == 8) {return "AL_FORMAT_MONO8";}
      }
      else if (m_ext.Format.nChannels == 2)
      {
         if      (m_ext.Format.wBitsPerSample == 16) {return "AL_FORMAT_STEREO16";}
         else if (m_ext.Format.wBitsPerSample == 8) {return "AL_FORMAT_STEREO8";}
      }
      else if ((m_ext.Format.nChannels >= 4) && (m_ext.Format.wBitsPerSample == 16))
      {
         return "AL_FORMAT_QUAD16";
      }
   }
   else if (m_type == WF_EXT)
   {
      if ((m_ext.Format.nChannels == 1) && (m_ext.dwChannelMask == SPEAKER_FRONT_CENTER))
      {
         if      (m_ext.Format.wBitsPerSample == 16) {return "AL_FORMAT_MONO16";}
         else if (m_ext.Format.wBitsPerSample == 8) {return "AL_FORMAT_MONO8";}
      }
      else if ((m_ext.Format.nChannels == 2) && (m_ext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)))
      {
         if      (m_ext.Format.wBitsPerSample == 16) {return "AL_FORMAT_STEREO16";}
         else if (m_ext.Format.wBitsPerSample == 8) {return "AL_FORMAT_STEREO8";}
      }
      else if ((m_ext.Format.nChannels == 2) && (m_ext.Format.wBitsPerSample == 16) && (m_ext.dwChannelMask == (SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
      {
         return  "AL_FORMAT_REAR16";
      }
      else if ((m_ext.Format.nChannels == 4) && (m_ext.Format.wBitsPerSample == 16) && (m_ext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
      {
         return "AL_FORMAT_QUAD16";
      }
      else if ((m_ext.Format.nChannels == 6) && (m_ext.Format.wBitsPerSample == 16) && (m_ext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
      {
         return "AL_FORMAT_51CHN16";
      }
      else if ((m_ext.Format.nChannels == 7) && (m_ext.Format.wBitsPerSample == 16) && (m_ext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_BACK_CENTER)))
      {
         return "AL_FORMAT_61CHN16";
      }
      else if ((m_ext.Format.nChannels == 8) && (m_ext.Format.wBitsPerSample == 16) && (m_ext.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT)))
      {
         return "AL_FORMAT_71CHN16";
      }
   }

   return "AL_FORMAT_UNKNOWN";
}

///////////////////////////////////////////////////////////////////////////////

DWORD WavFile::getData(DWORD periodicPos, char* data, DWORD bufSize)
{
   if (periodicPos > m_dataSize)
   {
      ASSERT_MSG( false, "Trying to move past the end of the sound file" );
      return 0;
   }
   m_file->seek(periodicPos + m_dataOffset, std::ios_base::beg);

   DWORD leftToRead = m_dataSize - periodicPos;
   if (leftToRead == 0) {return 0;}

   if (leftToRead > bufSize) {leftToRead = bufSize;}

   DWORD bytesRead = (DWORD)m_file->read((byte*)data, leftToRead);
   return bytesRead;
}

///////////////////////////////////////////////////////////////////////////////

std::string WavFile::getFormat() const
{
   return m_format;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int WavFile::getFrequency() const
{
   return m_ext.Format.nSamplesPerSec;
}

///////////////////////////////////////////////////////////////////////////////

DWORD WavFile::getLength() const
{
   return m_dataSize;
}

///////////////////////////////////////////////////////////////////////////////

DWORD WavFile::getBytesPerSec() const
{
   return m_ext.Format.nAvgBytesPerSec;
}

///////////////////////////////////////////////////////////////////////////////

DWORD WavFile::getBlockAlignment() const
{
   return m_ext.Format.nBlockAlign;
}

///////////////////////////////////////////////////////////////////////////////
