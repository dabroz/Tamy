#ifndef _RENDER_RESOURCE_STORAGE_H
#error "This file can only be included from RenderResourceStorage.h"
#else

#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\RenderResource.h"


///////////////////////////////////////////////////////////////////////////////

template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::RenderResourceStorage( Renderer& renderer )
   : m_renderer( renderer )
   , m_rendererImplementation( *static_cast< RENDERER* >( m_renderer.implementation() ) )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::~RenderResourceStorage()
{
   // for some reason type resolution doesn't work in this destructor, and I can't
   // call any method on the engine objects that accepts the 'm_renderer'.
   // That's why I created a method clear, which doesn't have that problem.
   // Call it before deleting the storage so that all the host engine objects know
   // their renderer implementations are being removed
}

///////////////////////////////////////////////////////////////////////////////

// @Main thread
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
void RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::clear( const Renderer& renderer )
{
   // release the resources
   unsigned int count = m_engineResources.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      RenderResource* renderResource = m_engineResources[i];
      if ( renderResource != NULL )
      {
         renderResource->detachListener( this );
         renderResource->removeReference();

         // release the resource
         RENDER_RESOURCE resourceImpl = m_resourceImplementation[i];
         releaseResource( resourceImpl );
      }
   }
   m_engineResources.clear();
   m_resourceImplementation.clear();
}

///////////////////////////////////////////////////////////////////////////////

/// @Rendering thread
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
RENDER_RESOURCE RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::getInstance( const ENGINE_TYPE* engineResource )
{
   RENDER_RESOURCE resourceImpl = NULL;

   RenderResource* renderResource = engineResource->m_renderResource;

   uint rrId = renderResource->getRenderResourceId();
   uint resourceSlotsCount = m_resourceImplementation.size();
   if ( rrId >= resourceSlotsCount )
   {
      m_resourceImplementation.resize( rrId + 1, NULL );
      m_engineResources.resize( rrId + 1, NULL );
   }

   resourceImpl = m_resourceImplementation[rrId];
   if ( resourceImpl == NULL )
   {
      // the resource is not initialized - create it
      resourceImpl = createResource( engineResource );
      m_resourceImplementation[rrId] = resourceImpl;
      m_engineResources[rrId] = renderResource;
      renderResource->addReference();

      // attach this storage as a listener
      new ( m_rendererImplementation.mtComm() ) RCAttachRenderResourceListener( this, renderResource );
   }
   else
   {
      // verify that this id really represents that resource
      RenderResource* memorizedRenderResource = m_engineResources[rrId];
      ASSERT_MSG( memorizedRenderResource == renderResource, "Memorized render resource doesn't match that owned by the queried engine resource" );
   }

   return resourceImpl;
}

///////////////////////////////////////////////////////////////////////////////

/// @Rendering thread
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
void RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::refreshResource( RenderResource* renderRes )
{
   ENGINE_TYPE* engineRes = (ENGINE_TYPE*)renderRes->getHostResource();

   uint resourceId = renderRes->getRenderResourceId();
   ASSERT_MSG( resourceId < m_resourceImplementation.size() && m_resourceImplementation[resourceId] != NULL, "No resource by this ID was registered" );
   ASSERT_MSG( m_engineResources[resourceId] == renderRes, "Memorized render resource doesn't match that owned by the queried engine resource" );

   RENDER_RESOURCE& resourceImpl = m_resourceImplementation[resourceId];
   refreshResource( engineRes, resourceImpl );
}

///////////////////////////////////////////////////////////////////////////////

/// @Rendering thread
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
void RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::deleteResource( uint resourceId )
{
   ASSERT_MSG( resourceId < m_resourceImplementation.size() && m_resourceImplementation[resourceId] != NULL, "No resource by this ID was registered" );

   RENDER_RESOURCE resourceImpl = m_resourceImplementation[resourceId];
   releaseResource( resourceImpl );

   m_resourceImplementation[resourceId] = NULL;

   RenderResource* renderResource = m_engineResources[resourceId];
   renderResource->removeReference();
   m_engineResources[resourceId] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
Renderer& RenderResourceStorage< RENDERER, ENGINE_TYPE, RENDER_RESOURCE >::getHostRenderer() const
{
   return m_renderer;
}

///////////////////////////////////////////////////////////////////////////////

#endif _RENDER_RESOURCE_STORAGE_H
