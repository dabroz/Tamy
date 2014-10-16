/// @file   core\Filesystem.h
/// @brief  file system manager
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "core\Singleton.h"
#include "core\MemoryRouter.h"
#include "core\TimeDependent.h"
#include "core\FilePath.h"
#include "core\Array.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class File;
class FilePath;
class CriticalSection;
class FilesystemScanner;
class FilesystemChangesTracker;
class FilesystemSnapshot;
struct FilesystemChanges;
struct FilesystemEntry;

///////////////////////////////////////////////////////////////////////////////

/**
 * A listener gets informed about the changes occurring within a file system.
 */
class FilesystemListener
{
public:
   virtual ~FilesystemListener() {}

   /**
    * Called when a new file appears in the filesystem.
    *
    * @param path    file path
    */
   virtual void onFileAdded( const FilePath& path ) = 0;

   /**
    * Called when a file in the file system changes.
    *
    * @param path    file path
    */
   virtual void onFileEdited( const FilePath& path ) = 0;

   /**
    * Called when a file in the file system is removed.
    *
    * @param path    file path
    */
   virtual void onFileRemoved( const FilePath& path ) = 0;

   /**
    * Called when a file in the file system changes its name.
    *
    * @param oldPath
    * @param newPath
    */
   virtual void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) = 0;

   /**
    * Called when a directory was added.
    *
    * @param dir     directory that was added
    */
   virtual void onDirAdded( const FilePath& dir ) = 0;

   /**
    * Called when a directory was removed.
    *
    * @param dir     directory that was removed
    */
   virtual void onDirRemoved( const FilePath& dir ) = 0;

   /**
    * Called when a directory in the file system changes its name.
    *
    * @param oldPath
    * @param newPath
    */
   virtual void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) = 0;

   /**
    * Called when the system is being deleted - it tells the listener to drop the connection.
    *
    * @param filesystem
    */
   virtual void onFilesystemDeleted( Filesystem& filesystem ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A manager of the file system that can be used to access 
 * external files.
 */
class Filesystem
{
   PRIORITY_SINGLETON( 7 );
   DECLARE_ALLOCATOR( Filesystem, AM_DEFAULT );

private:
   struct OpenFileDesc
   {
      DECLARE_ALLOCATOR( OpenFileDesc, AM_DEFAULT );

      FilePath    m_path;
      int         m_refsCount;      // negative value means that the file was opened for reading, a positive one - how many handles are currently reading from it

      OpenFileDesc( const FilePath& path, bool readMode )
         : m_path( path )
         , m_refsCount( readMode ? 1 : -1 )
      {}

      bool isOpenForWriting() const 
      {
         return m_refsCount < 0;
      }

      bool operator==( const OpenFileDesc& rhs ) const 
      {
         return m_path == rhs.m_path;
      }
   };

   struct FileRenameChange
   {
      DECLARE_ALLOCATOR( FileRenameChange, AM_DEFAULT );

      FilePath                   m_oldPath;
      FilePath                   m_newPath;
      const FilesystemEntry*     m_originalEntry;

      FileRenameChange( const FilePath& oldPath, const FilePath& newPath )
         : m_oldPath( oldPath )
         , m_newPath( newPath )
         , m_originalEntry( NULL )
      {}

   };

   typedef List< FilesystemListener* >          Listeners;
   typedef Array< OpenFileDesc >                OpenFilesDescriptors;

private:
   std::string                   m_rootDir;
   Listeners                     m_listeners;

   CriticalSection*              m_fsAccessLock;
   OpenFilesDescriptors          m_openFiles;

   FilesystemChangesTracker*     m_changesScanner;
   FilesystemSnapshot*           m_filesystemSnapshot;
   List< FileRenameChange >      m_fileRenameChanges;

public:
   /**
    * Constructor.
    *
    * @param rootDir
    */
   Filesystem( const char* rootDir = "" );

   /**
    * Singleton constructor.
    */
   Filesystem( const SingletonConstruct& );
   ~Filesystem();

   /**
    * This manager maps the file system to a specific directory's contents.
    * This method allows to define that directory. From this point on 
    * every file name will be considered relative to this root dir.
    *
    * @param rootDir    name of the file system root directory
    */
   void changeRootDir( const std::string& rootDir );

   /**
    * Returns the current filesystem root directory.
    *
    * @return           filesystem root directory name
    */
   const std::string& getCurrRoot() const;

   /**
    * Checks if the specified path points to a directory, or a regular file.
    *
    * @param path
    */
   bool isDir( const FilePath& path ) const;

   /**
    * A quick way to check if the given file exists in the file system.
    *
    * @param fileName   name of the file we want to find
    * @return           'true' if the file exists in the file system,
    *                   'false' otherwise
    */
   bool doesExist( const FilePath& fileName ) const;

   /**
    * The method opens a file with specific access privileges.
    *
    * @param fileName   name of the file we want to open
    * @param mode       access mode
    */
   File* open( const FilePath& fileName, const std::ios_base::openmode mode = std::ios_base::in );

   /**
    * Creates a new directory.
    *
    * @param dirName    relative path to the directory we want to create.
    */
   void mkdir( const FilePath& dirName ) const;

   /**
    * Removes an fs element specified by the path from the file system.
    *
    * @param path       path to an element we want to remove
    *
    * @return           'true' if the element was successfully removed, 'false' otherwise
    */
   bool remove( const FilePath& path );

   /**
    * Renames a file.
    *
    * @param oldPath
    * @param newPath
    */
   bool rename( const FilePath& oldPath, const FilePath& newPath );

   // -------------------------------------------------------------------------
   // Filesystem changes tracking
   // -------------------------------------------------------------------------

   /**
    * Starts tracking changes in the filesystem.
    */
   void startTrackingChanges();

   /**
    * Call this method periodically to check for any changes in the filesystem ( such
    * as file additions and removals, file resaves and so on ).
    */
   void trackChanges();

   /**
    * Forces the test of changes in the filesystem.
    */
   void forceCheckChanges();

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener.
    */
   void attach( FilesystemListener& listener );

   /**
    * Detaches a listener.
    *
    * @param listener.
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
   // Utils
   // -------------------------------------------------------------------------
   /**
    * Converts the specified absolute path to the file system relative path.
    *
    * @param absoluteFilePath
    */
   std::string toRelativePath( const std::string& absoluteFilePath ) const;

   /**
    * Converts the specified relative path to the file system absolute path.
    *
    * @param relativeFilePath
    */
   std::string toAbsolutePath( const std::string& relativeFilePath ) const;

   /**
    * The method scans the file system, starting from the specified root
    * directory, and informs via the FilesystemScanner interface about its finding.
    *
    * @param rootDir          directory from which the scanning should begin
    * @param scanner
    * @param recursive        use recursive search through the directories tree
    */
   void scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive = true ) const;

private:
   /**
    * This notification needs to be called after the file has actually been closed, 
    * because the new timestamp of the file will be read.
    */
   void notifyFileClosed( const FilePath& fileName );

   File* tryOpenFile( const FilePath& fileName, const std::ios_base::openmode mode );

   /**
    * Broadcasts the specified filesystem changes to the listeners.
    *
    * @param changes
    */
   void broadcastFilesystemChanges( const FilesystemChanges& changes );

   friend class File;
   friend class EventFileEdited;
   friend class EventFileRemoved;
   friend class EventDirAdded;
   friend class EventDirRemoved;
};

///////////////////////////////////////////////////////////////////////////////

