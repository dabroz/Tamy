/// @file   core/Mutex.h
/// @brief  threads synchronization tool
#pragma once

#include "core/MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Mutex lets through the first thread that locks it, stopping all subsequent threads from
 * passing until the first one releases it.
 *
 * As the interface suggests, the same instance of mutex should be used to synchronize multiple threads,
 * so if you're synchronizing a class method i.e., it's best to make the mutex instance a static member of that class.
 */
class Mutex
{
   DECLARE_ALLOCATOR( Mutex, AM_DEFAULT );

private:
   void*    m_mutexHandle;

public:
   /**
    * Constructor.
    */
   Mutex();
   ~Mutex();

   /**
    * Acquire a lock on the mutex.
    */
   void lock();

   /**
    * Release the lock.
    */
   void unlock();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A simple tool that locks access to a section of code by the means of a mutex.
 */
class MutexedSection
{
   DECLARE_ALLOCATOR( MutexedSection, AM_DEFAULT );

private:
   Mutex&         m_mutex;
   bool           m_yieldAfterUnlock;

public:
   /**
    * Constructor.
    *
    * @param mutex
    * @param yieldAfterUnlock
    */
   MutexedSection( Mutex& mutex, bool yieldAfterUnlock = false );
   ~MutexedSection();
};

///////////////////////////////////////////////////////////////////////////////
