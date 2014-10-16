#ifndef _RESOURCE_MANAGEMENT_UTIL_H
#error "This file can only be included from ResourceManagementUtil.h"
#else

#include "core/Resource.h"
#include "core/ReflectionType.h"
#include "core/FilesystemUtils.h"
#include "core/FilePath.h"


///////////////////////////////////////////////////////////////////////////////

template< typename ResourceType >
bool ResourceManagementUtil::isResourceOfType( const FilePath& path )
{
   std::string extension = FilesystemUtils::extractExtension( path.c_str() );
   const SerializableReflectionType* resourceType = Resource::findResourceClass( extension.c_str() );

   bool isResource = ( resourceType && resourceType->isExactlyA( ResourceType::getStaticRTTI() ) );

   return isResource;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_MANAGEMENT_UTIL_H
