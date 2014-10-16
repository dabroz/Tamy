/// @file   core-Renderer/RenderResource.h
/// @brief  a resource managed by the renderer
#pragma once

#include "core\ReflectionObject.h"
#include "core\List.h"
#include "core\MemoryRouter.h"
#include "core-Renderer\RenderCommand.h"
#include "core\UniqueIdGenerator.h"


///////////////////////////////////////////////////////////////////////////////

class IRenderResourceStorage;

///////////////////////////////////////////////////////////////////////////////

/**
 * Renderer needs to keep its own representations of the rendered stuff ( such as
 * vertex & index buffers, textures etc. ).
 *
 * This class will allow to simply reference those internal renderer resources.
 *
 * A render resource is considered constant - it cannot change. What we do is we
 * create implementations for it on the renderer's implementation side - but these
 * only take from the original resource and assume it hasn't changed ( there's also 
 * a mechanism for signaling that to those implementations ).
 */
class RenderResource : public ReflectionObject
{
   DECLARE_ALLOCATOR( RenderResource, AM_DEFAULT );
   DECLARE_CLASS();

private:

   struct IdHolder
   {
      virtual ~IdHolder() {}

      virtual uint getId() const = 0;
   };

   template< typename T >
   struct TIdHolder : public IdHolder
   {
      uint     m_id;

      TIdHolder()
      {
         m_id = UniqueIdGenerator< T >::getNextId();
      }

      ~TIdHolder()
      {
         UniqueIdGenerator< T >::releaseId( m_id );
         m_id = -1;
      }

      uint getId() const { return m_id; }
   };

private:
   void*                                        m_hostResource;
   List< IRenderResourceStorage* >              m_listeners;

   IdHolder*                                    m_idHolder;

public:
   /**
    * Instantiates this class.
    *
    * @param hostResource
    */
   template< typename T >
   static RenderResource* instantiate( void* hostResource )
   {
      IdHolder* idHolder = new TIdHolder< T >();
      return new RenderResource( hostResource, idHolder );
   }  
   ~RenderResource();

   /**
    * Returns a resource id identifying the resource.
    */
   uint getRenderResourceId() const;

   /**
    * Returns a pointer at the host resource.
    */
   void* getHostResource() const;

   /**
    * Indicates that the resource changed and the implementations should be refreshed.
    */
   void setDirty();

   /**
    * Attaches a new render storage as a listener.
    *
    * @param listener
    */
   void attachListener( IRenderResourceStorage* listener );

   /**
    * Detaches a render storage from listening to this resource's changes.
    *
    * @param listener
    */
   void detachListener( IRenderResourceStorage* listener );

private:
   /**
    * Constructor.
    *
    * @param hostResource
    * @param idHolder
    */
   RenderResource( void* hostResource, IdHolder* idHolder );
};

///////////////////////////////////////////////////////////////////////////////

class RCRefreshResource : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRefreshResource, AM_DEFAULT );

private:
   IRenderResourceStorage*    m_storage;
   RenderResource*            m_resource;

public:
   /**
    * Constructor.
    *
    * @param storage
    * @param resource
    */
   RCRefreshResource( IRenderResourceStorage* storage, RenderResource* resource );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

class RCResourceDeleted : public RenderCommand
{
   DECLARE_ALLOCATOR( RCResourceDeleted, AM_DEFAULT );

private:
   IRenderResourceStorage*    m_storage;
   uint                       m_resourceId;

public:
   /**
    * Constructor.
    *
    * @param storage
    * @param resourceId
    */
   RCResourceDeleted( IRenderResourceStorage* storage, uint resourceId );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

};

///////////////////////////////////////////////////////////////////////////////

class RCAttachRenderResourceListener : public RenderCommand
{
   DECLARE_ALLOCATOR( RCAttachRenderResourceListener, AM_DEFAULT );

private:
   IRenderResourceStorage*    m_storage;
   RenderResource*            m_resource;

public:
   /**
    * Constructor.
    *
    * @param storage
    * @param resource
    */
   RCAttachRenderResourceListener( IRenderResourceStorage* storage, RenderResource* resource );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
