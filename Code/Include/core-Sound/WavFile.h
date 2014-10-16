/// @file    core-Sound/WavFile.h
/// @brief  sound loaded from a WAV file
#pragma once

#include "core-Sound\Sound.h"
#include <windows.h>
#include <stdio.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class File;

///////////////////////////////////////////////////////////////////////////////

#define MAX_NUM_WAVEID			1024

///////////////////////////////////////////////////////////////////////////////

enum WAVEFILETYPE
{
	WF_EX  = 1,
	WF_EXT = 2
};

///////////////////////////////////////////////////////////////////////////////

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

struct WAVEFORMATEX
{
    WORD    wFormatTag;
    WORD    nChannels;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;
    WORD    wBitsPerSample;
    WORD    cbSize;
};

#endif 

///////////////////////////////////////////////////////////////////////////////

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_

struct WAVEFORMATEXTENSIBLE
{
    WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID            SubFormat;
};

#endif

///////////////////////////////////////////////////////////////////////////////

/**
 * Sound loaded from a WAV file.
 */
class WavFile : public Sound  
{
   DECLARE_ALLOCATOR( WavFile, AM_DEFAULT );

private:
   WAVEFILETYPE	m_type;
	WAVEFORMATEXTENSIBLE m_ext;		// For non-WAVEFORMATEXTENSIBLE wav-files, the header is stored in the Format member of wfEXT
	char*          m_data;
	DWORD          m_dataSize;
	File*          m_file;
	DWORD	         m_dataOffset;

   std::string m_format;

public:
   WavFile(File* file);
	~WavFile();

   DWORD getData(DWORD periodicPos, char* data, DWORD bufSize);

   std::string getFormat() const;
   unsigned int getFrequency() const;
   DWORD getLength() const;
   DWORD getBytesPerSec() const;
   DWORD getBlockAlignment() const;

private:
	void parseFile();
   std::string recognizeFormat();
};

///////////////////////////////////////////////////////////////////////////////
