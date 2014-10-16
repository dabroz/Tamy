#ifndef _RESOURCE_HANDLE_H
#error "This file can only be included from ResourceHandle.h"
#else

#include "core/ResourcesManager.h"
#include "core/Resource.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
TResourceHandle< T >::TResourceHandle( const Resource& resource, ResourcesManager* resMgr )
   : ResourceHandle( resource.getFilePath() : FilePath(), -1, resMgr )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TResourceHandle< T >::TResourceHandle( const FilePath& resourcePath, ResourcesManager* resMgr )
   : ResourceHandle( resourcePath, -1, resMgr )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TResourceHandle< T >::TResourceHandle( typename const TResourceHandle< T >& rhs )
   : ResourceHandle( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* TResourceHandle< T >::get()
{
   Resource* resource = getResource();
   return static_cast< T* >( resource );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T* TResourceHandle< T >::get() const
{
   const Resource* resource = getResource();
   return static_cast< const T* >( resource );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_HANDLE_H
