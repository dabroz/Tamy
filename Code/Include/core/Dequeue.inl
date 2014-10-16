#ifndef _DEQUEUE_H
#error "This file can only be included from Dequeue.h"
#else

#include "core\DefaultAllocator.h"
#include "core\RoundBuffer.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
Dequeue< T >::Dequeue()
   : m_head( NULL )
   , m_tail( NULL )
   , m_elementsCount( 0 )
   , m_allocator( new DefaultAllocator() )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Dequeue< T >::Dequeue( uint elemsCount )
   : m_head( NULL )
   , m_tail( NULL )
   , m_elementsCount( 0 )
   , m_allocator( NULL )
{
   uint bufSize = RoundBuffer::sizeOf< Elem >() * elemsCount;
   m_allocator = new RoundBuffer( bufSize );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Dequeue< T >::~Dequeue()
{
   clear();

   delete m_allocator;
   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Dequeue< T >::push( const T& elem )
{
   Elem* newElement = new ( m_allocator ) Elem( elem );

   if ( m_tail )
   {
      m_tail->m_next = newElement;
      m_tail = newElement;
   }

   if ( !m_head )
   {
      // this is the very first element
      m_head = newElement;
      m_tail = newElement;
   }

   ++m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T Dequeue< T >::pop()
{
   ASSERT_MSG( m_head, "Collection is empty" );
   T val = m_head->m_val;

   Elem* removedElement = m_head;
   m_head = m_head->m_next;
   delete removedElement;

   if ( m_head == NULL )
   {
      // that was the last element
      m_tail = NULL;
   }

   --m_elementsCount;
   ASSERT_MSG( m_elementsCount > 0 || ( m_head == NULL && m_tail == NULL ), "Queue markers were not reset when all of its elements were removed" );

   return val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Dequeue< T >::front() const
{
   ASSERT_MSG( m_head, "Collection is empty" );
   return m_head->m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Dequeue< T >::front()
{
   ASSERT_MSG( m_head, "Collection is empty" );
   return m_head->m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Dequeue< T >::empty() const
{
   return m_elementsCount == 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Dequeue< T >::clear()
{
   // remove the remaining elements
   for ( Elem* elem = m_head; elem != NULL; )
   {
      Elem* removedElem = elem;
      elem = elem->m_next;
      delete removedElem;
   }

   m_head = NULL;
   m_tail = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint Dequeue< T >::size() const
{
   return m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _DEQUEUE_H
