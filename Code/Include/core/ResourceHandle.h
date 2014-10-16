/// @file   core/ResourceHandle.h
/// @brief  handle to an object aggregated in a resource.

#ifndef _RESOURCE_HANDLE_H
#define _RESOURCE_HANDLE_H

#include <string>
#include "core\MemoryRouter.h"
#include "core\ReflectionObject.h"
#include "core\RefPtr.h"
#include "core\FilePath.h"

 
///////////////////////////////////////////////////////////////////////////////

class Resource;
class ResourcesManager;
class AsyncOperation;

///////////////////////////////////////////////////////////////////////////////

/**
 * A handle to a resource-related object.
 *
 * Because resource may be queried for from a constant context,
 * and we still need to cache loaded objects - we're using mutable values
 * for cached data.
 */
class ResourceHandle : public ReflectionObject
{
   DECLARE_ALLOCATOR( ResourceHandle, AM_DEFAULT );
   DECLARE_CLASS()

protected:
   FilePath                      m_path;
   int                           m_objectId;

private:
   // runtime data
   ResourcesManager*             m_resMgr;

   mutable Resource*             m_resource;

public:
   /**
    * Constructor.
    *
    * @param resourcePath        path to a parent resource of the object
    * @param objectId            id of an object in the resource ( set to -1 to provide access to the resource itself )
    * @param resMgr
    */
   ResourceHandle( const FilePath& resourcePath = FilePath(), int objectId = -1, ResourcesManager* resMgr = NULL );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   ResourceHandle( const ResourceHandle& rhs );
   virtual ~ResourceHandle();

   /**
    * Assignment operator.
    *
    * @param rhs
    */
   void operator=( const ResourceHandle& rhs );

   /**
    * Checks if the resource is loaded at the moment.
    */
   bool isNull() const;

   /**
    * Schedules a resource load operation.
    */
   void loadResource() const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectPreSave();

protected:

   /**
    * Returns a resource the handle operates on.
    */
   Resource* getResource() const;
    
   /**
    * Called by the resource when it's about to be unloaded.
    *
    * @param hostResource
    */
   friend class Resource;
   void onResourceUnloaded( Resource* hostResource );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Handle to a resource.
 */
template< typename T >
class TResourceHandle : public ResourceHandle
{
   DECLARE_ALLOCATOR( TResourceHandle, AM_DEFAULT );

public:
   /**
    * Constructor.
    *
    * @param resource
    * @param resMgr              resources manager instance the resource is registered with ( if none is specified, 
    *                            the default singleton will be referenced )
    */
   TResourceHandle( const Resource& resource, ResourcesManager* resMgr = NULL );

   /**
    * Constructor.
    *
    * @param resourcePath        path to a parent resource of the object
    * @param resMgr              resources manager instance the resource is registered with ( if none is specified, 
    *                            the default singleton will be referenced )
    */
   TResourceHandle( const FilePath& resourcePath = "", ResourcesManager* resMgr = NULL );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   TResourceHandle( typename const TResourceHandle< T >& rhs );

   /**
    * Access operator.
    */
   T* get();

   /**
    * Access operator ( const version )
    */
   const T* get() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/ResourceHandle.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_HANDLE_H
