#include "core-TestFramework\TestFramework.h"
#include "core\Filesystem.h"
#include "core\FilesystemSection.h"
#include "core\FilePath.h"
#include "core\types.h"
#include "core\EventsDispatcher.h"
#include "core\FilesystemSectionScanner.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   struct FSScannerMock : public FilesystemSectionScanner
   {
      std::vector< FilePath >       m_directories;
      std::vector< FilePath >       m_files;

      FSScannerMock( FilesystemSection& section ) : FilesystemSectionScanner( section ) {}

      void onSectionDirectory( const FilePath& name )
      {
         m_directories.push_back( name );
      }

      void onSectionFile( const FilePath& name )
      {
         m_files.push_back( name );
      }

      void reset()
      {
         m_directories.clear();
         m_files.clear();
      }
   };

   // -------------------------------------------------------------------------

   struct FSListenerMock : public FilesystemListener
   {
      int      m_filesEdited;
      int      m_filesRemoved;
      int      m_dirsChanged;

      FSListenerMock() : m_filesEdited( 0 ), m_filesRemoved( 0 ), m_dirsChanged( 0 ) {}

      void reset()
      {
         m_filesEdited = 0;
         m_filesRemoved = 0;
         m_dirsChanged = 0;
      }

      void onFileAdded( const FilePath& path ) override
      {
      }

      void onFileEdited( const FilePath& path ) override
      {
         m_filesEdited++;
      }

      void onFileRemoved( const FilePath& path ) override
      {
         m_filesRemoved++;
      }

      void onDirAdded( const FilePath& dir ) override
      {
         m_dirsChanged++;
      }

      void onDirRemoved( const FilePath& dir ) override
      {
         m_dirsChanged++;
      }

      void onFilesystemDeleted( Filesystem& filesystem ) override
      {
      }

      void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) override
      {
      }

      void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) override
      {
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSection, restrictedAccessToSpecificDirectories )
{
   Filesystem filesystem;
   filesystem.changeRootDir("../Data/");

   FilesystemSection section( filesystem );

   FilePath rootDirPath( "/" );
   FilePath childDirPath( "/family/" );

   // add the top level dir
   {
      // an empty section ( without any directories defined ) doesn't allow you to access any directories at all
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.addDirectory( rootDirPath );
      CPPUNIT_ASSERT( true == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( true == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.removeDirectory( rootDirPath );
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );
   }

   // add the child dir only
   {
      // an empty section ( without any directories defined ) doesn't allow you to access any directories at all
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.addDirectory( childDirPath );
      CPPUNIT_ASSERT( true == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( true == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.removeDirectory( childDirPath );
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );
   }

   // add the child dir only, but then unregister it be removing its parent ( should also work )
   {
      // an empty section ( without any directories defined ) doesn't allow you to access any directories at all
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.addDirectory( childDirPath );
      CPPUNIT_ASSERT( true == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( true == section.isMember( childDirPath ) );

      // now add the root dir to section and check if it can be changed
      section.removeDirectory( rootDirPath );
      CPPUNIT_ASSERT( false == section.isMember( rootDirPath ) );
      CPPUNIT_ASSERT( false == section.isMember( childDirPath ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSection, scanning )
{
   Filesystem filesystem;
   filesystem.changeRootDir( "../Data/" );

   FilesystemSection section( filesystem );

   FilePath rootDirPath( "/" );
   FilePath childDirPath( "/family/" );

   // first when nothing is defined
   {
      FSScannerMock scanner( section );
      filesystem.scan( rootDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_directories.size() );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_files.size() );

      filesystem.scan( childDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_directories.size() );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_files.size() );
   }

   // next - when the root directory is defined
   {
      section.addDirectory( rootDirPath );

      FSScannerMock scanner( section );
      filesystem.scan( rootDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)1, scanner.m_directories.size() ); // only "/family/" will be reported here, since we start the scan from "/"
      CPPUNIT_ASSERT_EQUAL( (uint)6, scanner.m_files.size() ); // // files from both "/" and "/family/" will be reported here

      scanner.reset();
      filesystem.scan( childDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_directories.size() ); // "/family/" won't be reported since it's the dir we start the scan from
      CPPUNIT_ASSERT_EQUAL( (uint)1, scanner.m_files.size() ); // only the files from "/family/" will be reported here

      section.removeDirectory( rootDirPath );
   }

   // and last - when only the child directory is defined
   {
      section.addDirectory( childDirPath );

      FSScannerMock scanner( section );
      filesystem.scan( rootDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)1, scanner.m_directories.size() ); // "/family/" will be reported here
      CPPUNIT_ASSERT_EQUAL( (uint)1, scanner.m_files.size() ); // only the files from "/family/" will be reported here

      scanner.reset();
      filesystem.scan( childDirPath, scanner, true );
      CPPUNIT_ASSERT_EQUAL( (uint)0, scanner.m_directories.size() ); // "/family/" won't be reported since it's the dir we start the scan from
      CPPUNIT_ASSERT_EQUAL( (uint)1, scanner.m_files.size() ); // only the files from "/family/" will be reported here

      section.removeDirectory( childDirPath );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSection, listener )
{
   Filesystem filesystem;
   filesystem.changeRootDir( "../Data/" );
   filesystem.startTrackingChanges();

   FilesystemSection section( filesystem );

   FilePath rootDirPath( "/" );
   FilePath childDirPath( "/family/" );
   section.addDirectory( childDirPath );

   // add a new directory ( and remove it afterwards ) in a directory that's not part of the section
   {
      // create a sub dir used during the test
      filesystem.mkdir( FilePath( "/testSection/" ) );

      FSListenerMock listener;
      section.attach( listener );

      // What's important to note here - the changes are done not by the section interface, but by the underlying Filesystem interface.
      // So we can be sure that whoever changes anything in the filesystem that's under our section's jurisdiction,
      // we'll find out about it.
      listener.reset();
      filesystem.mkdir( FilePath( "/testSection/someDir/" ) );
      filesystem.forceCheckChanges();
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      filesystem.remove( FilePath( "/testSection/someDir/" ) );
      filesystem.forceCheckChanges();
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      section.detach( listener );

      // cleanup
      filesystem.remove( FilePath( "/testSection/" ) );
   }

   // same thing, but when we're doing it in a section directory
   {
      FSListenerMock listener;
      section.attach( listener );

      // What's important to note here - the changes are done not by the section interface, but by the underlying Filesystem interface.
      // So we can be sure that whoever changes anything in the filesystem that's under our section's jurisdiction,
      // we'll find out about it.
      listener.reset();
      filesystem.mkdir( FilePath( "/family/someDir/" ) );
      filesystem.forceCheckChanges();
      CPPUNIT_ASSERT_EQUAL( 1, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      listener.reset();
      filesystem.remove( FilePath( "/family/someDir/" ) );
      filesystem.forceCheckChanges();
      CPPUNIT_ASSERT_EQUAL( 1, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      section.detach( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSection, notificationUponProjectDirectoriesChange )
{
   Filesystem filesystem;
   filesystem.changeRootDir( "../Data/" );

   FilesystemSection section( filesystem );

   FilePath rootDirPath( "/" );
   FilePath childDirPath( "/family/" );

   // first test the case where there were no project directories defined before we attached the listener
   {
      FSListenerMock listener;
      section.attach( listener );

      // no project directories - no notifications
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      section.addDirectory( childDirPath );
      CPPUNIT_ASSERT_EQUAL( 1, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

      // detaching the listener also generates notifications - the received info
      // is the same as if the section directory got deleted
      listener.reset();
      section.detach( listener );
      CPPUNIT_ASSERT_EQUAL( 1, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );
   }

   // now test the case when there were some directories defined before we attach the lister
   {
      FSListenerMock listener;

      section.addDirectory( childDirPath );
      section.attach( listener );

      // a bulk info about the attached directories came
      CPPUNIT_ASSERT_EQUAL( 1, listener.m_dirsChanged );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesEdited );
      CPPUNIT_ASSERT_EQUAL( 0, listener.m_filesRemoved );

   }
}

///////////////////////////////////////////////////////////////////////////////
