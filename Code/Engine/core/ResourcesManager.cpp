#include "core.h"
#include "core\ResourcesManager.h"
#include "core\ResourcesDB.h"
#include "core\IProgressObserver.h"
#include "core\ReflectionSerializationUtil.h"
#include "core\Profiler.h"
#include "core\Log.h"
#include "core\MissingDependenciesMapper.h"
#include "core\Singleton.h"
#include "core\ResOpLoad.h"
#include "core\ResourceUtils.h"
#include "core\ResourceDependenciesGraph.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( ResourcesManager );

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::ResourcesManager()
{
   init();
}

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::ResourcesManager( const SingletonConstruct& )
{
   init();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::init( )
{
   m_filesystem = &TSingleton< Filesystem >::getInstance();

   m_resourcesDB = new ResourcesDB( *this );
   m_dependenciesGraph = new ResourceDependenciesGraph();

   m_filesystem->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::~ResourcesManager()
{
   if ( m_filesystem )
   {
      m_filesystem->detach( *this );
   }

   reset();

   m_filesystem = NULL;

   delete m_resourcesDB;
   m_resourcesDB = NULL;

   delete m_dependenciesGraph;
   m_dependenciesGraph = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::reset()
{
   // release all resources
   m_resourcesDB->clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::setFilesystem( Filesystem& filesystem )
{
   // we need to delete all resources along with changing the filesystem
   reset();

   if ( m_filesystem )
   {
      m_filesystem->detach( *this );
   }

   m_filesystem = &filesystem;
   m_filesystem->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResourcesManager::findResource( const FilePath& name )
{
   Resource* res = m_resourcesDB->findResource( name );
   return res;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::collectLoadedResourcesPaths( std::set< FilePath >& outPaths ) const
{
   m_resourcesDB->collectLoadedResourcesPaths( outPaths );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive ) const
{
   // scan the contents of the filesystem
   m_filesystem->scan( rootDir, scanner, recursive );

   // scan the registered resources
   m_resourcesDB->scan( rootDir, scanner, recursive );
}

///////////////////////////////////////////////////////////////////////////////

bool ResourcesManager::addResource( Resource* resource )
{
   if ( !resource )
   {
      return false;
   }

   m_resourcesDB->lock();

   bool wasInserted = m_resourcesDB->insertResourceEntry( resource );
   if ( wasInserted )
   {
      resource->finalizeResourceLoading();
   }

   m_resourcesDB->unlock();
   
   return resource->isManaged();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::deleteResource( Resource* resource )
{
   if ( resource )
   {
      deleteResource( resource->getFilePath() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::deleteResource( const FilePath& resourcePath )
{
   m_resourcesDB->lock();

   m_filesystem->remove( resourcePath );
   m_resourcesDB->unlinkDependencies( resourcePath );

   m_resourcesDB->unlock();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::unloadResource( Resource* resource )
{
   if ( resource )
   {
      unloadResource( resource->getFilePath() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::unloadResource( const FilePath& resourcePath )
{
   m_resourcesDB->lock();

   m_resourcesDB->removeResourceEntry( resourcePath );
   m_resourcesDB->unlinkDependencies( resourcePath );

   m_resourcesDB->unlock();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::copyResource( Resource* resource, const FilePath& newPath )
{
   if ( !resource || !resource->isManaged() )
   {
      return;
   }

   // save the resource in its new place
   FilePath normalizedPath;
   newPath.changeFileExtension( resource->getVirtualExtension(), normalizedPath );
   ResourcesSerializationUtil::saveResource( *m_filesystem, resource, normalizedPath );
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResourcesManager::create( const FilePath& filePath, bool loadOnly )
{
   ResOpLoad operation( this, filePath, loadOnly );
   operation.execute();

   Resource* loadedResource = operation.getAcquiredResource();
   return loadedResource;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onResourceAdded( Resource* resource )
{
   scanResourceDependencies( resource->getFilePath() );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onDirAdded( const FilePath& dir )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onDirRemoved( const FilePath& dir )
{
   // go through the registered resources and see if any of those 
   // still exist - if they don't remove them from the system
   Array< FilePath > entriesToRemove;
   m_resourcesDB->collectResourcesFromDir( dir, entriesToRemove );

   // remove the removed resources entries from the map
   unsigned int count = entriesToRemove.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      const FilePath& resourcePath = entriesToRemove[i];
      bool doesResourceExist = m_filesystem->doesExist( resourcePath );
      if ( !doesResourceExist )
      {
         bool result = m_resourcesDB->removeResourceEntry( resourcePath );
         ASSERT( result );
      }
   }

   m_dependenciesGraph->onDirRemoved( dir );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileAdded( const FilePath& path )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileEdited( const FilePath& path )
{
   if ( m_resourcesDB->findResource( path ) )
   {
      m_dependenciesGraph->clearDependencies( path );
      scanResourceDependencies( path );
   }
   
   List< FilePath > dependentResources = m_dependenciesGraph->getAffectedResources( path );
   dependentResources.pushBack( path );

   // find all resources that may be affected by this file changing
   for ( List< FilePath >::iterator it = dependentResources.begin(); !it.isEnd(); ++it )
   {
      // if any of these resources are already loaded, reload them
      const FilePath& resourcePath = *it;
      Resource* resource = m_resourcesDB->findResource( resourcePath );
      if ( resource )
      {
         ResOpLoad loader( this, resourcePath, true );
         loader.reload( resource );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileRemoved( const FilePath& path )
{
   m_resourcesDB->removeResourceEntry( path );
   m_dependenciesGraph->onFileRemoved( path );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   m_resourcesDB->changeFilePath( oldPath, newPath );
   m_dependenciesGraph->onFileRenamed( oldPath, newPath );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onDirRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   m_resourcesDB->changeDirPath( oldPath, newPath );
   m_dependenciesGraph->onDirRenamed( oldPath, newPath );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFilesystemDeleted( Filesystem& filesystem )
{
   if ( m_filesystem == &filesystem )
   {
      // switch back to the default filesystem
      m_filesystem = &TSingleton< Filesystem >::getInstance();
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::scanResourceDependencies( const FilePath& path )
{
   // the file is a resource - scan its contents and map the dependencies
   List< FilePath > dependencies;
   ResourceUtils::getNonResourceDependencies( *m_filesystem, path, dependencies );

   for ( List< FilePath >::iterator it = dependencies.begin(); !it.isEnd(); ++it )
   {
      const FilePath& dependencyPath = *it;
      m_dependenciesGraph->addDependency( dependencyPath, path );
   }
}

///////////////////////////////////////////////////////////////////////////////
