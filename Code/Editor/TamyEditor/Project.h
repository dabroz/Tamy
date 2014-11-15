/// @file   TamyEditor/Project.h
/// @brief  edited project settings
#pragma once

#include <string>
#include "core/Resource.h"
#include "core/FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class FilesystemSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * Edited project settings.
 */
class Project : public Resource
{
   DECLARE_RESOURCE()

private:
   static FilePath                     s_engineDir;

   // ; separated project paths ( because we don't support arrays of simple types at the moment )
   std::string                         m_projectPaths;
      
   // runtime data
   FilesystemSection*                  m_fsSection;

public:
   /**
    * Constructor.
    *
    * @param filePath
    */
   Project( const FilePath& filePath = FilePath() );
   ~Project();

   /**
    * Returns the filesystem section this project spans over.
    */
   inline FilesystemSection& getFilesystemSection() { return *m_fsSection; }

   /**
    * Checks if the specified path is part of the project.
    *
    * @param path
    */
   bool isMember( const FilePath& path ) const;

   /**
    * Adds a new directory to the project.
    *
    * @param dir
    */
   void addDirectory( const FilePath& dir );

   /**
    * Removes a directory from the project.
    *
    * @param dir
    */
   void removeDirectory( const FilePath& dir );

   /**
    * Collects all project directories.
    *
    * @param outProjectDirectories
    */
   void collectDirectories( std::vector< FilePath >& outProjectDirectories );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceSave( ResourcesManager& mgr );
   void onResourceLoaded( ResourcesManager& mgr );

private:
   /**
    * Sets the data from the filesystem section.
    *
    * @param section
    */
   void set( const FilesystemSection& section );

   /**
    * Stores the project settings on the specified filesystem section.
    *
    * @param outSection
    */
   void store( FilesystemSection& outSection );

   /**
    * Adds a couple of absolutely necessary directories to the project.
    */
   void ensureMainDirectoriesPresence();

   /**
    * Checks if the specified directory is one of the runtime directories we don't
    * serialize.
    *
    * @param dir
    */
   bool isRuntimeDir( const std::string& dir ) const;
};

///////////////////////////////////////////////////////////////////////////////
