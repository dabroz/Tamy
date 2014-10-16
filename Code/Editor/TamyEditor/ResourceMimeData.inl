#ifndef _RESOURCE_MIME_DATA_H
#error "This file can only be included in ResourceMimeData.h"
#else

#include "core\ReflectionTypesRegistry.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename ResourceType >
ResourceMimeData* ResourceMimeData::create( ResourceType*& outResource )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   ReflectionTypeID< ResourceType > typeId;
   const SerializableReflectionType* acceptedBaseType = typesRegistry.findSerializable( typeId.m_id );

   ASSERT_MSG( acceptedBaseType, "Specified base type not found in the registry" );

   ResourceMimeData* decoder = new ResourceMimeData( acceptedBaseType, (Resource*&)outResource );
   return decoder;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool ResourceMimeData::containsRelevantData( const QMimeData& data )
{
   return containsRelevantData( data, T::getStaticRTTI() );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_MIME_DATA_H
