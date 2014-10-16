/// @file   core\ResourcesManager.h
/// @brief  manager of file resources used by the game
#ifndef _RESOURCES_MANAGER_H
#define _RESOURCES_MANAGER_H

#include <map>
#include <string>
#include "core\Filesystem.h"
#include "core\MemoryAllocator.h"
#include "core\Singleton.h"


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class Resource;
class IProgressObserver;
class FilesystemScanner;
class FilePath;
class ResourcesDB;
struct SingletonConstruct;

///////////////////////////////////////////////////////////////////////////////

/**
 * This manager manages the lifetime and accessibility of resources that can 
 * be loaded from files and usually contain large amount of data we don't want 
 * to scatter the precious memory with.
 *
 * Since there can only be a single instance of a resource manager ( there's no point
 * in having two instances managing resources ), this class is made into a singleton.
 *
 *
 * Resources manager works in two modes:
 * - RMM_ReadWrite  - in this mode the manager can create, load and remove existing resources at will
 * - RMM_ReadOnlly  - in this mode the manager can only operate on existing, loaded resources.
 *                    New resources can't be created, existing resources can't be removed or unloaded etc.
 */
class ResourcesManager : public FilesystemListener
{
   PRIORITY_SINGLETON( 8 );
   DECLARE_ALLOCATOR( ResourcesManager, AM_DEFAULT );

private:

   // -------------------------------------------------------------------------

   class ProgressObserverCreator
   {
      DECLARE_ALLOCATOR( ProgressObserverCreator, AM_DEFAULT );

   public:
      virtual ~ProgressObserverCreator() {}
      
      /**
       * Instantiates a progress observer class.
       */
      virtual IProgressObserver* create() const = 0;
   };

   // -------------------------------------------------------------------------

private:
   Filesystem*                         m_filesystem;
   ResourcesDB*                        m_resourcesDB;
   class ResourceDependenciesGraph*    m_dependenciesGraph;
   friend class Resource;

public:
   /**
    * Default constructor.
    */
   ResourcesManager();

   /**
    * Singleton constructor.
    */
   ResourcesManager( const SingletonConstruct& );
   ~ResourcesManager();

   // -------------------------------------------------------------------------
   // Unrestricted read operations
   // -------------------------------------------------------------------------

   /**
    * Looks up a resource with the given name of the specified type.
    *
    * @param name       name of the resource
    * @return           pointer to the resource or NULL if the manager does not have one.
    */
   template< typename RESOURCE_TYPE >
   RESOURCE_TYPE* findResource( const FilePath& name );

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
    * The method scans the resource manager memory in search for loaded resources, 
    * starting from the specified root directory, and informs via the FilesystemScanner 
    * interface about its finding.
    *
    * @param rootDir    directory from which the scanning should begin
    * @param scanner
    * @param recursive  use recursive search through the directories tree
    */
   void scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive = true ) const;

   // -------------------------------------------------------------------------
   // Manager configuration
   // -------------------------------------------------------------------------
   /**
    * Reset the manager by removing all registered resources.
    */
   void reset();

   /**
    * Sets a new filesystem the manager should use to load the resources from.
    *
    * @param filesystem
    */
   void setFilesystem( Filesystem& filesystem );

   /**
    * Returns the currently set filesystem.
    *
    * @return filesystem
    */
   inline Filesystem& getFilesystem();

   /**
    * Returns the currently set filesystem (const version).
    *
    * @return filesystem
    */
   inline const Filesystem& getFilesystem() const;

   /**
    * Returns the number of currently managed resources.
    */
   inline uint getResourcesCount() const;

   /**
    * Returns a pointer to the underlying resources database.
    */
   inline ResourcesDB* getResourcesDB() const { return m_resourcesDB; }

   /**
    * Returns the dependencies graph.
    */
   inline class ResourceDependenciesGraph* getDependenciesGraph() const {
      return m_dependenciesGraph;
   }

   // -------------------------------------------------------------------------
   // Synchronous operations API
   // -------------------------------------------------------------------------

   /**
    * Creates a resource based on a file with the specified name and registers it
    * with the manager, if it hasn't been registered before.
    *
    * @param name       name of the resource file (should exist in the
    *                   currently set filesystem)
    * @param loadOnly   if set to 'true', the manager won't attempt to create a new resource
    *                   if it doesn't already exist in the filesystem
    */
   template< typename RESOURCE_TYPE >
   RESOURCE_TYPE* create( const FilePath& name, bool loadOnly = false );

   /**
    * Creates a resource based on a file with the specified name and registers it
    * with the manager, if it hasn't been registered before.
    *
    * @param name       name of the resource file (should exist in the
    *                   currently set filesystem)
    * @param loadOnly   if set to 'true', the manager won't attempt to create a new resource
    *                   if it doesn't already exist in the filesystem
    */
   Resource* create( const FilePath& name, bool loadOnly = false );

   /**
    * Registers a new resource instance with the resources manager and notifies it about this fact
    * by calling the onResourceLoaded method and by registering all active components with it.
    *
    * @param resource   new resource instance to register
    * @return           'true' if the resource was added, 'false' otherwise ( in the latter case the passed resource
    *                   will automatically get deleted
    */
   bool addResource( Resource* resource );
  
   /**
    * Erases a resource from the memory and hard drive.
    *
    * @param resource
    */
   void deleteResource( Resource* resource );

   /**
   * Unloads a resource from the memory and hard drive.
   *
   * @param resourcePath
   */
   void deleteResource( const FilePath& resourcePath );

   /**
   * Unloads a resource from the memory.
   *
   * @param resource
   */
   void unloadResource( Resource* resource );

   /**
   * Unloads a resource from the memory.
   *
   * @param resourcePath
   */
   void unloadResource( const FilePath& resourcePath );

   /**
    * Copies a resource to a different location.
    *
    * @param resource
    * @param newPath
    */
   void copyResource( Resource* resource, const FilePath& newPath );

protected:
   // -------------------------------------------------------------------------
   // ResourcesDB notifications API
   // -------------------------------------------------------------------------
   friend class ResourcesDB;
   void onResourceAdded( Resource* resource );

   // -------------------------------------------------------------------------
   // FilesystemListener implementation
   // -------------------------------------------------------------------------
   void onDirAdded( const FilePath& dir ) override;
   void onDirRemoved( const FilePath& dir ) override;
   void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onFileAdded( const FilePath& path ) override;
   void onFileEdited( const FilePath& path ) override;
   void onFileRemoved( const FilePath& path ) override;
   void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onFilesystemDeleted( Filesystem& filesystem ) override;

private:
   void init();
   void scanResourceDependencies( const FilePath& path );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ResourcesManager.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCES_MANAGER_H
