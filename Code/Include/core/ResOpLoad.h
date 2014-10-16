/// @file   core/ResOpLoad.h
/// @brief  an operation that takes care of atomic loading of multiple resources
#pragma once

#include "core\MemoryRouter.h"
#include "core\Event.h"
#include "core\FilePath.h"
#include <map>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class IProgressObserver;
class Resource;
class ResourcesManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * This operation that takes care of atomic loading of multiple resources.
 */
class ResOpLoad
{
   DECLARE_ALLOCATOR( ResOpLoad, AM_DEFAULT );

private:
   ResourcesManager*                         m_resMgr;
   FilePath                                  m_resourcePath;
   bool                                      m_loadOnly;
   FilePath                                  m_reloaderResourcePath;

   IProgressObserver*                        m_progressObserver;

   typedef std::map< FilePath, Resource* >   ResourcesMap;
   ResourcesMap                              m_resourcesToFinalize;

   // this flag indicates whether the created resource was loaded, or created
   Resource*                                 m_loadedResource;
   Resource*                                 m_resourceToSave;


public:
   /**
    * Constructor.
    *
    * @param resMgr
    * @param loadPath
    * @param loadOnly            if set to 'true', the operation won't attempt to create a new resource
    *                            if it doesn't already exist in the filesystem
    * @param progressObserver    a thread-safe progress observer ( optional )
    */
   ResOpLoad( ResourcesManager* resMgr, const FilePath& loadPath, bool loadOnly = false, IProgressObserver* progressObserver = NULL );

   /**
    * Returns the loaded resource.
    */
   inline Resource* getAcquiredResource() const { return m_loadedResource; }

   /**
    * Executes the operation.
    */
   void execute();

   /**
    * Reloads the specified resource.
    *
    * @param existingResource
    */
   void reload( Resource* existingResource );

private:
   /**
    * This method creates a new resource.
    */
   Resource* create();

   /**
    * This method loads the resource from the filesystem.
    */
   Resource* load();

   /**
    * Looks for a resource among the deserialized resources and in the specified resources manager instance.
    *
    * @param path
    * @return  pointer to the requested resource
    */
   Resource* findResource( const FilePath& path );
};

///////////////////////////////////////////////////////////////////////////////
