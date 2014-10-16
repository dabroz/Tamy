#include "core\FilesystemSection.h"
#include "core\FilesystemSectionScanner.h"
#include "core\Filesystem.h"
#include "core\FilePath.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

FilesystemSection::FilesystemSection( Filesystem& filesystem )
   : m_filesystem( filesystem )
{
   m_filesystem.attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

FilesystemSection::~FilesystemSection()
{
   m_filesystem.detach( *this );
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemSection::isMember( const FilePath& path ) const
{
   uint count = m_allowedDirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      const FilePath& allowedDir = m_allowedDirs[i];
      if ( allowedDir.isLocatedInDir( path ) || path.isLocatedInDir( allowedDir ) )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::addDirectory( const FilePath& path )
{
   // check the path's uniqueness
   uint count = m_allowedDirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_allowedDirs[i] == path )
      {
         return;
      }
   }

   // remove all previously defined directories that are children of the added directory
   for ( int i = count - 1; i >= 0; --i )
   {
      if ( m_allowedDirs[i].isLocatedInDir( path ) )
      {
         m_allowedDirs.erase( m_allowedDirs.begin() + i );
      }
   }

   m_allowedDirs.push_back( path );

   // inform the listeners about the new directory being attached
   {
      uint count = m_listeners.size();
      for ( uint i = 0; i < count; ++i )
      {
         m_listeners[i]->onDirAdded( path );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::removeDirectory( const FilePath& path )
{
   // remove all directories that are either the specified dir, or are its children
   bool wasDirRemoved = false;
   uint count = m_allowedDirs.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      if ( path == m_allowedDirs[i] || m_allowedDirs[i].isLocatedInDir( path ) )
      {
         m_allowedDirs.erase( m_allowedDirs.begin() + i );
         wasDirRemoved = true;
         break;
      }
   }

   // inform the listeners about the new directory being attached
   if ( wasDirRemoved )
   {
      uint count = m_listeners.size();
      for ( uint i = 0; i < count; ++i )
      {
         m_listeners[i]->onDirRemoved( path );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::attach( FilesystemListener& listener )
{
   // check the listener's uniqueness
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_listeners[i] == &listener )
      {
         return;
      }
   }

   m_listeners.push_back( &listener );

   // inform the listener about all kept directories
   {
      uint dirsCount = m_allowedDirs.size();
      for ( uint i = 0; i < dirsCount; ++i )
      {
         listener.onDirAdded( m_allowedDirs[i] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::detach( FilesystemListener& listener )
{
   bool wasDetached = false;
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_listeners[i] == &listener )
      {
         m_listeners.erase( m_listeners.begin() + i );
         wasDetached = true;
         break;
      }
   }

   // inform the listener that all directories from this section have been detached
   if ( wasDetached )
   {
      uint dirsCount = m_allowedDirs.size();
      for ( uint i = 0; i < dirsCount; ++i )
      {
         listener.onDirRemoved( m_allowedDirs[i] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::pullStructure( FilesystemListener& listener, FilePath& root ) const
{
   class ListenerFilter : public FilesystemListener
   {
   private:
      FilesystemListener&           m_listener;
      const FilesystemSection&      m_section;

   public:
      ListenerFilter( FilesystemListener& listener, const FilesystemSection& section )
         : m_listener( listener )
         , m_section( section )
      {}

      // -------------------------------------------------------------------------
      // FilesystemListener implementation
      // -------------------------------------------------------------------------
      void onFileAdded( const FilePath& path ) override
      {
         if ( m_section.isMember( path ) )
         {
            m_listener.onFileAdded( path );
         }
      }

      void onFileEdited( const FilePath& path ) override
      {
         if ( m_section.isMember( path ) )
         {
            m_listener.onFileEdited( path );
         }
      }

      void onFileRemoved( const FilePath& path ) override
      {
         if ( m_section.isMember( path ) )
         {
            m_listener.onFileRemoved( path );
         }
      }

      void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) override
      {
         if ( m_section.isMember( oldPath ) )
         {
            m_listener.onFileRenamed( oldPath, newPath );
         }
      }

      void onDirAdded( const FilePath& dir ) override
      {
         if ( m_section.isMember( dir ) )
         {
            m_listener.onDirAdded( dir );
         }
      }

      void onDirRemoved( const FilePath& dir ) override
      {
         if ( m_section.isMember( dir ) )
         {
            m_listener.onDirRemoved( dir );
         }
      }

      void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) override
      {
         if ( m_section.isMember( oldPath ) )
         {
            m_listener.onDirRenamed( oldPath, newPath );
         }
      }

      void onFilesystemDeleted( Filesystem& filesystem ) override
      {
         m_listener.onFilesystemDeleted( filesystem );
      }
   };

   ListenerFilter filter( listener, *this );
   m_filesystem.pullStructure( filter, root );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onFileAdded( const FilePath& path )
{
   if ( !isMember( path ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onFileAdded( path );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onFileEdited( const FilePath& path )
{
   if ( !isMember( path ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onFileEdited( path );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onFileRemoved( const FilePath& path )
{
   if ( !isMember( path ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onFileRemoved( path );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onDirAdded( const FilePath& dir )
{
   if ( !isMember( dir ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onDirAdded( dir );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onDirRemoved( const FilePath& dir )
{
   if ( !isMember( dir ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onDirRemoved( dir );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onFileRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   if ( !isMember( oldPath ) )
   {
      return;
   }

   // pass the message on to the listeners
   uint count = m_listeners.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_listeners[i]->onFileRenamed( oldPath, newPath );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onDirRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   // the change affects only member directories located within the changed directory
   bool propagateNotification = false;
   const uint count = m_allowedDirs.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      if ( oldPath == m_allowedDirs[i] || m_allowedDirs[i].isLocatedInDir( oldPath ) )
      {
         m_allowedDirs[i] = newPath;
         propagateNotification = true;
         break;
      }
   }

   // pass the message on to the listeners, if applicable
   if ( propagateNotification )
   {
      uint count = m_listeners.size();
      for ( uint i = 0; i < count; ++i )
      {
         m_listeners[i]->onDirRenamed( oldPath, newPath );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSection::onFilesystemDeleted( Filesystem& filesystem )
{
   ASSERT_MSG( false, "This should never take place - a section is an integral part of a filesystem and it lives for as long as the filesystem does" );
}

///////////////////////////////////////////////////////////////////////////////
