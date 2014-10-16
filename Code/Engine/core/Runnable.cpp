#include "core\Runnable.h"
#include "core\Thread.h"
#include "core\ThreadSystem.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

Runnable::Runnable()
   : m_hostThread( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Runnable::~Runnable()
{
}

///////////////////////////////////////////////////////////////////////////////

void Runnable::execute( Thread* hostThread, uint stackSize, ulong& outThreadId, void*& outThreadHandle )
{
   m_hostThread = hostThread;

   outThreadHandle = CreateThread( NULL, stackSize, (LPTHREAD_START_ROUTINE)threadMain, this, 0, (DWORD*)&outThreadId );
}

///////////////////////////////////////////////////////////////////////////////

void Runnable::threadMain( void* contextData )
{
   Runnable* runnable = (Runnable*)contextData;
   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();

   // first - report that a new thread of execution has been created
   threadSystem.registerThread( *runnable->m_hostThread );

   // run thread's functionality
   runnable->run();

   // unregister the thread of execution
   threadSystem.unregisterThread();

}

///////////////////////////////////////////////////////////////////////////////
