#include "GameRunner.h"
#include <windows.h>
#include "core/StringUtils.h"
#include "core/types.h"
#include "core/Assert.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////

WindowsGameRunner::WindowsGameRunner( const std::string& exePath )
   : m_exePath( exePath )
{
}

///////////////////////////////////////////////////////////////////////////////

void WindowsGameRunner::run()
{
   STARTUPINFOA startupInfo;
   memset( &startupInfo, 0, sizeof( STARTUPINFOA ) );
   startupInfo.cb = sizeof( STARTUPINFOA );

   PROCESS_INFORMATION processInfo;
   memset( &processInfo, 0, sizeof( PROCESS_INFORMATION ) );

   // get the startup directory
   std::string executionDir;
   {
      Array< std::string > pathElems;
      StringUtils::tokenize( m_exePath, "/\\", pathElems );

      uint count = pathElems.size() - 1;
      executionDir = pathElems[0];
      for ( uint i = 1; i < count; ++i )
      {
         executionDir += "/" + pathElems[i];
      }
   }

   bool wasProcessCreated = CreateProcessA( m_exePath.c_str(), NULL, NULL, NULL, false, 0, NULL, executionDir.c_str(), &startupInfo, &processInfo );
   if ( !wasProcessCreated )
   {
      DWORD errCode = GetLastError();
      ASSERT_MSG( false, "Process could not be created" );
   }
}

///////////////////////////////////////////////////////////////////////////////
