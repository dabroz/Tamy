#include "core.h"
#include "core/ReflectionTypeComponent.h"
#include "core/InStream.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionTypeComponent::ReflectionTypeComponent( const std::string& memberName, int dataOffset )
   : m_memberName( memberName )
   , m_dataOffset( dataOffset )
   , m_traits( RTC_None )
{
   m_id = generateId( memberName );
}

///////////////////////////////////////////////////////////////////////////////

ReflectionProperty* ReflectionTypeComponent::createProperty( void* object ) const
{
   ReflectionProperty* property = instantiateProperty( object );
   
   if( property != NULL )
   {
      property->setParams( m_id, m_memberName, m_label, m_traits );
   }

   return property;
}

///////////////////////////////////////////////////////////////////////////////

void* ReflectionTypeComponent::getPtrToField( void* object ) const
{
   uint baseAddr = (uint)object;
   uint fieldAddr = baseAddr + m_dataOffset;
   return (void*)fieldAddr;
}

///////////////////////////////////////////////////////////////////////////////
