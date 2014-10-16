#include "core\ResourceHandle.h"
#include "core\Resource.h"
#include "core\ResourcesManager.h"
#include "core\Thread.h"

 
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( ResourceHandle );
   VERSION_NO( 1 );
   PARENT( ReflectionObject );
   PROPERTY( FilePath, m_path );
   PROPERTY( int, m_objectId )
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ResourceHandle::ResourceHandle( const FilePath& resourcePath, int objectId, ResourcesManager* resMgr )
   : m_path( resourcePath )
   , m_objectId( objectId )
   , m_resource( NULL )
{
   if ( resMgr )
   {
      m_resMgr = resMgr;
   }
   else
   {
      m_resMgr = &TSingleton< ResourcesManager >::getInstance();
   }
}

///////////////////////////////////////////////////////////////////////////////

ResourceHandle::ResourceHandle( const ResourceHandle& rhs )
   : m_path( rhs.m_path )
   , m_objectId( rhs.m_objectId )
   , m_resource( NULL )
{
   if ( rhs.m_resMgr )
   {
      m_resMgr = rhs.m_resMgr;
   }
   else
   {
      m_resMgr = &TSingleton< ResourcesManager >::getInstance();
   }

   // copy the resource instance in a thread-safe way ( we don't have to lock this object's critical section,
   // because the object's just being created )
   m_resource = rhs.m_resource;
}

///////////////////////////////////////////////////////////////////////////////

ResourceHandle::~ResourceHandle()
{
   // if we're dealing with a resource, detach self from it
   if ( m_resource )
   {
      m_resource->detachHandleListener( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceHandle::operator=( const ResourceHandle& rhs )
{
   m_path = rhs.m_path;
   m_objectId = rhs.m_objectId;
   
   // copy the resource instance in a thread-safe way
   m_resource = rhs.m_resource;
}

///////////////////////////////////////////////////////////////////////////////

void ResourceHandle::onObjectPreSave()
{
}

///////////////////////////////////////////////////////////////////////////////

// @caller thread
bool ResourceHandle::isNull() const
{
   bool isInstanceEmpty = ( m_resource == NULL );

   return isInstanceEmpty;
}

///////////////////////////////////////////////////////////////////////////////

// @caller thread
void ResourceHandle::loadResource() const
{
   // load the resource, if necessary
   Resource* res = m_resource;
   if ( !res )
   {
      m_resource = m_resMgr->create( m_path, false );
      m_resource->attachHandleListener( const_cast< ResourceHandle& >( *this ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

// @caller thread
Resource* ResourceHandle::getResource() const
{
   // load the resource, if necessary
   Resource* res = m_resource;
   if ( !res )
   {
      m_resource = m_resMgr->create( m_path, false );
      m_resource->attachHandleListener( const_cast< ResourceHandle& >( *this ) );

   }

   return res;
}

///////////////////////////////////////////////////////////////////////////////

// @ResOpUnload thread
void ResourceHandle::onResourceUnloaded( Resource* hostResource )
{
   ASSERT_MSG( hostResource == m_resource, "Received a notification from an unrelated resource" );
   m_resource = NULL;
}

///////////////////////////////////////////////////////////////////////////////
