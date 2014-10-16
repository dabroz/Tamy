/// @file   core/Runnable.h
/// @brief  an interface to a class that can be executed on a separate thread of execution
#pragma once

#include "core\types.h"


class Thread;

///////////////////////////////////////////////////////////////////////////////

/**
 * An interface to a class that can be executed on a separate thread of execution
 */
class Runnable
{
private:
   Thread*        m_hostThread;

public:
   /**
    * Constructor.
    */
   Runnable();
   virtual ~Runnable();

   /**
    * Put your multithreaded code here
    */
   virtual void run() = 0;

private:
   friend class Thread;

   /**
    * Called by the thread in order to initialize the runnable instance before it's handed over
    * to a new thread of execution.
    *
    * @param hostThread
    * @param stackSize
    * @param outThreadId
    * @param outThreadHandle
    */
   void execute( Thread* hostThread, uint stackSize, ulong& outThreadId, void*& outThreadHandle );

   /**
    * Thread of execution main method
    *
    * @param contextData
    */
   static void threadMain( void* contextData );
};

///////////////////////////////////////////////////////////////////////////////
