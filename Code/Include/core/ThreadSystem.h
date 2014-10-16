/// @file   core/ThreadSystem.h
/// @brief  multithreading system
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class Thread;
class Runnable;
class CriticalSection;
struct SingletonConstruct;

///////////////////////////////////////////////////////////////////////////////

/**
 * Core multithreading system.
 */
class ThreadSystem
{
   PRIORITY_SINGLETON( 1 );
   DECLARE_ALLOCATOR( ThreadSystem, AM_DEFAULT );

private:
   struct ThreadEntry
   {
      DECLARE_ALLOCATOR( ThreadEntry, AM_DEFAULT );

      uint                 m_threadId;
      Thread*              m_thread;

      /**
       * Constructor.
       *
       * @param threadId
       * @param thread
       */
      ThreadEntry( uint threadId, Thread* thread )
         : m_threadId( threadId )
         , m_thread( thread )
      {}
   };

private:
   // a Thread instance reserved for the main thread
   Thread*                 m_mainThread;
   ulong                   m_mainThreadId;

   CriticalSection*        m_threadsLock;
   Array< ThreadEntry* >   m_threads;

public:
   /**
    * Singleton constructor.
    */
   ThreadSystem( const SingletonConstruct& );
   ~ThreadSystem();

   /**
    * Returns 'true' if the method was called from the main thread.
    */
   bool isMainThread() const;

   /**
    * Returns id of the main thread.
    */
   inline ulong getMainThreadId() const { return m_mainThreadId; }

   /**
    * Returns a Thread instance dedicated to the thread this method was called from.
    */
   Thread& getCurrentThread();

   /**
    * Returns a number of active processor cores.
    */
   uint getCoresCount() const;

private:
   // -------------------------------------------------------------------------
   // Threads management API
   // -------------------------------------------------------------------------
   friend class Runnable;

   /**
    * Called by a thread of execution is being activated ( it's "start" method is activated ).
    * The method is invoked from that new thread of execution.
    *
    * @param thread
    */
   void registerThread( Thread& thread );

   /**
    * Called when a thread of execution is about to expire.
    */
   void unregisterThread();
};

///////////////////////////////////////////////////////////////////////////////
