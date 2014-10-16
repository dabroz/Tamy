#ifndef _BEST_RESULTS_LIST_H
#error "This file can only be included in BestResultsList.h"
#else

#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
void BestResultsList< TWeight, TValue >::init( uint maxResults )
{
   m_entries.clear();
   m_maxEntries = maxResults;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
void BestResultsList< TWeight, TValue >::insert( const TWeight& weight, const TValue& value )
{
   ListUtils::insertSorted< Entry, Entry >( m_entries, Entry( weight, value ) );
   if ( m_entries.size() > m_maxEntries )
   {
      m_entries.popBack();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
typename BestResultsList< TWeight, TValue >::const_iterator BestResultsList< TWeight, TValue >::begin() const
{
   return const_iterator( m_entries.begin() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
BestResultsList< TWeight, TValue >::const_iterator::const_iterator( const typename List< Entry >::const_iterator& it )
: m_it( it )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
BestResultsList< TWeight, TValue >::const_iterator::const_iterator( const const_iterator& rhs )
   : m_it( rhs.m_it )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
BestResultsList< TWeight, TValue >::const_iterator::~const_iterator()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
bool BestResultsList< TWeight, TValue >::const_iterator::isEnd() const
{
   return m_it.isEnd();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
void BestResultsList< TWeight, TValue >::const_iterator::operator=( const const_iterator& rhs )
{
   m_it = rhs.m_it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
bool BestResultsList< TWeight, TValue >::const_iterator::operator==( const const_iterator& rhs ) const
{
   return m_it == rhs.m_it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
bool BestResultsList< TWeight, TValue >::const_iterator::operator!=( const const_iterator& rhs ) const
{
   return m_it != rhs.m_it;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
typename BestResultsList< TWeight, TValue >::const_iterator& BestResultsList< TWeight, TValue >::const_iterator::operator++( )
{
   ++m_it;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
typename BestResultsList< TWeight, TValue >::const_iterator& BestResultsList< TWeight, TValue >::const_iterator::operator--( )
{
   --m_it;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
const TValue& BestResultsList< TWeight, TValue >::const_iterator::operator*( ) const
{
   const Entry& entry = *m_it;
   return entry.m_value;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
const TValue&  BestResultsList< TWeight, TValue >::const_iterator::operator->( ) const
{
   const Entry& entry = *m_it;
   return entry.m_value;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _BEST_RESULTS_LIST_H
