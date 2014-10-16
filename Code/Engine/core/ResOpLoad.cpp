#include "core\ResOpLoad.h"
#include "core\ResourcesManager.h"
#include "core\ResourcesDB.h"
#include "core\IProgressObserver.h"
#include "core\Filesystem.h"
#include "core\File.h"
#include "core\InFileStream.h"
#include "core\Resource.h"
#include "core\ReflectionObject.h"
#include "core\ExternalDependenciesLinker.h"
#include "core\Thread.h"
#include "core\Assert.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

ResOpLoad::ResOpLoad( ResourcesManager* resMgr, const FilePath& loadPath, bool loadOnly, IProgressObserver* progressObserver )
   : m_resMgr( resMgr )
   , m_resourcePath( loadPath )
   , m_loadOnly( loadOnly )
   , m_progressObserver( progressObserver )
   , m_loadedResource( NULL )
   , m_resourceToSave( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResOpLoad::execute()
{
   ResourcesDB* resourcesDB = m_resMgr->getResourcesDB();

   // first - try locating an existing resource in the resources manager - perhaps it already exists
   m_loadedResource = resourcesDB->findResource( m_resourcePath );
   if ( m_loadedResource == NULL  )
   {
      // the resource doesn't exist - try loading it.
      m_loadedResource = load();

      if ( !m_loadedResource && !m_loadOnly )
      {
         // loading failed - and the user indicated that in this case he wants a new resource created, so let's do it
         m_loadedResource = create();
         m_resourceToSave = m_loadedResource;
      }
   }

   // commit operation results
   {
      resourcesDB->lock();

      // At this point all dependencies between the objects have been restored
      // and the resources have been registered with the resources manager.
      // Since it's the resources that manage the loaded objects, perform one last step -
      // inform them that everything has been successfully loaded so that they can kick off any final
      // post-load group activities
      ResourcesMap::const_iterator lastResIt = m_resourcesToFinalize.end();
      for ( ResourcesMap::const_iterator it = m_resourcesToFinalize.begin(); it != lastResIt; ++it )
      {
         Resource* res = it->second;
         resourcesDB->insertResourceEntry( res );
         res->finalizeResourceLoading();
      }

      resourcesDB->unlock();

      // cleanup
      m_resourcesToFinalize.clear();

      // a resource could have been created in the process - it needs to be saved to HD
      // right away - so schedule this command
      if ( m_resourceToSave )
      {
         m_resourceToSave->saveResource();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResOpLoad::reload( Resource* existingResource )
{
   // the resource doesn't exist - try loading it.
   m_reloaderResourcePath = existingResource->getFilePath();
   Resource* newResource = load();
   m_reloaderResourcePath = FilePath();

   // new dependencies may be loaded ( because the new resource may be dependent on
   // another material file or something i.e. )

   // replace the contents of the existing resource with that of the loaded resource
   existingResource->replaceContents( *newResource );

   // we want to finialize the old resource instead of the new one - which only served the purpose
   // of being the data carrier
   ResourcesMap::iterator it = m_resourcesToFinalize.find( existingResource->getFilePath() );
   it->second = existingResource;

   // delete the loaded resource
   delete newResource;

   // finalize loaded resources
   ResourcesDB* resourcesDB = m_resMgr->getResourcesDB();
   {
      resourcesDB->lock();

      // At this point all dependencies between the objects have been restored
      // and the resources have been registered with the resources manager.
      // Since it's the resources that manage the loaded objects, perform one last step -
      // inform them that everything has been successfully loaded so that they can kick off any final
      // post-load group activities
      ResourcesMap::const_iterator lastResIt = m_resourcesToFinalize.end();
      for ( ResourcesMap::const_iterator it = m_resourcesToFinalize.begin(); it != lastResIt; ++it )
      {
         Resource* res = it->second;
         resourcesDB->insertResourceEntry( res );
         res->finalizeResourceLoading();
      }

      resourcesDB->unlock();

      // cleanup
      m_resourcesToFinalize.clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResOpLoad::create()
{
   // the resource doesn't exist at all - create a new one
   const SerializableReflectionType* resourceClass = static_cast< const SerializableReflectionType* >( Resource::findResourceClass( m_resourcePath.extractExtension() ) );
   if ( !resourceClass )
   {
      return NULL;
   }

   Resource* createdResource = resourceClass->instantiate< Resource >();

   ASSERT_MSG( createdResource, "Resource was not created" );
   createdResource->setFilePath( m_resourcePath );

   // put the new resource up for registration
   m_resourcesToFinalize.insert( std::make_pair( m_resourcePath, createdResource ) );

   return createdResource;
}

///////////////////////////////////////////////////////////////////////////////

class SerializationScopeMarker
{
public:
   SerializationScopeMarker()
   {
      ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
      threadSystem.getCurrentThread().m_serializationInProgress = true;
   }

   ~SerializationScopeMarker()
   {
      ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
      threadSystem.getCurrentThread().m_serializationInProgress = false;
   }
};
#define BROADCAST_SERIALIZATION_IN_PROGRESS() SerializationScopeMarker __serializationScopeMarker;

///////////////////////////////////////////////////////////////////////////////

Resource* ResOpLoad::load()
{
   BROADCAST_SERIALIZATION_IN_PROGRESS();

   Filesystem& filesystem = m_resMgr->getFilesystem();

   std::vector< FilePath > resourcesToLoad;
   std::vector< FilePath > resourcesMap;
   resourcesToLoad.push_back( m_resourcePath );

   // notify about the serialization progress
   if ( m_progressObserver )
   {
      m_progressObserver->initialize( resourcesToLoad.size() );
      m_progressObserver->setStatus( "Loading resources" );
   }


   // Go through all resources in the list and load them. New resources
   // will be added to the list as we keep mapping them.
   std::vector< ReflectionObject* > mappedResources;
   std::vector< ReflectionObject* > allLoadedObjects;
   uint loadedResourceIdx = 0;
   FilePath loadedResourcePath;
   while( loadedResourceIdx < resourcesToLoad.size() )
   {
      loadedResourcePath = resourcesToLoad[loadedResourceIdx];
      ++loadedResourceIdx;

      // first check if such a resource is not yet loaded ( perhaps it's already there, and we can save time )
      Resource* res = findResource( loadedResourcePath );
      if ( res == NULL )
      {
         // open the file for loading
         std::string extension = loadedResourcePath.extractExtension();
         std::ios_base::openmode accessMode = Resource::getFileAccessMode( extension );
         File* file = filesystem.open( loadedResourcePath, std::ios_base::in | accessMode );

         if ( file )
         {
            InFileStream inStream( file );
            ReflectionLoader loader;
            loader.deserialize( inStream, &resourcesToLoad, &resourcesMap );
            res = loader.getNextObject< Resource >();

            allLoadedObjects.insert( allLoadedObjects.end(), loader.m_allLoadedObjects.begin(), loader.m_allLoadedObjects.end() );
         }

         if ( res )
         {
            // add the newly loaded resource to our list of resources to finalize
            res->setFilePath( loadedResourcePath );
            m_resourcesToFinalize.insert( std::make_pair( loadedResourcePath, res ) );
         }
      }

      // memorize the mapped resource
      mappedResources.push_back( res );

      // NOTE: do not stop the process if any of the external dependencies are missing - they will simply be missing and the user will have to fill them in manually,
      // however log this incident
      if ( !res )
      {
         LOG( "ResOpLoad: Resource '%s' was found missing when resource '%s' was being loaded", loadedResourcePath.c_str(), m_resourcePath.c_str() );
      }

      // advance the observer ONLY when the resource was successfully loaded
      if ( m_progressObserver )
      {
         m_progressObserver->advance();
      }
   }

   // mapped the loaded resources
   ExternalDependenciesLinker linker( resourcesMap, FindResourceDelegate::FROM_METHOD( ResOpLoad, findResource, this ) );
   linker.linkDependencies( allLoadedObjects );

   // make sure that all loaded objects are informed that they were loaded
   uint allLoadedObjectsCount = allLoadedObjects.size();
   for ( uint i = 0; i < allLoadedObjectsCount; ++i )
   {
      allLoadedObjects[i]->notifyObjectLoaded();
   }

   // return the loaded resource
   Resource* loadedResource = NULL;
   {
      ResourcesMap::iterator it = m_resourcesToFinalize.find( m_resourcePath );
      if ( it != m_resourcesToFinalize.end() )
      {
         loadedResource = it->second;
      }
   }
   return loadedResource;
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResOpLoad::findResource( const FilePath& path )
{
   if ( m_reloaderResourcePath == path )
   {
      // reloaded resource is assumed not to exist
      return NULL;
   }

   // look among loaded resources first
   ResourcesMap::const_iterator it = m_resourcesToFinalize.find( path );
   if ( it != m_resourcesToFinalize.end() )
   {
      // found it
      return it->second;
   }

   // look in the resources manager
   ResourcesDB* resourcesDB = m_resMgr->getResourcesDB();
   Resource* res = resourcesDB->findResource( path );
   return res;
}

///////////////////////////////////////////////////////////////////////////////
