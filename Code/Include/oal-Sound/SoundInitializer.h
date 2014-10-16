#pragma once

#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class OpenALSoundSystem;
struct SoundDeviceInfo;

///////////////////////////////////////////////////////////////////////////////

/** 
 * This class determines what sound devices are present on the machine
 * and allows the user to choose the most suitable one
 */
class SoundInitializer
{
private:
   OpenALSoundSystem& m_soundSystem;
   std::vector<SoundDeviceInfo> m_devices;

public:
   SoundInitializer(OpenALSoundSystem& soundSystem);

   /**
    * The method returns all the sound devices present on the machine
    */
   const std::vector<SoundDeviceInfo>& getAllDevices() const {return m_devices;}

   /**
    * The method allows to find the most suitable device.
    * It uses the 'ideal' sound device description and three weights
    * specifying which details of the ideal description matter to us the most
    */
   const SoundDeviceInfo& findBest(const SoundDeviceInfo& ideal,
                                   int sourceCountImportance = 1,
                                   int versionImportance = 0,
                                   int extensionsImportance = 1) const;

private:
   void enumDevices(char* devicesArr);
   void getDeviceInfo(SoundDeviceInfo& device);
   unsigned int getMaxNumSources() const;

   int scoreDevice(const SoundDeviceInfo& scoredDevice, 
                   const SoundDeviceInfo& idealDevice,
                   int sourceCountImportance,
                   int versionImportance,
                   int extensionsImportance) const;
};

///////////////////////////////////////////////////////////////////////////////
