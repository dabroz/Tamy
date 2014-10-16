#ifndef _RESOURCES_MANAGER_H
#error "This file can only be included from ResourcesManager.h"
#else

#include <stdexcept>
#include "core\Filesystem.h"
#include "core\Resource.h"
#include "core\ResourcesDB.h"


///////////////////////////////////////////////////////////////////////////////

uint ResourcesManager::getResourcesCount() const
{
   return m_resourcesDB->getResourcesCount();
}

///////////////////////////////////////////////////////////////////////////////

Filesystem& ResourcesManager::getFilesystem()
{
   return *m_filesystem;
}

///////////////////////////////////////////////////////////////////////////////

const Filesystem& ResourcesManager::getFilesystem() const
{
   return *m_filesystem;
}

///////////////////////////////////////////////////////////////////////////////

template< typename RESOURCE_TYPE >
RESOURCE_TYPE* ResourcesManager::create( const FilePath& name, bool loadOnly )
{
   FilePath nameWithCorrectExtension;
   name.changeFileExtension( RESOURCE_TYPE::getExtension(), nameWithCorrectExtension );
   Resource* res = create( nameWithCorrectExtension, loadOnly );

   return DynamicCast< RESOURCE_TYPE >( res );
}

///////////////////////////////////////////////////////////////////////////////

template< typename RESOURCE_TYPE >
RESOURCE_TYPE* ResourcesManager::findResource( const FilePath& name )
{
   FilePath nameWithCorrectExtension;
   name.changeFileExtension( RESOURCE_TYPE::getExtension(), nameWithCorrectExtension );
   Resource* res = findResource( nameWithCorrectExtension );
   if ( res )
   {
      return DynamicCast< RESOURCE_TYPE >( res );
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCES_MANAGER_H
