/// @file   core/ResourcesDB.h
/// @brief  resources database backend
#pragma once

#include "core\MemoryRouter.h"
#include "core\FilePath.h"
#include "core\types.h"
#include "core\Array.h"
#include "core\List.h"
#include <map>
#include <set>


///////////////////////////////////////////////////////////////////////////////

class Resource;
class ResourcesManager;
class FilesystemScanner;

///////////////////////////////////////////////////////////////////////////////

/**
 * Resources database backend.
 */
class ResourcesDB
{
   DECLARE_ALLOCATOR( ResourcesDB, AM_DEFAULT );

private:
   typedef std::map< FilePath, Resource* >                     ResourcesMap;

private:
   ResourcesManager&                   m_host;
   ResourcesMap                        m_resources;

public:
   /**
    * Constructor.
    *
    * @param host
    */
   ResourcesDB( ResourcesManager& host );
   ~ResourcesDB();

   /**
    * Locks the database, allowing the locking thread to perform write operations.
    */
   void lock();

   /**
    * Unlocks the database, disallowing any further write operations.
    */
   void unlock();

   /**
    * Clears the database of all registered resources.
    */
   void clear();

   /**
    * Returns the host ResourcesManager instance.
    */
   inline ResourcesManager& getResourcesManager() { return m_host; }

   /**
    * Returns the number of currently managed resources.
    */
   inline uint getResourcesCount() const { return m_resources.size(); }

   /**
    * Creates a new entry in the resources map.
    *
    * @param resource
    */
   bool insertResourceEntry( Resource* resource );

   /**
    * Removes an entry from the resources map.
    *
    * @param resourcePath
    * @return  'true' if the resource was successfully removed, 'false' otherwise
    */
   bool removeResourceEntry( const FilePath& resourcePath );

   /**
    * Removes an entry from the resources map.
    *
    * @param resource
    * @return  'true' if the resource was successfully removed, 'false' otherwise
    */
   bool removeResourceEntry( Resource* resource );

   /**
    * Changes a filepath of the specified resource.
    *
    * @param oldPath    path to the resource we want to change
    * @param newPath    new resource path
    */
   void changeFilePath( const FilePath& oldPath, const FilePath& newPath );

   /**
   * Changes paths of all resources located in the specified dir.
   *
   * @param oldDirPath
   * @param newDirPath
   */
   void changeDirPath( const FilePath& oldDirPath, const FilePath& newDirPath );

   /**
    * Unlinks dependencies of the loaded resources on a resource with the specified path.
    *
    * @param path
    */
   void unlinkDependencies( const FilePath& path );

   /**
    * Looks for a resource with the specified name ( utility method )
    *
    * @param name
    */
   Resource* findResource( const FilePath& name );

   /**
    * Collects paths of loaded resources.
    *
    * @param outPaths
    */
   void collectLoadedResourcesPaths( std::set< FilePath >& outPaths ) const;

   /**
    * Collects path of loaded resources that are located in the specified dir.
    *
    * @param dir
    * @param outPaths
    */
   void collectResourcesFromDir( const FilePath& dir, Array< FilePath >& outPaths ) const;

   /**
    * Collects path of loaded resources that are located in the specified dir.
    *
    * @param dir
    * @param outPaths
    */
   void collectResourcesFromDir( const FilePath& dir, List< Resource* >& outResources );

   /**
    * The method scans the resource manager memory in search for loaded resources, 
    * starting from the specified root directory, and informs via the FilesystemScanner 
    * interface about its finding.
    *
    * @param rootDir    directory from which the scanning should begin
    * @param scanner
    * @param recursive  use recursive search through the directories tree
    */
   void scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive = true ) const;

};

///////////////////////////////////////////////////////////////////////////////
