#ifndef _REFLECTION_OBJECT_PTR_MIME_DATA_H
#error "This file can only be included in ReflectionObjectPtrMimeData.inl"
#else

#include "core\ReflectionTypesRegistry.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename AcceptedBaseType >
ReflectionObjectPtrMimeData::ReflectionObjectPtrMimeData( ReflectionObject*& objPtr )
   : m_objPtr( objPtr )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   ReflectionTypeID< AcceptedBaseType > typeId;
   m_acceptedBaseType = typesRegistry.findSerializable( typeId.m_id );

   ASSERT_MSG( m_acceptedBaseType, "Specified base type not found in the registry" );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_OBJECT_PTR_MIME_DATA_H
