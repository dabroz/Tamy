#include "core\Filesystem.h"
#include "core\FilesystemUtils.h"
#include "core\File.h"
#include "core\FilePath.h"
#include "core\StringUtils.h"
#include "core\ListUtils.h"
#include "core\CriticalSection.h"
#include "core\Thread.h"
#include "core\ResourceUtils.h"
#include <stdexcept>
#include <algorithm>

// logging
#include "core\Log.h"

// changes tracking
#include "core\FilesystemChangesTracker.h"
#include "core\FilesystemSnapshot.h"
#include "core\FilesystemSnapshotScanner.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( Filesystem );

///////////////////////////////////////////////////////////////////////////////

Filesystem::Filesystem( const char* rootDir )
   : m_rootDir( rootDir )
   , m_fsAccessLock( new CriticalSection() )
   , m_changesScanner( NULL )
{
   m_filesystemSnapshot = new FilesystemSnapshot( *this );
}

///////////////////////////////////////////////////////////////////////////////

Filesystem::Filesystem( const SingletonConstruct& )
   : m_fsAccessLock( new CriticalSection() )
   , m_changesScanner( NULL )
{
   m_filesystemSnapshot = new FilesystemSnapshot( *this );
}

///////////////////////////////////////////////////////////////////////////////

Filesystem::~Filesystem()
{
   // inform listeners that we're going out of scope
   for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      FilesystemListener* listener = *it;
      listener->onFilesystemDeleted( *this );
   }
   m_listeners.clear();

   delete m_fsAccessLock;
   m_fsAccessLock = NULL;

   delete m_changesScanner;
   m_changesScanner = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::startTrackingChanges()
{
   if ( m_changesScanner )
   {
      // we're already tracking filesystem changes
      return;
   }

   m_changesScanner = new FilesystemChangesTracker( *this );
   m_changesScanner->restart();
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::changeRootDir(const std::string& rootDir)
{
   m_rootDir = rootDir;

   std::size_t endingSlashPos = m_rootDir.find_last_of("/\\");
   if (endingSlashPos != (m_rootDir.length() - 1))
   {
      m_rootDir += "/";
   }

   // notify the changes scanner that it should now start scanning a different directory
   if ( m_changesScanner )
   {
      m_changesScanner->restart();
   }

   // reset the filesystem snapshot
   delete m_filesystemSnapshot;
   m_filesystemSnapshot = new FilesystemSnapshot( *this );
   forceCheckChanges();
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Filesystem::getCurrRoot() const
{
   return m_rootDir;
}

///////////////////////////////////////////////////////////////////////////////

bool Filesystem::isDir( const FilePath& fileName ) const
{
   if ( fileName.empty() )
   {
      // yup - it's the root directory
      return true;
   }

   std::string absPathStr = fileName.toAbsolutePath( *this );
   DWORD attribs = GetFileAttributesA( absPathStr.c_str() );
   return ( ( attribs & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY );
}

///////////////////////////////////////////////////////////////////////////////

bool Filesystem::doesExist( const FilePath& fileName ) const
{
   if ( fileName.empty() )
   {
      return false;
   }

   std::string absPathStr = fileName.toAbsolutePath( *this );
   DWORD attribs = GetFileAttributesA( absPathStr.c_str() );
   return attribs != INVALID_FILE_ATTRIBUTES;
}

///////////////////////////////////////////////////////////////////////////////

File* Filesystem::open( const FilePath& fileName, const std::ios_base::openmode mode )
{
   // block until the file can be open
   File* file = NULL;
   while( true )
   {
      file = tryOpenFile( fileName, mode );

      if ( file )
      {
         break;
      }

      Thread::yield();    
   }

   // at this point we're releasing the lock - because if it turns out that the file could not be opened
   // for some reason, we're gonna destroy its handle which in turn will call 'notifyFileClosed' that will acquire the lock again
   // in order to unregister the file handler

   if ( !file->isOpened() )
   {
      LOG( "Filesystem: Couldn't open the file '%s'", fileName.c_str() );

      delete file;
      file = NULL;
   }

   return file;
}

///////////////////////////////////////////////////////////////////////////////

File* Filesystem::tryOpenFile( const FilePath& fileName, const std::ios_base::openmode mode )
{
   // verify the file is not open
   bool isToBeWritten = ( ( mode & std::ios_base::out ) == std::ios_base::out );
   OpenFileDesc desc( fileName, !isToBeWritten );

   m_fsAccessLock->enter();
   uint openFileIdx = m_openFiles.find( desc );
   if ( openFileIdx != EOA )
   {
      OpenFileDesc& desc = m_openFiles[openFileIdx];

      if ( isToBeWritten )
      {
         // The file is already open, and we cannot open it again for writing
         return NULL;
      }
      else if ( desc.isOpenForWriting() )
      {
         // The file is already open for writing - wait until that operation finishes and try again
         return NULL;
      }

      // at this point we only deal with files open for reading - meaning that they have positive ref values counters.
      ASSERT( desc.m_refsCount > 0 );
      ++desc.m_refsCount;
   }
   else
   {
      m_openFiles.push_back( desc );
   }
   m_fsAccessLock->leave();

   File* file = new File( *this, fileName, mode );
   return file;
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::notifyFileClosed( const FilePath& fileName )
{
   // verify the file is not open
   OpenFileDesc desc( fileName, true );

   m_fsAccessLock->enter();
   uint openFileIdx = m_openFiles.find( desc );
   if ( openFileIdx == EOA )
   {
      ASSERT_MSG( false, "Attempting to close a file that wasn't open in the first place" );
   }
   else
   {
      OpenFileDesc& desc = m_openFiles[openFileIdx];

      --desc.m_refsCount;
      if ( desc.m_refsCount <= 0 )
      {
         m_openFiles.remove( openFileIdx );
      }
   }
   m_fsAccessLock->leave();

   // Update the file's timestamp in the filesystem snapshot - this is done to prevent files edited inside the editor
   // of causing mass updates of related files.
   FilesystemEntry* entry = m_filesystemSnapshot->accessEntry( fileName );
   if ( entry )
   {
      std::string absolutePath = fileName.toAbsolutePath( *this );
      FilesystemUtils::getTimestamp( absolutePath.c_str(), entry->m_timestampLo, entry->m_timestampHi );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::mkdir( const FilePath& dirName ) const
{
   std::string absPath = dirName.toAbsolutePath( *this );
   FilesystemUtils::mkdir( absPath.c_str() );

   FilePath parentDir;
   dirName.extractDir( parentDir );
}

///////////////////////////////////////////////////////////////////////////////

bool Filesystem::remove( const FilePath& path )
{
   std::string absPathStr = path.toAbsolutePath( *this );
   FilesystemUtils::removeFile( absPathStr.c_str() );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Filesystem::rename( const FilePath& oldPath, const FilePath& newPath )
{
   if ( oldPath == newPath )
   {
      // nothing changes
      return true;
   }

   char oldPathAbs[1024];
   char newPathAbs[1024];
   oldPath.toAbsolutePath( *this, oldPathAbs );
   newPath.toAbsolutePath( *this, newPathAbs );

   // check file attributes
   DWORD attribs = GetFileAttributesA( oldPathAbs );
   if ( attribs == INVALID_FILE_ATTRIBUTES )
   {
      // the file doesn't seem to exist
      return false;
   }

   // copy the file to the new location
   bool result = FilesystemUtils::renameFileDir( oldPathAbs, newPathAbs );
   if ( !result )
   {
      // nothing happened - log it and bail
      ASSERT_MSG( false, "File renaming failed" );
      return false;
   }

   // scan the entire repository and update all resources that contain references to the renamed path
   ResourceUtils::updateResourceName( *this, oldPath, newPath );

   // mark the file as renamed
   m_fileRenameChanges.pushBack( FileRenameChange( oldPath, newPath ) );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::toRelativePath(const std::string& absoluteFilePath ) const
{
   // tokenize both the filename and the root dir
   Array<std::string> fileNameTokens;
   Array<std::string> rootDirTokens;

   StringUtils::tokenize( absoluteFilePath, "/", fileNameTokens );
   StringUtils::tokenize( m_rootDir, "/", rootDirTokens );

   // if the root dir path contains more or equal amount of nodes
   // as the filename, file can't possibly be located inside that directory
   if (rootDirTokens.size() >= fileNameTokens.size())
   {
      return "";
   }

   // compare all the nodes from the root directory to those on the file
   // to make sure the file is indeed located inside the root director
   unsigned int rootDirTokensCount = rootDirTokens.size();
   for (unsigned int i = 0; i < rootDirTokensCount; ++i)
   {
      if (rootDirTokens[i] != fileNameTokens[i])
      {
         return "";
      }
   }

   // assemble a file name relative to the root directory
   std::string relativeFileName = "";
   unsigned int fileNameTokensCount = fileNameTokens.size();
   for (unsigned int i = rootDirTokensCount; i < fileNameTokensCount; ++i)
   {
      relativeFileName += "/" + fileNameTokens[i];
   }

   // if the path ended with a backslash (meaning that it was a path to a directory),
   // finish it with one as well
   if ( absoluteFilePath[ absoluteFilePath.length() - 1 ] == '/' )
   {
      relativeFileName += "/";
   }
   return relativeFileName;
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::toAbsolutePath( const std::string& relativeFilePath ) const
{
   return m_rootDir + relativeFilePath;
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::attach( FilesystemListener& listener )
{
   Listeners::iterator it = ListUtils::find( m_listeners, &listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( &listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::detach( FilesystemListener& listener )
{
   Listeners::iterator it = ListUtils::find( m_listeners, &listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::pullStructure( FilesystemListener& listener, FilePath& root ) const
{
   const FilesystemEntry* rootEntry = m_filesystemSnapshot->getEntry( root );
   if ( !rootEntry )
   {
      return;
   }

   List< const FilesystemEntry* > entriesQueue;
   entriesQueue.pushBack( rootEntry );

   while ( !entriesQueue.empty() )
   {
      const FilesystemEntry* entry = entriesQueue.front();
      entriesQueue.popFront();

      for ( List< FilesystemEntry* >::const_iterator it = entry->m_children.begin(); !it.isEnd(); ++it )
      {
         entriesQueue.pushBack( *it );
      }

      if ( entry->m_isDir )
      {
         // notify listeners   
         listener.onDirAdded( entry->m_path );
      }
      else
      {
         listener.onFileAdded( entry->m_path );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::scan( const FilePath& rootDir, FilesystemScanner& scanner, bool recursive ) const
{
   std::vector< std::string > pathsStack;
   pathsStack.push_back( rootDir.toAbsolutePath( *this ) + "/" );

   while( !pathsStack.empty() )
   {
      std::string currPath = pathsStack.back();
      pathsStack.pop_back();

      WIN32_FIND_DATA findFileData;
      HANDLE hFind;
      bool result = true;
      for ( hFind = FindFirstFile( ( currPath + "*" ).c_str(), &findFileData ); hFind != INVALID_HANDLE_VALUE && result; result = FindNextFile( hFind, &findFileData ) )
      {
         std::string name = findFileData.cFileName;
         if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
         {
            // we found a directory
            if ( name != "." && name != ".." )
            {
               std::string dirName = currPath + name + "/";
               scanner.onDirectory( FilePath( toRelativePath( dirName ) ) );

               if ( recursive )
               {
                  // this is a recursive search, so add the found directory to the search tree
                  pathsStack.push_back( dirName );
               }
            }
         }
         else
         {
            // we found a file
            scanner.onFile( FilePath( toRelativePath( currPath + name ) ) );
         }
      }

      FindClose( hFind );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::trackChanges()
{
   if ( !m_changesScanner )
   {
      return;
   }
   
   bool changeDetected = m_changesScanner->processChanges();
   if ( changeDetected )
   {
      forceCheckChanges();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::forceCheckChanges()
{
   FilesystemSnapshot newSnapshot( *this );
   FilesystemSnapshotScanner scanner( *this, newSnapshot );
   scan( FilePath(), scanner, true );

   FilesystemChanges changes;
   m_filesystemSnapshot->compare( newSnapshot, changes );

   // apply the changes ( do it now, 'cause in a sec we'll modify the list of changes to account for the rename operations
   // so that we can broadcast them correctly )
   m_filesystemSnapshot->swapContents( newSnapshot );

   // include the rename changes
   for ( List< FileRenameChange >::iterator it = m_fileRenameChanges.begin(); !it.isEnd(); ++it )
   {
      FileRenameChange& change = *it;

      List< const FilesystemEntry* >::iterator addedFileIt = ListUtils::find< const FilesystemEntry*, FilePath, FilesystemEntry >( changes.m_addedFiles, change.m_newPath );
      if ( !addedFileIt.isEnd() )
      {
         // the file was added as a result of a rename operation - scratch it
         addedFileIt.markForRemoval();
      }

      List< const FilesystemEntry* >::iterator removedFileIt = ListUtils::find< const FilesystemEntry*, FilePath, FilesystemEntry >( changes.m_removedFiles, change.m_oldPath );
      if ( !removedFileIt.isEnd() )
      {
         change.m_originalEntry = *removedFileIt;

         // the file was added as a result of a rename operation - scratch it
         removedFileIt.markForRemoval();
      }
   }

   // broadcast the changes
   broadcastFilesystemChanges( changes );

   // cleanup
   m_fileRenameChanges.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::broadcastFilesystemChanges( const FilesystemChanges& changes )
{
   // added files
   for ( List< const FilesystemEntry* >::const_iterator itChange = changes.m_addedFiles.begin(); !itChange.isEnd(); ++itChange )
   {
      const FilePath& path = itChange->m_path;

      if ( itChange->m_isDir )
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onDirAdded( path );
         }
      }
      else
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onFileAdded( path );
         }
      }
   }

   // removed files
   for ( List< const FilesystemEntry* >::const_iterator itChange = changes.m_removedFiles.begin(); !itChange.isEnd(); ++itChange )
   {
      const FilePath& path = itChange->m_path;

      // scan the entire repository and update all resources that contain references to the renamed path
      ResourceUtils::updateResourceName( *this, path, FilePath( "<deleted>" ) );

      if ( itChange->m_isDir )
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onDirRemoved( path );
         }
      }
      else
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onFileRemoved( path );
         }
      }
   }


   // modified files
   for ( List< const FilesystemEntry* >::const_iterator itChange = changes.m_modifiedFiles.begin(); !itChange.isEnd(); ++itChange )
   {
      const FilePath& path = itChange->m_path;

      // check file attributes
      if ( !itChange->m_isDir )
      {
         // we only broadcast the changes of the files
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onFileEdited( path );
         }
      }
   }

   // renamed files
   for ( List< FileRenameChange >::iterator it = m_fileRenameChanges.begin(); !it.isEnd(); ++it )
   {
      const FileRenameChange& change = *it;
      if ( change.m_originalEntry->m_isDir )
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onFileRenamed( change.m_oldPath, change.m_newPath );
         }
      }
      else
      {
         for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            FilesystemListener* listener = *it;
            listener->onFileRenamed( change.m_oldPath, change.m_newPath );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
