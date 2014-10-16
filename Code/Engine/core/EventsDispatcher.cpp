#include "core\EventsDispatcher.h"
#include "core\CriticalSection.h"
#include "core\TSFragmentedMemoryPool.h"
#include "core\Event.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( EventsDispatcher );

///////////////////////////////////////////////////////////////////////////////

EventsDispatcher::EventsDispatcher( const SingletonConstruct& )
   : m_queueLock( new CriticalSection() )
   , m_eventsQueue( 512 )
   , m_eventsPool( new TSFragmentedMemoryPool( 1024, 64 ) )   // we can dispatch up to 512 events simultaneously, but because we're
                                                            // we're copying their list during their dispatch, virtually allowing
                                                            // for new events to be submitted at the same time, we need twice as large
                                                            // an events pool
{
   m_eventsQueueSnapshot = new EventPtr[512];
}

///////////////////////////////////////////////////////////////////////////////

EventsDispatcher::~EventsDispatcher()
{
   // clear the events queue
   m_queueLock->enter();
   
   while( !m_eventsQueue.empty() )
   {
      Event* event = m_eventsQueue.front();
      m_eventsQueue.pop();

      delete event;
   }
   m_eventsQueue.clear();

   m_queueLock->leave();


   delete m_eventsPool;
   m_eventsPool = NULL;

   delete [] m_eventsQueueSnapshot;
   m_eventsQueueSnapshot = NULL;

   delete m_queueLock;
   m_queueLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EventsDispatcher::dispatchEvents()
{
   // grab a snapshot of the queue
   m_queueLock->enter();
   uint count = m_eventsQueue.size();
   for ( uint i = 0; i < count; ++i )
   {
      Event* event = m_eventsQueue.front();
      m_eventsQueueSnapshot[i] = event;
      m_eventsQueue.pop();
   }

   ASSERT_MSG( m_eventsPool->getAllocatedChunksCount() == count, "Not all events were released" );
   m_queueLock->leave();

   // dispatch events
   for ( uint i = 0; i < count; ++i )
   {
      Event* event = m_eventsQueueSnapshot[i];
      event->dispatch();
      delete event;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* Event::operator new( size_t size )
{
   EventsDispatcher& dispatcher = TSingleton< EventsDispatcher >::getInstance();

   dispatcher.m_queueLock->enter();
   void* ptr = dispatcher.m_eventsPool->alloc( size );
   dispatcher.m_eventsQueue.push( (Event*)ptr );
   dispatcher.m_queueLock->leave();

   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void Event::operator delete( void* deletedPtr )
{
   EventsDispatcher& dispatcher = TSingleton< EventsDispatcher >::getInstance();

   dispatcher.m_queueLock->enter();
   dispatcher.m_eventsPool->dealloc( deletedPtr );
   dispatcher.m_queueLock->leave();
}

///////////////////////////////////////////////////////////////////////////////
