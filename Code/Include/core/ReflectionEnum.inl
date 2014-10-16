#ifndef _REFLECTION_ENUM_H
#error "This file can only be included from ReflectionEnum.h"
#else

#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
TReflectionEnum< EnumType >::TReflectionEnum()
   : ReflectionEnum( TSingleton< ReflectionTypesRegistry >::getInstance().find< EnumType >()->m_name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
TReflectionEnum< EnumType >::TReflectionEnum( const std::string& typeName )
   : ReflectionEnum( typeName )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
typename TReflectionEnum< EnumType >::AssocMap& TReflectionEnum< EnumType >::getMap()
{
   return m_assocMap;
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
void TReflectionEnum< EnumType >::registerEnumerator( const EnumType enumerator, const std::string &eStr )
{
   const bool registered = getMap().insert( typename AssocMap::value_type( enumerator, eStr ) ).second;
   ASSERT( registered );
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
const std::string& TReflectionEnum< EnumType >::from( const EnumType enumerator ) const
{
   static const std::string emptyStr;

   // Search for the string in our map.
   const typename AssocMap::const_iterator it = m_assocMap.find( enumerator );

   // If we have it, then return the associated enumerator.
   if( it != m_assocMap.end() )
   {
      return it->second;
   }
   else
   {
      return emptyStr;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
const bool TReflectionEnum< EnumType >::to( const std::string& str, EnumType& outEnumerator ) const
{
   // search for the enumerator in our map
   typename AssocMap::const_iterator it = m_assocMap.begin();
   for ( ; it != m_assocMap.end(); ++it )
   {
      if ( it->second == str )
      {
         break;
      }
   }

   // if we didn't find it, we can't do this conversion
   if( it == m_assocMap.end() )
   {
      return false;
   }

   // Verify that there's no enumerator with the same value assigned
   typename AssocMap::const_iterator anotherIt = it;
   for ( ++anotherIt; anotherIt != m_assocMap.end(); ++anotherIt )
   {
      if ( anotherIt->second == str )
      {
         // unfortunately - there is, which means we can't perform an unambiguous conversion
         return false;
      }
   }

   // we found exactly one string which matches the required enumerator
   outEnumerator = it->first;
   return true;
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
int TReflectionEnum< EnumType >::getIndex( int enumeratorValue ) const
{
   int idx = 0;
   for ( typename AssocMap::const_iterator it = m_assocMap.begin(); it != m_assocMap.end(); ++it, ++idx )
   {
      if ( it->first == enumeratorValue )
      {
         return idx;
      }
   }

   // if we got here, it means there's no enumerator with the specified value assigned
   return -1;
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
void TReflectionEnum< EnumType >::getEnumerators( std::vector< std::string >& outEnumerators ) const
{
   for ( AssocMap::const_iterator it = m_assocMap.begin(); it != m_assocMap.end(); ++it )
   {
      outEnumerators.push_back( it->second );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
int TReflectionEnum< EnumType >::getValue( const std::string& enumeratorName ) const
{
   EnumType val;
   if ( to( enumeratorName, val ) )
   {
      return val;
   }
   else
   {
      return -1;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename EnumType >
bool TReflectionEnum< EnumType >::isA( const ReflectionType& referenceType ) const
{
   ReflectionTypeID< EnumType > thisId;
   return referenceType.m_id == thisId.m_id || __super::isA( referenceType );
}

///////////////////////////////////////////////////////////////////////////////


#endif // _REFLECTION_ENUM_H
