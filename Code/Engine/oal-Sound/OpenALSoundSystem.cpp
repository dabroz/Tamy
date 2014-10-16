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

#include "oal-Sound\OpenALSoundSystem.h"
#include "oal-Sound\SoundDeviceInfo.h"
#include "oal-Sound\OALSoundDevice.h"
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////

OpenALSoundSystem::OpenALSoundSystem(const std::string& openAlDllFile)
{

	m_openALDLL = LoadLibrary(openAlDllFile.c_str());
	
	if (!m_openALDLL)
   {
      throw std::runtime_error(std::string("Unable to open a dynamic library ") + openAlDllFile);
   }

	// get function pointers from DLL
	alEnable = (LPALENABLE)GetProcAddress(m_openALDLL, "alEnable");
	if (alEnable == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alEnable' function address");
	}
	alDisable = (LPALDISABLE)GetProcAddress(m_openALDLL, "alDisable");
	if (alDisable == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDisable' function address");
	}
	alIsEnabled = (LPALISENABLED)GetProcAddress(m_openALDLL, "alIsEnabled");
	if (alIsEnabled == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alIsEnabled' function address");
	}
	alGetBoolean = (LPALGETBOOLEAN)GetProcAddress(m_openALDLL, "alGetBoolean");
	if (alGetBoolean == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetBoolean' function address");	
	}
	alGetInteger = (LPALGETINTEGER)GetProcAddress(m_openALDLL, "alGetInteger");
	if (alGetInteger == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetInteger' function address");
	}
	alGetFloat = (LPALGETFLOAT)GetProcAddress(m_openALDLL, "alGetFloat");
	if (alGetFloat == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetFloat' function address");
	}
	alGetDouble = (LPALGETDOUBLE)GetProcAddress(m_openALDLL, "alGetDouble");
	if (alGetDouble == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetDouble' function address");
	}
	alGetBooleanv = (LPALGETBOOLEANV)GetProcAddress(m_openALDLL, "alGetBooleanv");
	if (alGetBooleanv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetBooleanv' function address");
	}
	alGetIntegerv = (LPALGETINTEGERV)GetProcAddress(m_openALDLL, "alGetIntegerv");
	if (alGetIntegerv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetIntegerv' function address");
	}
	alGetFloatv = (LPALGETFLOATV)GetProcAddress(m_openALDLL, "alGetFloatv");
	if (alGetFloatv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetFloatv' function address");
	}
	alGetDoublev = (LPALGETDOUBLEV)GetProcAddress(m_openALDLL, "alGetDoublev");
	if (alGetDoublev == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetDoublev' function address");
	}
	alGetString = (LPALGETSTRING)GetProcAddress(m_openALDLL, "alGetString");
	if (alGetString == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetString' function address");
	}
	alGetError = (LPALGETERROR)GetProcAddress(m_openALDLL, "alGetError");
	if (alGetError == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetError' function address");
	}
	alIsExtensionPresent = (LPALISEXTENSIONPRESENT)GetProcAddress(m_openALDLL, "alIsExtensionPresent");
	if (alIsExtensionPresent == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alIsExtensionPresent' function address");
	}
	alGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(m_openALDLL, "alGetProcAddress");
	if (alGetProcAddress == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetProcAddress' function address");
	}
	alGetEnumValue = (LPALGETENUMVALUE)GetProcAddress(m_openALDLL, "alGetEnumValue");
	if (alGetEnumValue == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetEnumValue' function address");
	}
	alListeneri = (LPALLISTENERI)GetProcAddress(m_openALDLL, "alListeneri");
	if (alListeneri == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alListeneri' function address");
	}
	alListenerf = (LPALLISTENERF)GetProcAddress(m_openALDLL, "alListenerf");
	if (alListenerf == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alListenerf' function address");
	}
	alListener3f = (LPALLISTENER3F)GetProcAddress(m_openALDLL, "alListener3f");
	if (alListener3f == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alListener3f' function address");
	}
	alListenerfv = (LPALLISTENERFV)GetProcAddress(m_openALDLL, "alListenerfv");
	if (alListenerfv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alListenerfv' function address");
	}
	alGetListeneri = (LPALGETLISTENERI)GetProcAddress(m_openALDLL, "alGetListeneri");
	if (alGetListeneri == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetListeneri' function address");
	}
	alGetListenerf =(LPALGETLISTENERF)GetProcAddress(m_openALDLL, "alGetListenerf");
	if (alGetListenerf == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetListenerf' function address");
	}
	alGetListener3f = (LPALGETLISTENER3F)GetProcAddress(m_openALDLL, "alGetListener3f");
	if (alGetListener3f == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetListener3f' function address");
	}
	alGetListenerfv = (LPALGETLISTENERFV)GetProcAddress(m_openALDLL, "alGetListenerfv");
	if (alGetListenerfv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetListenerfv' function address");
	}
	alGenSources = (LPALGENSOURCES)GetProcAddress(m_openALDLL, "alGenSources");
	if (alGenSources == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGenSources' function address");
	}
	alDeleteSources = (LPALDELETESOURCES)GetProcAddress(m_openALDLL, "alDeleteSources");
	if (alDeleteSources == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDeleteSources' function address");
	}
	alIsSource = (LPALISSOURCE)GetProcAddress(m_openALDLL, "alIsSource");
	if (alIsSource == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alIsSource' function address");
	}
	alSourcei = (LPALSOURCEI)GetProcAddress(m_openALDLL, "alSourcei");
	if (alSourcei == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcei' function address");
	}
	alSourcef = (LPALSOURCEF)GetProcAddress(m_openALDLL, "alSourcef");
	if (alSourcef == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcef' function address");
	}
	alSource3f = (LPALSOURCE3F)GetProcAddress(m_openALDLL, "alSource3f");
	if (alSource3f == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSource3f' function address");
	}
	alSourcefv = (LPALSOURCEFV)GetProcAddress(m_openALDLL, "alSourcefv");
	if (alSourcefv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcefv' function address");
	}
	alGetSourcei = (LPALGETSOURCEI)GetProcAddress(m_openALDLL, "alGetSourcei");
	if (alGetSourcei == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetSourcei' function address");
	}
	alGetSourcef = (LPALGETSOURCEF)GetProcAddress(m_openALDLL, "alGetSourcef");
	if (alGetSourcef == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetSourcef' function address");
	}
	alGetSourcefv = (LPALGETSOURCEFV)GetProcAddress(m_openALDLL, "alGetSourcefv");
	if (alGetSourcefv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetSourcefv' function address");
	}
	alSourcePlayv = (LPALSOURCEPLAYV)GetProcAddress(m_openALDLL, "alSourcePlayv");
	if (alSourcePlayv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcePlayv' function address");
	}
	alSourceStopv = (LPALSOURCESTOPV)GetProcAddress(m_openALDLL, "alSourceStopv");
	if (alSourceStopv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourceStopv' function address");
	}
	alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(m_openALDLL, "alSourcePlay");
	if (alSourcePlay == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcePlay' function address");
	}
	alSourcePause = (LPALSOURCEPAUSE)GetProcAddress(m_openALDLL, "alSourcePause");
	if (alSourcePause == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourcePause' function address");
	}
	alSourceStop = (LPALSOURCESTOP)GetProcAddress(m_openALDLL, "alSourceStop");
	if (alSourceStop == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourceStop' function address");
	}
	alGenBuffers = (LPALGENBUFFERS)GetProcAddress(m_openALDLL, "alGenBuffers");
	if (alGenBuffers == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGenBuffers' function address");
	}
	alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(m_openALDLL, "alDeleteBuffers");
	if (alDeleteBuffers == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDeleteBuffers' function address");
	}
	alIsBuffer = (LPALISBUFFER)GetProcAddress(m_openALDLL, "alIsBuffer");
	if (alIsBuffer == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alIsBuffer' function address");
	}
	alBufferData = (LPALBUFFERDATA)GetProcAddress(m_openALDLL, "alBufferData");
	if (alBufferData == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alBufferData' function address");
	}
	alGetBufferi = (LPALGETBUFFERI)GetProcAddress(m_openALDLL, "alGetBufferi");
	if (alGetBufferi == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetBufferi' function address");
	}
	alGetBufferf = (LPALGETBUFFERF)GetProcAddress(m_openALDLL, "alGetBufferf");
	if (alGetBufferf == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alGetBufferf' function address");
	}
	alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(m_openALDLL, "alSourceQueueBuffers");
	if (alSourceQueueBuffers == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourceQueueBuffers' function address");
	}
	alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(m_openALDLL, "alSourceUnqueueBuffers");
	if (alSourceUnqueueBuffers == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alSourceUnqueueBuffers' function address");
	}
	alDistanceModel = (LPALDISTANCEMODEL)GetProcAddress(m_openALDLL, "alDistanceModel");
	if (alDistanceModel == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDistanceModel' function address");
	}
	alDopplerFactor = (LPALDOPPLERFACTOR)GetProcAddress(m_openALDLL, "alDopplerFactor");
	if (alDopplerFactor == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDopplerFactor' function address");
	}
	alDopplerVelocity = (LPALDOPPLERVELOCITY)GetProcAddress(m_openALDLL, "alDopplerVelocity");
	if (alDopplerVelocity == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alDopplerVelocity' function address");
	}
	alcGetString = (LPALCGETSTRING)GetProcAddress(m_openALDLL, "alcGetString");
	if (alcGetString == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetString' function address");
	}
	alcGetIntegerv = (LPALCGETINTEGERV)GetProcAddress(m_openALDLL, "alcGetIntegerv");
	if (alcGetIntegerv == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetIntegerv' function address");
	}
	alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(m_openALDLL, "alcOpenDevice");
	if (alcOpenDevice == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcOpenDevice' function address");
	}
	alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(m_openALDLL, "alcCloseDevice");
	if (alcCloseDevice == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcCloseDevice' function address");
	}
	alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(m_openALDLL, "alcCreateContext");
	if (alcCreateContext == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcCreateContext' function address");
	}
	alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(m_openALDLL, "alcMakeContextCurrent");
	if (alcMakeContextCurrent == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcMakeContextCurrent' function address");
	}
	alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(m_openALDLL, "alcProcessContext");
	if (alcProcessContext == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcProcessContext' function address");
	}
	alcGetCurrentContext = (LPALCGETCURRENTCONTEXT)GetProcAddress(m_openALDLL, "alcGetCurrentContext");
	if (alcGetCurrentContext == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetCurrentContext' function address");
	}
	alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(m_openALDLL, "alcGetContextsDevice");
	if (alcGetContextsDevice == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetContextsDevice' function address");
	}
	alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(m_openALDLL, "alcSuspendContext");
	if (alcSuspendContext == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcSuspendContext' function address");
	}
	alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(m_openALDLL, "alcDestroyContext");
	if (alcDestroyContext == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcDestroyContext' function address");
	}
	alcGetError = (LPALCGETERROR)GetProcAddress(m_openALDLL, "alcGetError");
	if (alcGetError == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetError' function address");
	}
	alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT)GetProcAddress(m_openALDLL, "alcIsExtensionPresent");
	if (alcIsExtensionPresent == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcIsExtensionPresent' function address");
	}
	alcGetProcAddress = (LPALCGETPROCADDRESS)GetProcAddress(m_openALDLL, "alcGetProcAddress");
	if (alcGetProcAddress == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetProcAddress' function address");
	}
	alcGetEnumValue = (LPALCGETENUMVALUE)GetProcAddress(m_openALDLL, "alcGetEnumValue");
	if (alcGetEnumValue == NULL)
	{
		throw std::runtime_error("Failed to retrieve 'alcGetEnumValue' function address");
	}
}

///////////////////////////////////////////////////////////////////////////////

OpenALSoundSystem::~OpenALSoundSystem()
{
	// unload the dll
	if (m_openALDLL)
	{
		FreeLibrary(m_openALDLL);
		m_openALDLL = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////

OALSoundDevice* OpenALSoundSystem::createDevice(const SoundDeviceInfo& deviceDesc)
{
   if (deviceDesc.name.length() == 0)
   {
      throw std::invalid_argument("Invalid SoundDeviceInfo");
   }

   return new OALSoundDevice(deviceDesc, *this, 8);
}

///////////////////////////////////////////////////////////////////////////////
