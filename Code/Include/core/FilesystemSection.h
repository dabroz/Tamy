/// @file   core/FilesystemSection.h
/// @brief  a section of the filesystem that filters out all directories except for the selected few
#pragma once

#include <vector>
#include "core\Filesystem.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class FilePath;

///////////////////////////////////////////////////////////////////////////////

/**
 * A section of the filesystem that filters out all directories except for the selected few.
 */
class FilesystemSection : public FilesystemListener
{
   DECLARE_ALLOCATOR( FilesystemSection, AM_DEFAULT );

private:
   Filesystem&                            m_filesystem;

   std::vector< FilePath >                m_allowedDirs;
   std::vector< FilesystemListener* >     m_listeners;

public:
   /**
    * Constructor.
    *
    * @param filesystem       host filesystem
    */
   FilesystemSection( Filesystem& filesystem );
   ~FilesystemSection();

   /**
    * Tells whether the node at the specified path can be modified.
    */
   bool isMember( const FilePath& path ) const;

   /**
    * Adds a new directory to the section filter.
    *
    * @param path
    */
   void addDirectory( const FilePath& path );

   /**
    * Removes a directory from the section filter. If it's a parent directory
    * to several defined section directories, it will cause all of them to be removed.
    *
    * @param path
    */
   void removeDirectory( const FilePath& path );

   /**
    * Returns a list of section directories.
    */
   inline const std::vector< FilePath >& getSectionDirectories() const { return m_allowedDirs; }

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a filesystem listener to the section.
    *
    * @param listener
    */
   void attach( FilesystemListener& listener );

   /**
    * Detaches a filesystem listener from the section.
    *
    * @param listener
    */
   void detach( FilesystemListener& listener );

   /**
    * Pulls the current structure of the filesystem.
    *
    * @param listener
    * @param root       the root directory the structure of which we want pulled
    */
   void pullStructure( FilesystemListener& listener, FilePath& root ) const;

   // -------------------------------------------------------------------------
   // FilesystemListener implementation
   // -------------------------------------------------------------------------
   void onFileAdded( const FilePath& path ) override;
   void onFileEdited( const FilePath& path ) override;
   void onFileRemoved( const FilePath& path ) override;
   void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onDirAdded( const FilePath& dir ) override;
   void onDirRemoved( const FilePath& dir ) override;
   void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onFilesystemDeleted( Filesystem& filesystem ) override;
};

///////////////////////////////////////////////////////////////////////////////
