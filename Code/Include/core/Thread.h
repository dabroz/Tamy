/// @file   core/Thread.h
/// @brief  this class represents a single thread of execution
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class TSFragmentedMemoryPool;
class Runnable;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class represents a single thread of execution.
 */
class Thread
{
   DECLARE_ALLOCATOR( Thread, AM_DEFAULT );

public:
   typedef void (*Func)( void* contextData );

public:
   // This memory pool can be accessed using the exposed public allocator - m_sharedMemoryPoolAllocator.
   // It's main purpose is to provide a common, dedicated area in memory for all type-related
   // functionality in order to avoid constant memory reallocations
   TSFragmentedMemoryPool*    m_sharedMemoryPool;

   // Also - each thread has one instance of SerializationFlag that is used by the serialization system
   // to indicate whether an object is created by a serialization routine, or in a normal way.
   bool                       m_serializationInProgress;

private:
   friend class ThreadSystem;
   uint                       m_stackSize;

   void*                      m_threadHandle;
   ulong                      m_threadId;

public:
   /**
    * Constructor.
    *
    * @param stackSize     how large a stack should be created for the thread ( 1kB by default )
    */
   Thread( uint stackSize = 1024 );
   ~Thread();

   /**
    * Starts a new thread of execution with the specified function as its main function.
    *
    * @param runnable
    *
    * @return  'true' if thread was successfully started, 'false' otherwise
    */
   bool start( Runnable& runnable );

   /**
    * Joins the thread back to the main thread of execution, effectively finishing
    * its execution.
    */
   void join();

   /**
    * Returns a thread id of a thread this instance is currently running.
    */
   ulong getThreadId() const { return m_threadId; }

   /**
    * Checks if the thread this method was called on is represented by this thread instance.
    */
   bool isCurrentThread() const;

   /**
    * Yields execution privileges of the thread it's been called from in favor of other threads.
    */
   static void yield();

   /**
    * Suspends caller thread's execution for the specified amount of time.
    *
    * @param miliseconds
    */
   static void sleep( uint miliseconds );

   /**
    * Returns the id of the thread of execution the method was called from.
    */
   static ulong getCurrentThreadId();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This handy macro allows to quickly tell if the object is being serialized at the moment or not.
 */
#define IS_BEING_SERIALIZED()  ( TSingleton< ThreadSystem >::getInstance().getCurrentThread().m_serializationInProgress )

///////////////////////////////////////////////////////////////////////////////
