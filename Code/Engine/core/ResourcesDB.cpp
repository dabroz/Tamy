#include "core\ResourcesDB.h"
#include "core\Resource.h"
#include "core\MissingDependenciesMapper.h"
#include "core\Profiler.h"
#include "core\Log.h"
#include "core\Filesystem.h"
#include "core\FilesystemScanner.h"


///////////////////////////////////////////////////////////////////////////////

ResourcesDB::ResourcesDB( ResourcesManager& host )
   : m_host( host )
{
}

///////////////////////////////////////////////////////////////////////////////

ResourcesDB::~ResourcesDB()
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::lock()
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::unlock()
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::clear()
{
   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      Resource* resource = it->second;

      resource->resetResourcesManager();
      resource->removeReference();
   }

   m_resources.clear();
}

///////////////////////////////////////////////////////////////////////////////

bool ResourcesDB::insertResourceEntry( Resource* resource )
{
   if ( resource == NULL )
   {
      return false;
   }

   if ( resource->isManaged() )
   {
      return false;
   }

   // correct the resource's name
   FilePath correctResourcePath;
   resource->getFilePath().changeFileExtension( resource->getVirtualExtension(), correctResourcePath );

   ResourcesMap::iterator it = m_resources.find( correctResourcePath );
   if ( it != m_resources.end() )
   {
      // a resource by this name already exists - and we can't replace it, since
      // pointers to it may exist all over the running application
      resource->removeReference();
      return false;
   }

   resource->setFilePath( correctResourcePath );
   m_resources.insert( std::make_pair( correctResourcePath, resource ) );
   resource->setResourcesManager( *this );

   // notify the host
   m_host.onResourceAdded( resource );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool ResourcesDB::removeResourceEntry( Resource* resource )
{
   if ( resource == NULL )
   {
      return false;
   }

   bool result = removeResourceEntry( resource->getFilePath() );
   return result;
}

///////////////////////////////////////////////////////////////////////////////

bool ResourcesDB::removeResourceEntry( const FilePath& resourcePath )
{
   ResourcesMap::iterator it = m_resources.find( resourcePath );
   if ( it == m_resources.end() )
   {
      return false;
   }

   Resource* resource = it->second;
   resource->resetResourcesManager();
   resource->removeReference();
   m_resources.erase( it );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::changeFilePath( const FilePath& oldPath, const FilePath& newPath )
{
   ResourcesMap::iterator existingResIt = m_resources.find( oldPath );
   if ( existingResIt == m_resources.end() )
   {
      // the resource we want to update doesn't exist
      return;
   }

   ResourcesMap::iterator newResIt = m_resources.find( newPath );
   if ( newResIt != m_resources.end() )
   {
      // the resource we want to map the old one to already exists - overwriting is not supported
      return;
   }

   Resource* resource = existingResIt->second;
   resource->setFilePath( newPath, this );
   m_resources.erase( existingResIt );
   m_resources.insert( std::make_pair( newPath, resource ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::changeDirPath( const FilePath& oldDirPath, const FilePath& newDirPath )
{
   List< FilePath > entriesToRemove;
   List< Resource* > resourcesToReadd;
   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      if ( it->first.isLocatedInDir( oldDirPath ) )
      {
         FilePath newResourcePath;
         it->first.swapPart( oldDirPath, newDirPath, newResourcePath );

         it->second->setFilePath( newResourcePath );
         resourcesToReadd.pushBack( it->second );
         entriesToRemove.pushBack( it->first );
      }
   }

   // remove old entries
   for ( List< FilePath >::iterator it = entriesToRemove.begin(); !it.isEnd(); ++it )
   {
      ResourcesMap::iterator entryToRemoveIt = m_resources.find( *it );
      m_resources.erase( entryToRemoveIt );
   }

   // re-add remapped resources
   for ( List< Resource* >::iterator it = resourcesToReadd.begin(); !it.isEnd(); ++it )
   {
      Resource* resource = *it;
      m_resources.insert( std::make_pair( resource->getFilePath(), resource ) );
   }

}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::unlinkDependencies( const FilePath& path )
{
   // check all registered resources and remove their dependencies on the removed resource
   MissingDependenciesMapper mapper;
   ResourcesMap::const_iterator lastResIt = m_resources.end();
   for ( ResourcesMap::const_iterator it = m_resources.begin(); it != lastResIt; ++it )
   {
      if ( it->first == path )
      {
         // don't edit the resource we're removing
         continue;
      }

      Resource* checkedRes = it->second;
      mapper.set( *checkedRes );
      mapper.removeDependenciesOn( path );
   }
   mapper.reset();
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResourcesDB::findResource( const FilePath& name )
{
   ResourcesMap::iterator it = m_resources.find( name );
   if ( it != m_resources.end() )
   {
      return it->second;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::collectLoadedResourcesPaths( std::set< FilePath >& outPaths ) const
{
   for ( ResourcesMap::const_iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      outPaths.insert( it->first );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::collectResourcesFromDir( const FilePath& dir, Array< FilePath >& outPaths ) const
{
   for ( ResourcesMap::const_iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      const FilePath& path = it->first;
      if ( path.isLocatedInDir( dir ) || path == dir )
      {
         outPaths.push_back( path );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::collectResourcesFromDir( const FilePath& dir, List< Resource* >& outResources )
{
   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      Resource* resource = it->second;
      const FilePath& path = it->first;
      if ( path.isLocatedInDir( dir ) || path == dir )
      {
         outResources.pushBack( resource );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesDB::scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive ) const
{
   const std::size_t rootDirNameLen = rootDir.getRelativePath().length();
   for ( ResourcesMap::const_iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      std::size_t rootDirPos = it->first.getRelativePath().find( rootDir );
      if ( rootDirPos != 0 )
      {
         // we only want to consider files located in the root directory
         continue;
      }

      if ( recursive )
      {
         // it's a recursive search - add all files
         scanner.onFile( it->first );
      }
      else
      {
         // it's not a recursive search - add only the files in this directory
         std::size_t subDirCharPos = it->first.getRelativePath().find_first_of( "/\\", rootDirNameLen );
         if ( subDirCharPos == std::string::npos )
         {
            scanner.onFile( it->first );
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
