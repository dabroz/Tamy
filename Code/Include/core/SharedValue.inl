#ifndef _SHARED_VALUE_H
#error "This file can only be included in SharedValue.h"
#else

#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
TShared< T >::TShared( const T& rhs )
   : m_val( new T( rhs ) )
   , m_refCounter( new uint( 1 ) )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TShared< T >::TShared( const TShared< T >& rhs )
   : m_val( rhs.m_val )
   , m_refCounter( rhs.m_refCounter )
{
   ++(*m_refCounter);
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TShared< T >::~TShared()
{   
   decreaseRefCounter();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::decreaseRefCounter()
{
   --( *m_refCounter );

   if ( *m_refCounter == 0 )
   {
      delete m_val;
      m_val = NULL;

      delete m_refCounter;
      m_refCounter = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator=( const T& rhs )
{
   *m_val = rhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator=( const TShared< T >& rhs )
{
   // stop tracing the previous value
   decreaseRefCounter();

   // and trace the new one instead, provided of course it's legit
   ASSERT_MSG( rhs.m_refCounter, "You're trying to access a destroyed value" );
   m_val = rhs.m_val;
   m_refCounter = rhs.m_refCounter;
   ++(*m_refCounter);
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& TShared< T >::get() const
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return *m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& TShared< T >::get()
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return *m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T* TShared< T >::getPtr() const
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* TShared< T >::getPtr()
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator==( const T& rhs )
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return *m_val == rhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator==( const TShared< T >& rhs )
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return m_val == rhs.m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator!=( const T& rhs )
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return *m_val != rhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TShared< T >::operator!=( const TShared< T >& rhs )
{
   ASSERT_MSG( m_refCounter, "You're trying to access a destroyed value" );
   return m_val != rhs.m_val;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SHARED_VALUE_H
