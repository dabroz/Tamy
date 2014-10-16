/// @file   core-Renderer/RenderResourceStorage.h
/// @brief  storage for platform dependent resources
#ifndef _RENDER_RESOURCE_STORAGE_H
#define _RENDER_RESOURCE_STORAGE_H

#include "core\Delegate.h"
#include "core\MemoryRouter.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderResource;

///////////////////////////////////////////////////////////////////////////////

class IRenderResourceStorage
{
public:
   virtual ~IRenderResourceStorage() {}

   /**
    * Clears the storage, deleting all objects in it.
    *
    * @param renderer
    */
   virtual void clear( const Renderer& renderer ) = 0;

   /**
    * Called when a resource needs refreshing.
    *
    * @param resource
    */
   virtual void refreshResource( RenderResource* resource ) = 0;

   /**
    * Called when a resource should be deleted.
    *
    * @param resourceId
    */
   virtual void deleteResource( uint resourceId ) = 0;

   /**
    * Returns the instance of host renderer.
    */
   virtual Renderer& getHostRenderer() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Storage for platform dependent resources
 */
template< typename RENDERER, typename ENGINE_TYPE, typename RENDER_RESOURCE >
class RenderResourceStorage : public IRenderResourceStorage
{
   DECLARE_ALLOCATOR( RenderResourceStorage, AM_DEFAULT );

private:
   Renderer&                           m_renderer;
   RENDERER&                           m_rendererImplementation;

   Array< RenderResource* >            m_engineResources;
   Array< RENDER_RESOURCE >            m_resourceImplementation;

public:
   /**
    * Constructor.
    *
    * @param renderer         host renderer
    */
   RenderResourceStorage( Renderer& renderer );
   ~RenderResourceStorage();

   /**
    * Returns an instance of a RenderResource assigned to the specified object.
    *
    * @param obj     engine renderer object
    */
   RENDER_RESOURCE getInstance( const ENGINE_TYPE* obj );

   /**
    * Returns an array of resource implementations
    */
   inline Array< RENDER_RESOURCE >& getImplementations() {
      return m_resourceImplementation;
   }

   // -------------------------------------------------------------------------
   // IRenderResourceStorage implementation
   // -------------------------------------------------------------------------
   void clear( const Renderer& renderer );
   void refreshResource( RenderResource* resource );
   void deleteResource( uint resourceId );
   Renderer& getHostRenderer() const;

private:

   /**
    * Creates a new instance of the resource.
    */
   RENDER_RESOURCE createResource( const ENGINE_TYPE* obj ) const;

   /**
    * Releases the resource.
    */
   void releaseResource( RENDER_RESOURCE resource ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\RenderResourceStorage.inl"

///////////////////////////////////////////////////////////////////////////////

#endif _RENDER_RESOURCE_STORAGE_H
