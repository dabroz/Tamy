#include "core-Renderer\RenderResource.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderResourceStorage.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( RenderResource );
   PARENT( ReflectionObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RenderResource::RenderResource( void* hostResource, IdHolder* idHolder )
   : m_hostResource( hostResource )
   , m_idHolder( idHolder )
{
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
RenderResource::~RenderResource()
{
   for ( List< IRenderResourceStorage* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      IRenderResourceStorage* storage = *it;
      Renderer& renderer = storage->getHostRenderer();

      new ( renderer.rtComm() ) RCResourceDeleted( storage, getRenderResourceId() );
   }
   m_listeners.clear();

   m_hostResource = NULL;

   delete m_idHolder;
   m_idHolder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
void RenderResource::attachListener( IRenderResourceStorage* listener )
{
   // check for duplicates
   for ( List< IRenderResourceStorage*>::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      IRenderResourceStorage* checkedListener = *it;
      if ( checkedListener == listener )
      {
         // this listener is already registered
         return;
      }
   }

   m_listeners.pushBack( listener );
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
void RenderResource::detachListener( IRenderResourceStorage* listener )
{
   for ( List< IRenderResourceStorage* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      IRenderResourceStorage* checkedListener = *it;
      if ( checkedListener == listener )
      {
         it.markForRemoval();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
void RenderResource::setDirty()
{
   for ( List< IRenderResourceStorage* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      IRenderResourceStorage* storage = *it;   
      Renderer& renderer = storage->getHostRenderer();

      new ( renderer.rtComm() ) RCRefreshResource( storage, this );
   }
}

///////////////////////////////////////////////////////////////////////////////

uint RenderResource::getRenderResourceId() const
{
   return m_idHolder->getId();
}

///////////////////////////////////////////////////////////////////////////////

void* RenderResource::getHostResource() const
{
   return m_hostResource;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCRefreshResource::RCRefreshResource( IRenderResourceStorage* storage, RenderResource* resource )
   : m_storage( storage )
   , m_resource( resource )
{
}

///////////////////////////////////////////////////////////////////////////////

void RCRefreshResource::execute( Renderer& renderer )
{
   m_storage->refreshResource( m_resource );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCResourceDeleted::RCResourceDeleted( IRenderResourceStorage* storage, uint resourceId )
   : m_storage( storage )
   , m_resourceId( resourceId )
{
}

///////////////////////////////////////////////////////////////////////////////

void RCResourceDeleted::execute( Renderer& renderer )
{
   m_storage->deleteResource( m_resourceId );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCAttachRenderResourceListener::RCAttachRenderResourceListener( IRenderResourceStorage* storage, RenderResource* resource )
   : m_storage( storage )
   , m_resource( resource )
{
}

///////////////////////////////////////////////////////////////////////////////

void RCAttachRenderResourceListener::execute( Renderer& renderer )
{
   m_resource->attachListener( m_storage );
}

///////////////////////////////////////////////////////////////////////////////
