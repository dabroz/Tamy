#ifndef _REF_PTR_H
#error "This file can only be included from RefPtr.h"
#else

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRefPtr< T >::TRefPtr( T* obj )
   : RefPtr( obj )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRefPtr< T >::TRefPtr( const TRefPtr< T >& rhs )
   : RefPtr( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRefPtr< T >::~TRefPtr()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRefPtr< T >& TRefPtr< T >::operator=( const TRefPtr< T >& rhs )
{
   // we're ditching the old object in favor of the new one, so we need to decrease
   // the old one's reference counter ( since we're not gonna be referencing it any longer )
   // and increase the new one's
   if ( m_obj )
   {
      m_obj->removeReference();
   }

   m_obj = rhs.m_obj;

   if ( m_obj )
   {
      m_obj->addReference();
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRefPtr< T >& TRefPtr< T >::operator=( ReflectionObject* obj )
{
   // we're ditching the old object in favor of the new one, so we need to decrease
   // the old one's reference counter ( since we're not gonna be referencing it any longer )
   // and increase the new one's
   if ( m_obj )
   {
      m_obj->removeReference();
   }

   m_obj = obj;

   if ( m_obj )
   {
      m_obj->addReference();
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _REF_PTR_H
