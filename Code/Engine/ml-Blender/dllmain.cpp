#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

// type includes
#include "core/TypesRegistry.cpp"
#include "core-MVC/TypesRegistry.cpp"
#include "core-Renderer/TypesRegistry.cpp"
#include "core-AI/TypesRegistry.cpp"
#include "core-Physics/TypesRegistry.cpp"
#include "ext-RenderingPipeline/TypesRegistry.cpp"
#include "ext-2DGameLevel/TypesRegistry.cpp"

///////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
   switch ( ul_reason_for_call )
   {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      break;
   }

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
