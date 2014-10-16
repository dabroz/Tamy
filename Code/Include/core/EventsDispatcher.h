/// @file   core/EventsDispatcher.h
/// @brief  gathers and dispatches application level events that come from multiple threads
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"
#include "core\Dequeue.h"
#include "core\Delegate.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;
class TSFragmentedMemoryPool;
class Event;

///////////////////////////////////////////////////////////////////////////////

/**
 * Use this macro to dispatch a new event
 */
#define DISPATCH_EVENT new

///////////////////////////////////////////////////////////////////////////////

/**
 * In an application subsystems communicate via listeners, callbacks etc.
 * In a multithreaded application however, those messages might be marshaled between different
 * threads of execution to unprotected parts of code.
 *
 * This events dispatcher will gather all events coming from various places
 * and dispatch them from the main thread ( or the thread from which you'll call its update method ).
 */
class EventsDispatcher
{
   PRIORITY_SINGLETON( 6 );
   DECLARE_ALLOCATOR( EventsDispatcher, AM_DEFAULT );

   typedef Event*             EventPtr;
private:
   Dequeue< Event* >          m_eventsQueue;
   CriticalSection*           m_queueLock;

   EventPtr*                  m_eventsQueueSnapshot;

   friend class Event;
   TSFragmentedMemoryPool*    m_eventsPool;

public:
   /**
    * Singleton constructor.
    */
   EventsDispatcher( const SingletonConstruct& );
   ~EventsDispatcher();

   /**
    * Dispatches events.
    */
   void dispatchEvents();
};

///////////////////////////////////////////////////////////////////////////////
