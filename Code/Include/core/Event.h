/// @file   core/Event.h
/// @brief  application event
#pragma once


///////////////////////////////////////////////////////////////////////////////

/**
 * CAUTION: 
 * Events are allocated in the EventsDispatcher's internal memory pool
 * and they don't work with the MemoryRouter - so DON'T use the DECLARE_ALLOCATOR macro 
 * in them.
 *
 * It's also constrained to contain type that don't require the memory to be aligned
 * be it pointers or regular PODs - this requirement is imposed by a custom placement
 * operator we use here as well as due to performance reasons. 
 * We simply allocate the memory from the underlying pool, without padding it. 
 * 
 * The last constrain is the amount of memory an event can use - it should exceed 64 bytes.
 * This one is because we're using a special kind of memory pool that can allocate 
 * chunks up to that size - and no more.
 * This will enforce the users writing small events that can be dispatched around quickly.
 */
class Event
{
public:
   virtual ~Event() {}

   /**
    * Put your event code here.
    */
   virtual void dispatch() = 0;

   // -------------------------------------------------------------------------
   // Memory allocators
   // -------------------------------------------------------------------------
   static void* operator new( size_t size );
   static void operator delete( void* deletedPtr );
};

///////////////////////////////////////////////////////////////////////////////
