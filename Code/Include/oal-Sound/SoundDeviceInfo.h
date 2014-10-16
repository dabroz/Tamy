#pragma once

#include <vector>
#include <string>


///////////////////////////////////////////////////////////////////////////////

enum OpenALVer
{
   OALV_UNKNOWN,
   OALV_10,
   OALV_11
};

///////////////////////////////////////////////////////////////////////////////

struct SoundDeviceInfo
{
   std::string                name;
	OpenALVer                  version;
	unsigned int               sourceCount;
   std::vector<std::string>	extensions;

   SoundDeviceInfo(OpenALVer _version = OALV_UNKNOWN,
	                unsigned int _sourceCount = 0)
         : name(""),
         version(_version),
         sourceCount(_sourceCount)
   {}
};

///////////////////////////////////////////////////////////////////////////////
