#ifndef _LIST_H
#error "This file can only be included from List.h"
#else

#include "core/MemoryRouter.h"
#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::List( MemoryAllocator* allocator )
   : m_allocator( allocator )
   , m_head( NULL )
   , m_tail( NULL )
   , m_elementsCount( 0 )
{
   if ( !m_allocator )
   {
      m_allocator = &TSingleton< MemoryRouter >::getInstance().m_defaultAllocator;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::List( const List& rhs )
   : m_allocator( rhs.m_allocator )
   , m_head( NULL )
   , m_tail( NULL )
   , m_elementsCount( rhs.m_elementsCount )
{
   if ( rhs.m_head )
   {
      m_head = new ( m_allocator ) Elem( *rhs.m_head );
      m_tail = m_head;
      for ( Elem* nextElem = rhs.m_head->m_nextElem; nextElem != NULL; nextElem = nextElem->m_nextElem )
      {
         Elem* copiedElem = new ( m_allocator ) Elem( *nextElem );
         m_tail->m_nextElem = copiedElem;
         copiedElem->m_prevElem = m_tail;
         m_tail = copiedElem;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::~List()
{
   Elem* toRemove;
   for( Elem* elem = m_head; elem != NULL;  )
   {
      toRemove = elem;
      elem = elem->m_nextElem;
      delete toRemove;
   }

   m_head = NULL;
   m_tail = NULL;

   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::operator=( const List& rhs )
{
   // release previous contents
   Elem* toRemove;
   for( Elem* elem = m_head; elem != NULL;  )
   {
      toRemove = elem;
      elem = elem->m_nextElem;
      delete toRemove;
   }

   m_head = NULL;
   m_tail = NULL;

   // copy new contents
   m_elementsCount = rhs.m_elementsCount;

   if ( rhs.m_head )
   {
      m_head = new ( m_allocator ) Elem( *rhs.m_head );
      m_tail = m_head;
      for ( Elem* nextElem = rhs.m_head->m_nextElem; nextElem != NULL; nextElem = nextElem->m_nextElem )
      {
         Elem* copiedElem = new ( m_allocator ) Elem( *nextElem );
         m_tail->m_nextElem = copiedElem;
         copiedElem->m_prevElem = m_tail;
         m_tail = copiedElem;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint List< T >::getElemSize()
{
   return MemoryRouter::sizeOf< Elem >();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::clear()
{
   Elem* toRemove;
   for( Elem* elem = m_head; elem != NULL;  )
   {
      toRemove = elem;
      elem = elem->m_nextElem;
      delete toRemove;
   }

   m_head = NULL;
   m_tail = NULL;
   m_elementsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::empty() const
{
   return m_head == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::pushBack( const T& elem )
{
   Elem* newTail = new ( m_allocator ) Elem( elem );
   if ( m_tail )
   {
      m_tail->m_nextElem = newTail;
      newTail->m_prevElem = m_tail;
      m_tail = newTail;
   }
   else
   {
      m_tail = newTail;
      m_head = newTail;
   }

   // increase the number of elements counter
   ++m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::popBack()
{
   if ( m_tail )
   {
      Elem* removedTail = m_tail;
      m_tail = m_tail->m_prevElem;
      delete removedTail;

      if ( m_tail )
      {
         m_tail->m_nextElem = NULL;
      }
      else
      {
         m_head = NULL;
      }

      // decrease the number of elements counter
      --m_elementsCount;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::pushFront( const T& elem )
{
   Elem* newHead = new ( m_allocator ) Elem( elem );
   if ( m_head )
   {
      m_head->m_prevElem = newHead;
      newHead->m_nextElem = m_head;
      m_head = newHead;
   }
   else
   {
      m_tail = newHead;
      m_head = newHead;
   }

   // increase the number of elements counter
   ++m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::popFront()
{
   if ( m_head )
   {
      Elem* removedHead = m_head;
      m_head = m_head->m_nextElem;
      delete removedHead;

      if ( m_head )
      {
         m_head->m_prevElem = NULL;
      }
      else
      {
         m_tail = NULL;
      }

      // decrease the number of elements counter
      --m_elementsCount;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::insert( iterator& it, const T& elem )
{
   ASSERT_MSG( !it.isEnd() && m_head != NULL, "The element cannot be inserted, because the iterator doesn't point at anything" );
   
   // create a new list element
   Elem* newListElem = new ( m_allocator ) Elem( elem );
   
   // insert it after the element pointed at by the specified iterator
   if ( m_head == NULL )
   {
      m_head = newListElem;
      m_tail = newListElem;
   }
   else
   {
      Elem* oldPrevElement = it.m_currElement->m_prevElem;
      it.m_currElement->m_prevElem = newListElem;
      newListElem->m_nextElem = it.m_currElement;

      newListElem->m_prevElem = oldPrevElement;

      if ( oldPrevElement )
      {
         oldPrevElement->m_nextElem = newListElem;
      }

      // update the head
      if ( m_head == it.m_currElement )
      {
         m_head = newListElem;
      }
   }

   // invalidate the iterator
   it.m_currElement = NULL;

   // increase the number of elements
   ++m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::back() const
{
   ASSERT( m_tail );
   return m_tail->m_element;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& List< T >::back()
{
   ASSERT( m_tail );
   return m_tail->m_element;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::front() const
{
   ASSERT( m_head );
   return m_head->m_element;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& List< T >::front()
{
   ASSERT( m_head );
   return m_head->m_element;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::remove( iterator& it )
{
   Elem* elem = it.m_currElement;
   remove( elem );

   // invalidate the iterator
   it.m_currElement = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::remove( Elem* elem )
{
   if ( elem == NULL )
   {
      return;
   }

   // close the loop around this element
   Elem* prevElem = elem->m_prevElem;
   Elem* nextElem = elem->m_nextElem;

   if ( nextElem )
   {
      nextElem->m_prevElem = prevElem;
   }

   if ( prevElem )
   {
      prevElem->m_nextElem = nextElem;
   }

   // update the head, if it's the removed element
   if ( m_head == elem )
   {
      m_head = nextElem;
   }

   // update the tail, if it's the removed element
   if ( m_tail == elem )
   {
      m_tail = prevElem;
   }

   // delete the element
   delete elem;

   // decrease the number of elements counter
   --m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint List< T >::size() const
{
   return m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator List< T >::begin()
{
   return iterator( this, m_head );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator List< T >::begin() const
{
   return const_iterator( this, m_head );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator List< T >::end()
{
   return iterator( this, m_tail );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator List< T >::end() const
{
   return const_iterator( this, m_tail );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::iterator::iterator( List* hostList, Elem* startElem )
   : m_hostList( hostList )
   , m_currElement( startElem )
   , m_elemForRemoval( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::iterator::iterator( const iterator& rhs )
   : m_hostList( rhs.m_hostList )
   , m_currElement( rhs.m_currElement )
   , m_elemForRemoval( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T>
List< T >::iterator::~iterator()
{
   processElementsRemoval();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::iterator::isEnd() const
{
   return m_currElement == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::iterator::markForRemoval()
{
   // Check for duplicates. Since it's an iterator - providing sequential access to elements - 
   // it makes sense to check only the last element in the list in search of a duplicate

   if ( m_elemForRemoval && m_elemForRemoval->m_elem == m_currElement )
   {
      // yes - it's a duplicate
      return;
   }

   m_elemForRemoval = new ElemForRemoval( m_currElement, m_elemForRemoval );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::iterator::processElementsRemoval()
{
   for ( ElemForRemoval* elem = m_elemForRemoval; elem != NULL; )
   {
      ElemForRemoval* e = elem;
      elem = elem->m_next;

      m_hostList->remove( e->m_elem );
      delete e;   
   }
  
   m_elemForRemoval = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::iterator::operator=( const iterator& rhs )
{
   // first - process the elements scheduled for removal
   processElementsRemoval();

   m_currElement = rhs.m_currElement;
   m_hostList = rhs.m_hostList;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::iterator::operator==( const iterator& rhs ) const
{
   return m_currElement == rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::iterator::operator!=( const iterator& rhs ) const
{
   return m_currElement != rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator& List< T >::iterator::operator++()
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_nextElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator List< T >::iterator::operator++( int )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_nextElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator& List< T >::iterator::operator--()
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prevElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::iterator List< T >::iterator::operator--( int )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prevElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& List< T >::iterator::operator*()
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *(T*)0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::iterator::operator*() const
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *(const T*)0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& List< T >::iterator::operator->( )
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *( T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::iterator::operator->( ) const
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *( const T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::const_iterator::const_iterator( const List* hostList, Elem* startElem )
   : m_hostList( hostList )
   , m_currElement( startElem )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
List< T >::const_iterator::const_iterator( const const_iterator& rhs )
   : m_hostList( rhs.m_hostList )
   , m_currElement( rhs.m_currElement )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T>
List< T >::const_iterator::~const_iterator()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::const_iterator::isEnd() const
{
   return m_currElement == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void List< T >::const_iterator::operator=( const const_iterator& rhs )
{
   m_currElement = rhs.m_currElement;
   m_hostList = rhs.m_hostList;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::const_iterator::operator==( const const_iterator& rhs ) const
{
   return m_currElement == rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool List< T >::const_iterator::operator!=( const const_iterator& rhs ) const
{
   return m_currElement != rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator& List< T >::const_iterator::operator++()
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_nextElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator List< T >::const_iterator::operator++( int )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_nextElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator& List< T >::const_iterator::operator--()
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prevElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename List< T >::const_iterator List< T >::const_iterator::operator--( int )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prevElem;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::const_iterator::operator*() const
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *(const T*)0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& List< T >::const_iterator::operator->( ) const
{
   if ( m_currElement )
   {
      return m_currElement->m_element;
   }
   else
   {
      return *( const T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LIST_H
