/// @file   core-Sound/OggFile.h
/// @brief sound loaded from an OGG file
#pragma once

#include "core-Sound\Sound.h"
#include <windows.h>
#include <stdio.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * Sound loaded from an OGG file
 */
class OggFile : public Sound  
{
   DECLARE_ALLOCATOR( OggFile, AM_DEFAULT );

private:

public:
   OggFile(const std::string& fileName);
	~OggFile();

   DWORD getData(DWORD periodicPos, char* data, DWORD bufSize);

   std::string getFormat() const;
   unsigned int getFrequency() const;
   DWORD getLength() const;
   DWORD getBytesPerSec() const;
   DWORD getBlockAlignment() const;
};

///////////////////////////////////////////////////////////////////////////////
