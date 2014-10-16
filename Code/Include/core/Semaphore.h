/// @file   core/Semaphore.h
/// @brief  thread synchronization tool
#pragma once

#include "core/MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A semaphore lets the specified number of threads through, stopping
 * the remaining ones from passing until those that pass release their locks.
 */
class Semaphore
{
   DECLARE_ALLOCATOR( Semaphore, AM_DEFAULT );

private:
   void*    m_semaphoreHandle;

public:
   /**
    * Constructor.
    *
    * @param maxThreadsCount     how many threads can simultaneously pass through the semaphore
    */
   Semaphore( uint maxThreadsCount );
   ~Semaphore();

   /**
    * Try to acquire a semaphore lock. 
    * 
    * If the method succeeds, a lock WILL BE acquired - and thus the user will have to
    * release it afterwards.
    *
    * @return  'true' if the lock was acquired, 'false' otherwise
    */
   bool tryAcquiring();

   /**
    * Acquires a semaphore lock.
    */
   void acquire();

   /**
    * Releases a semaphore lock.
    */
   void release();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A simple tool that locks access to a section of code by the means of a semaphore.
 */
class SemaphoredSection
{
   DECLARE_ALLOCATOR( SemaphoredSection, AM_DEFAULT );

private:
   Semaphore&     m_semaphore;
   bool           m_yieldAfterUnlock;

public:
   /**
    * Constructor.
    *
    * @param semaphore
    * @param yieldAfterUnlock
    */
   SemaphoredSection( Semaphore& semaphore, bool yieldAfterUnlock = false );
   ~SemaphoredSection();
};

///////////////////////////////////////////////////////////////////////////////
