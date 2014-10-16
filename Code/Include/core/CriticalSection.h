/// @file   core/CriticalSection.h
/// @brief  threads synchronization tool
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Critical section works a lot like a mutex - it lets through the first thread that locks it, 
 * stopping all subsequent threads from passing until the first one releases it.
 * The difference is that it can be used from a single process only, but it's also much faster than a mutex.
 *
 * As the interface suggests, the same instance of critical section should be used to synchronize multiple threads,
 * so if you're synchronizing a class method i.e., it's best to make the critical section instance a static member of that class.
 */
class CriticalSection
{
   // We're not using custom allocators with this one, because we're using it to synchronize
   // memory allocations - so it's going to be instantiated before we have a memory system in place

private:
   void*    m_criticalSectionHandle;

public:
   /**
    * Constructor.
    */
   CriticalSection();
   ~CriticalSection();

   /**
    * Try entering the critical section without blocking.
    *
    * If locking succeeds, a lock is acquired and has to be released afterwards.
    *
    * @return  `true` if section was successfully entered, 'false' otherwise
    */
   bool tryEntering();

   /**
    * Enter the critical section.
    */
   void enter();

   /**
    * Leave the critical section.
    */
   void leave();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A simple tool that locks access to a section of code by the means of a critical section.
 */
class CriticalSectionedSection
{
   DECLARE_ALLOCATOR( CriticalSectionedSection, AM_DEFAULT );

private:
   CriticalSection&        m_criticalSection;
   bool                    m_yieldAfterUnlock;

public:
   /**
    * Constructor.
    *
    * @param mutex
    * @param yieldAfterUnlock
    */
   CriticalSectionedSection( CriticalSection& criticalSection, bool yieldAfterUnlock = false );
   ~CriticalSectionedSection();
};

///////////////////////////////////////////////////////////////////////////////
