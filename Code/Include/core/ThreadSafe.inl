#ifndef _THREAD_SAFE_H
#error "This file can be included only in ThreadSafe.h"
#else

#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
TThreadSafe< T >::TThreadSafe()
   : m_lock( new CriticalSection() )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TThreadSafe< T >::TThreadSafe( const T& rhs )
   : m_val( rhs )
   , m_lock( new CriticalSection() )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TThreadSafe< T >::TThreadSafe( typename const TThreadSafe< T >& rhs )
   : m_val( rhs.m_val )
   , m_lock( new CriticalSection() )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TThreadSafe< T >::~TThreadSafe()
{
   delete m_lock;
   m_lock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TThreadSafe< T >::operator=( const T& rhs )
{
   m_lock->enter();
   m_val = rhs;
   m_lock->leave();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T TThreadSafe< T >::get() const
{
   m_lock->enter();
   T val = m_val;
   m_lock->leave();

   return val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool TThreadSafe< T >::operator==( const T& rhs ) const
{
   m_lock->enter();
   bool result = ( m_val == rhs );
   m_lock->leave();

   return result;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool TThreadSafe< T >::operator!=( const T& rhs ) const
{
   m_lock->enter();
   bool result = ( m_val != rhs );
   m_lock->leave();

   return result;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool TThreadSafe< T >::operator==( typename const TThreadSafe< T >& rhs ) const
{
   m_lock->enter();
   rhs.m_lock->enter();

   bool result = ( m_val == rhs.m_val );

   m_lock->leave();
   rhs.m_lock->leave();

   return result;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool TThreadSafe< T >::operator!=( typename const TThreadSafe< T >& rhs ) const
{
   m_lock->enter();
   rhs.m_lock->enter();

   bool result = ( m_val != rhs.m_val );

   m_lock->leave();
   rhs.m_lock->leave();

   return result;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _THREAD_SAFE_H
