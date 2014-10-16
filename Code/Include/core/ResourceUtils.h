/// @file   core/ResourceUtils.h
/// @brief  various resource related utilities
#pragma once

#include "core\Filesystem.h"
#include "core\FilesystemScanner.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include <set>


///////////////////////////////////////////////////////////////////////////////

class ResourcesManager;
class Resource;
class InStream;
class OutStream;
class File;

///////////////////////////////////////////////////////////////////////////////

/**
 * This tool indexes the entire repository, resaving all encountered resources.
 *
 * It's to be used in situations when you want to migrate your stale resources
 * to get rid of any patching that takes place when you load them, or when
 * the save file format has changed.
 */
class ResourceUtils
{
public:
   /**
    * Resaves all resources from a repository managed by the specified resources manager.
    *
    * @param resMgr
    */
   static void resaveRepository( ResourcesManager& resMgr );

   /**
    * Updates all resources found in the repository, replacing all references to the old path
    * with the new path.
    *
    * @param fs
    * @param oldPath
    * @param newPath
    */
   static void updateResourceName( Filesystem& fs, const FilePath& oldPath, const FilePath& newPath );

   /**
    * Scans the specified resource and lists all of its dependencies non-resource dependencies.
    *
    * @param fs
    * @param resourcePath
    * @param outDependenciesPath
    */
   static void getNonResourceDependencies( Filesystem& fs, const FilePath& resourcePath, List< FilePath >& outDependenciesPaths );

   /**
    * Finds an available default name for the specified resource that we want to create in the specified parent directory.
    *
    * @param fs
    * @param parentDir
    * @param resource
    * @param outResourcePath
    */
   static void findAvailableDefaultName( Filesystem& fs, const FilePath& parentDir, Resource* resource, FilePath& outResourcePath );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This scanner will pick out all resources and create a list of them
 * for the resave tool to work on.
 */
class ResourcesResaveScanner : public FilesystemScanner
{
public:
   ResourcesManager&          m_resMgr;
   std::set< FilePath >       m_loadedResources;
   uint                       m_maxTemporaryResources;

public:
   /**
    * Constructor.
    *
    * @param resMgr
    */
   ResourcesResaveScanner( ResourcesManager& resMgr );

   // -------------------------------------------------------------------------
   // FilesystemScanner implementation
   // -------------------------------------------------------------------------
   void onDirectory( const FilePath& name ) override;
   void onFile( const FilePath& name ) override;

private:
   void unloadTemporaryResources();
};

///////////////////////////////////////////////////////////////////////////////

class ResourceRenameScanner : public FilesystemScanner
{
public:
   Filesystem&                m_fs;
   FilePath                   m_oldPath;
   FilePath                   m_newPath;

public:
   /**
    * Constructor.
    *
    * @param fs
    * @param oldPath
    * @param newPath
    */
   ResourceRenameScanner( Filesystem& fs, const FilePath& oldPath, const FilePath& newPath );

   // -------------------------------------------------------------------------
   // FilesystemScanner implementation
   // -------------------------------------------------------------------------
   void onDirectory( const FilePath& name ) override;
   void onFile( const FilePath& name ) override;

private:
   bool loadExternalDependencies( InStream& inStream, Array< FilePath >& outResourcesPath ) const;
   void saveExternalDependencies( OutStream& outStream, const Array< FilePath >& resourcesPath ) const;
   void loadFileRemainder( File* file, Array< byte >& outFileContentsBuf ) const;
   void saveFileRemainder( File* file, Array< byte >& fileContentsBuf ) const;
};

///////////////////////////////////////////////////////////////////////////////
