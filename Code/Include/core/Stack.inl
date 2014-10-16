#ifndef _STACK_H
#error "This file can only be included from Stack.h"
#else

#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
Stack< T >::Stack( MemoryAllocator* allocator ) 
   : m_top( NULL ) 
   , m_allocator( allocator )
{
   if ( !m_allocator )
   {
      m_allocator = &TSingleton< MemoryRouter >::getInstance().m_defaultAllocator;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Stack< T >::~Stack() 
{ 
   StackSeg* toRemove;
   for( StackSeg* elem = m_top; elem != NULL;  )
   {
      toRemove = elem;
      elem = elem->m_prev;
      delete toRemove;
   }

   m_top = NULL;

   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Stack< T >::push( T elem )
{
   StackSeg* newSeg = new ( m_allocator ) StackSeg( m_top, elem );
   m_top = newSeg;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T Stack< T >::pop()
{
   StackSeg* removedSeg = m_top;
   m_top = removedSeg->m_prev;
   removedSeg->m_prev = NULL;

   T elem = removedSeg->m_elem;
   delete removedSeg;

   return elem;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Stack< T >::top() const 
{ 
   return m_top->m_elem; 
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Stack< T >::top() 
{ 
   return m_top->m_elem; 
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Stack< T >::empty() const 
{
   return m_top == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename Stack< T >::iterator Stack< T >::begin()
{
   return iterator( this, m_top );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
Stack< T >::iterator::iterator( Stack* hostStack, StackSeg* startElem )
   : m_hostStack( hostStack )
   , m_currElement( startElem )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Stack< T >::iterator::iterator( const iterator& rhs )
   : m_hostStack( rhs.m_hostStack )
   , m_currElement( rhs.m_currElement )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T>
Stack< T >::iterator::~iterator()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Stack< T >::iterator::isEnd() const
{
   return m_currElement == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Stack< T >::iterator::operator=( const iterator& rhs )
{
   m_currElement = rhs.m_currElement;
   m_hostStack = rhs.m_hostStack;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Stack< T >::iterator::operator==( const iterator& rhs ) const
{
   return m_currElement == rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Stack< T >::iterator::operator!=( const iterator& rhs ) const
{
   return m_currElement != rhs.m_currElement;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename Stack< T >::iterator& Stack< T >::iterator::operator++( )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prev;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
typename Stack< T >::iterator Stack< T >::iterator::operator++( int )
{
   if ( m_currElement )
   {
      m_currElement = m_currElement->m_prev;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////


template< typename T >
T& Stack< T >::iterator::operator*( )
{
   if ( m_currElement )
   {
      return m_currElement->m_elem;
   }
   else
   {
      return *( T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Stack< T >::iterator::operator*( ) const
{
   if ( m_currElement )
   {
      return m_currElement->m_elem;
   }
   else
   {
      return *( const T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Stack< T >::iterator::operator->( )
{
   if ( m_currElement )
   {
      return m_currElement->m_elem;
   }
   else
   {
      return *( T* ) 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Stack< T >::iterator::operator->( ) const
{
   if ( m_currElement )
   {
      return m_currElement->m_elem;
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
Stack< T >::StackSeg::StackSeg( StackSeg* prev, T elem ) 
   : m_prev( prev ) 
   , m_elem( elem )
{}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Stack< T >::StackSeg::~StackSeg() 
{ 
   m_prev = NULL;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _STACK_H
