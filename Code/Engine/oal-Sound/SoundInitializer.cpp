#include "oal-Sound\SoundInitializer.h"
#include "oal-Sound\OpenALSoundSystem.h"
#include "oal-Sound\SoundDeviceInfo.h"
#include <cassert>


///////////////////////////////////////////////////////////////////////////////

SoundInitializer::SoundInitializer(OpenALSoundSystem& soundSystem)
      : m_soundSystem(soundSystem)
{
   char *devicesArr = NULL;

   if(soundSystem.alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
   {
      devicesArr = (char *)soundSystem.alcGetString(NULL, ALC_DEVICE_SPECIFIER);
      enumDevices(devicesArr);
   }

   if(soundSystem.alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT"))
   {
      devicesArr = (char *)soundSystem.alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
      enumDevices(devicesArr);
   }

   for (unsigned int i = 0; i < m_devices.size(); ++i)
   {
      getDeviceInfo(m_devices[i]);
   }

   if (m_devices.size() == 0)
   {
      throw std::runtime_error("There are no sound devices available");
   }
}

///////////////////////////////////////////////////////////////////////////////

void SoundInitializer::enumDevices(char* devicesArr)
{
   while (*devicesArr != NULL) 
   {
      SoundDeviceInfo device;
      device.name = devicesArr;

      bool similarFound = false;
      for (unsigned int i = 0; i < m_devices.size(); ++i)
      {
         if (m_devices[i].name == device.name)
         {
            similarFound = true;
            break;
         }
      }

      ALCdevice* pDevice = m_soundSystem.alcOpenDevice(device.name.c_str());
      bool canBeOpened = false;
      if (pDevice != NULL)
      {
         canBeOpened = true;
         m_soundSystem.alcCloseDevice(pDevice);
      }

      if ((similarFound == false) && (canBeOpened == true))
      {
         m_devices.push_back(device);
      }

      devicesArr += strlen(devicesArr) + 1;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SoundInitializer::getDeviceInfo(SoundDeviceInfo& device)
{
   ALCdevice* pDevice = m_soundSystem.alcOpenDevice(device.name.c_str());
   assert (pDevice != NULL);

   // get the version of the software
   int majorVersion, minorVersion;
   m_soundSystem.alcGetIntegerv(pDevice, ALC_MAJOR_VERSION, sizeof(int), &majorVersion);
   m_soundSystem.alcGetIntegerv(pDevice, ALC_MINOR_VERSION, sizeof(int), &minorVersion);
   if ((majorVersion == 1) && (minorVersion == 0))
   {
      device.version = OALV_10;
   }
   else if ((majorVersion > 1) || ((majorVersion == 1) && (minorVersion >= 1)))
   {
      device.version = OALV_11;
   }
   else
   {
      device.version = OALV_UNKNOWN;
   }

   // recognize the available extensions
   std::vector<std::string> allExtensions;
   allExtensions.push_back("ALC_EXT_CAPTURE");
   allExtensions.push_back("ALC_EXT_EFX");
   allExtensions.push_back("AL_EXT_OFFSET");
   allExtensions.push_back("AL_EXT_LINEAR_DISTANCE");
   allExtensions.push_back("AL_EXT_EXPONENT_DISTANCE");
   allExtensions.push_back("EAX2.0");
   allExtensions.push_back("EAX3.0");
   allExtensions.push_back("EAX4.0");
   allExtensions.push_back("EAX5.0");
   allExtensions.push_back("EAX-RAM");

   for (unsigned int i = 0; i < allExtensions.size(); ++i)
   {
      const std::string& extName = allExtensions[i];
      if (m_soundSystem.alcIsExtensionPresent(pDevice, extName.c_str()) == AL_TRUE)
         device.extensions.push_back(extName);
   }

   ALCcontext* context = m_soundSystem.alcCreateContext(pDevice, NULL);
   if (pDevice == NULL)
   {
	   throw std::runtime_error(std::string("Cannot create a context for sound device"));
   }
	m_soundSystem.alcMakeContextCurrent(context);

   // fin out how many sources we can simultaneously create
   device.sourceCount = getMaxNumSources();

   m_soundSystem.alcMakeContextCurrent(NULL);
	m_soundSystem.alcDestroyContext(context);
   m_soundSystem.alcCloseDevice(pDevice);
}

///////////////////////////////////////////////////////////////////////////////

unsigned int SoundInitializer::getMaxNumSources() const
{
	ALuint tmpSources[256];
	unsigned int sourceCount = 0;
   memset(tmpSources, 0, sizeof(ALuint) * 256);

	// clear AL error code
	m_soundSystem.alGetError();

	// generate up to 256 sources, checking for any errors
	for (sourceCount = 0; sourceCount < 256; sourceCount++)
	{
		m_soundSystem.alGenSources(1, &tmpSources[sourceCount]);
		if (m_soundSystem.alGetError() != AL_NO_ERROR)
			break;
	}

	// release the sources
   m_soundSystem.alDeleteSources(sourceCount, tmpSources);
	if (m_soundSystem.alGetError() != AL_NO_ERROR)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			m_soundSystem.alDeleteSources(1, &tmpSources[i]);
		}
	}

	return sourceCount;
}

///////////////////////////////////////////////////////////////////////////////

const SoundDeviceInfo& SoundInitializer::findBest(const SoundDeviceInfo& ideal,
                                                  int sourceCountImportance,
                                                  int versionImportance,
                                                  int extensionsImportance) const
{
   const SoundDeviceInfo* bestDevice = NULL;
   int score = 0;

   for (std::vector<SoundDeviceInfo>::const_iterator it = m_devices.begin();
       it != m_devices.end(); ++it)
   {
      int thisScore = scoreDevice(*it, ideal, 
                                  sourceCountImportance, 
                                  versionImportance, 
                                  extensionsImportance);
      if (thisScore > score)
      {
         bestDevice = &(*it);
         score = thisScore;
      }
   }

   return *bestDevice;
}

///////////////////////////////////////////////////////////////////////////////

int SoundInitializer::scoreDevice(const SoundDeviceInfo& scoredDevice, 
                                  const SoundDeviceInfo& idealDevice,
                                  int sourceCountImportance,
                                  int versionImportance,
                                  int extensionsImportance) const
{
   int score = 0;

   score = (scoredDevice.sourceCount - idealDevice.sourceCount) * sourceCountImportance;
   score += (scoredDevice.version - idealDevice.version) * versionImportance;

   unsigned int idealExtensionsCount = idealDevice.extensions.size();
   unsigned int devExtensionsCount = scoredDevice.extensions.size();
   int numExtensionsNotSupported = idealExtensionsCount;
   for (unsigned int i = 0; i < idealExtensionsCount; ++i)
   {
      for (unsigned int j = 0; j < devExtensionsCount; ++j)
      {
         if (scoredDevice.extensions[j] == idealDevice.extensions[i])
         {
            numExtensionsNotSupported--;
            break;
         }
      }
   }
   score += (idealExtensionsCount - numExtensionsNotSupported) * extensionsImportance;

   return score;
}

///////////////////////////////////////////////////////////////////////////////
