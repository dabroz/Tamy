#include "AssertHandler.h"
#include "EventAssert.h"
#include "assertDialog.h"
#include "core\EventsDispatcher.h"
#include "core\Thread.h"
#include "core\ThreadSystem.h"
#include "core\ThreadSafe.h"


///////////////////////////////////////////////////////////////////////////////

static void handleMainThreadAssertion( const char* msg, AssertionHandle& assertHandle )
{
   AssertDialog dlg( msg, assertHandle );
   dlg.exec();
}

///////////////////////////////////////////////////////////////////////////////

static void handleWorkerThreadAssertion( const char* msg, AssertionHandle& assertHandle )
{
   TThreadSafe< EventAssert::Choice > assertChoice( EventAssert::EAC_UNKNOWN );
   {
      // dispatch the event
      char whereaboutsStr[1024];
      sprintf_s( whereaboutsStr, "( %s : %d ) %s", assertHandle.getFile(), assertHandle.getLine(), assertHandle.getFunction() );

      DISPATCH_EVENT EventAssert( whereaboutsStr, msg, assertChoice );
   }

   // wait until a decision is made
   while( assertChoice.get() == EventAssert::EAC_UNKNOWN )
   {
      // keep the application running if this is the main thread
      Thread::yield();
   }

   EventAssert::Choice choice = assertChoice.get();
   switch ( choice)
   {
   case EventAssert::EAC_BREAK:
      {
         __asm int 3;
         break;
      }

   case EventAssert::EAC_IGNORE:
      {
         // do nothing
         break;
      }

   case EventAssert::EAC_IGNORE_ALL:
      {
         assertHandle.ignore();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void assertionCallback( const char* msg, AssertionHandle& assertHandle )
{
   // we need to handle an assert in one of 2 ways:
   // - if it occurred on the UI thread ( which is the main app thread by the way ), we'll show a modal dialog - that's because
   //   we need to make a decision as to what to do next ( stop, ignore it ), and the main app loop will be frozen. So we need
   //   something with its own local events pump - and a dialog box is perfect for that.
   // - if it occurred on a worker thread, we'll freeze that thread and create an assert tab in the Tools window

   if ( TSingleton< ThreadSystem >::getInstance().isMainThread() )
   {
      handleMainThreadAssertion( msg, assertHandle );
   }
   else
   {
      handleWorkerThreadAssertion( msg, assertHandle );
   }
}

///////////////////////////////////////////////////////////////////////////////
