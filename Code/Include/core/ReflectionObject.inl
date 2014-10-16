#ifndef _REFLECTION_OBJECT_H
#error "This file can only be included in ReflectionObject.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename ReferenceType >
bool ReflectionObject::isExactlyA() const
{
   const SerializableReflectionType& refType = ReferenceType::getStaticRTTI();
   return isExactlyA( refType );
}

///////////////////////////////////////////////////////////////////////////////

template< typename ReferenceType >
bool ReflectionObject::isA() const
{
   const SerializableReflectionType& refType = ReferenceType::getStaticRTTI();
   return isA( refType );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* DynamicCast( ReflectionObject* obj ) 
{
   if ( !obj )
   {
      return NULL;
   }

   if ( obj->isA< T >() )
   {
      T* castObj = static_cast< T* >( obj );
      return castObj;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T* DynamicCast( const ReflectionObject* obj )
{
   if ( !obj )
   {
      return NULL;
   }

   if ( obj->isA< T >() )
   {
      const T* castObj = static_cast< const T* >( obj );
      return castObj;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_OBJECT_H
