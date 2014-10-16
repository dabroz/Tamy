/// @file   core-Sound/Sound.h
/// @brief  a sound representation
#pragma once

#include <string>
#include <windows.h>
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This class represents a single sound. A sound is nothing more than
 * data - on its own it can't do much. But when registered with a SoundDevice,
 * it can be played, mixed with other sounds and so on
 */
class Sound
{
private:
   std::string m_name;

public:
   Sound(const std::string& name) : m_name(name) {}
   virtual ~Sound() {}

   /**
    * This method returns the name of the sound file
    */
   const std::string& getName() const {return m_name;}

   /**
    * The method retrieves the data from the sound file to the preallocated buffer
    * from the current reading position (which can be obtained using @see getDataOffset 
    * method)
    *
    * @param periodicPos - offset in the sound from which we want to start reading data
    * @param data - preallocated buffer the data will be stored in
    * @param bufSize - size of the preallocated buffer - that's how much
    *                  data we want read from the file
    *
    * @return the actual number of bytes read from the file
    */
   virtual DWORD getData(DWORD periodicPos, char* data, DWORD bufSize) = 0;

   /** 
    * The sound format of the file
    */
   virtual std::string getFormat() const = 0;

   /**
    * The frequency with which the file should be read
    */
   virtual unsigned int getFrequency() const = 0;

   /**
    * The size of the entire sound buffer (in bytes)
    */
   virtual DWORD getLength() const = 0;

   /**
    * The average amount of bytes that should be played per second for the file
    */
   virtual DWORD getBytesPerSec() const = 0;

   /**
    * Sound data block alignment value
    */
   virtual DWORD getBlockAlignment() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
