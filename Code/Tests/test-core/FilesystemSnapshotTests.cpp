#include "core-TestFramework\TestFramework.h"
#include "core\Filesystem.h"
#include "core\FilesystemSnapshot.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSnapshot, managingEntries )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   FilesystemSnapshot snapshot( fs );
   snapshot.add( FilePath( "/root/assets/b.txt" ), 0, 0 );
   snapshot.add( FilePath( "/root/code/c.lua" ), 0, 0 );
   snapshot.add( FilePath( "/root/assets/a.txt" ), 0, 0 );
   
   std::string snapshotStr = "(0)root;(1)code;(2)c.lua;(1)assets;(2)b.txt;(2)a.txt;";
   CPPUNIT_ASSERT_EQUAL( snapshotStr, snapshot.toString() );

   // removing a non-existing entry from a non-existing directory
   snapshot.remove( FilePath( "/root/gameplay/x.txt" ) );
   CPPUNIT_ASSERT_EQUAL( snapshotStr, snapshot.toString() );

   // removing a non-existing entry from an existing directory
   snapshot.remove( FilePath( "/root/code/x.txt" ) );
   CPPUNIT_ASSERT_EQUAL( snapshotStr, snapshot.toString() );

   // removing an existing entry
   snapshot.remove( FilePath( "/root/assets/a.txt" ) );
   snapshotStr = "(0)root;(1)code;(2)c.lua;(1)assets;(2)b.txt;";
   CPPUNIT_ASSERT_EQUAL( snapshotStr, snapshot.toString() );

   // removing an existing entry
   snapshot.remove( FilePath( "/root/assets" ) );
   snapshotStr = "(0)root;(1)code;(2)c.lua;";
   CPPUNIT_ASSERT_EQUAL( snapshotStr, snapshot.toString() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemSnapshot, comparingSnapshots )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   FilesystemSnapshot originalSnapshot( fs );
   originalSnapshot.add( FilePath( "/root/assets/b.txt" ), 0, 0 );
   originalSnapshot.add( FilePath( "/root/code/c.lua" ), 0, 0 );
   originalSnapshot.add( FilePath( "/root/assets/a.txt" ), 0, 0 );
   originalSnapshot.add( FilePath( "/root/assets/c.txt" ), 0, 0 );

   FilesystemSnapshot identicalSnapshot( fs );
   identicalSnapshot.add( FilePath( "/root/code/c.lua" ), 0, 0 );
   identicalSnapshot.add( FilePath( "/root/assets/a.txt" ), 0, 0 );
   identicalSnapshot.add( FilePath( "/root/assets/b.txt" ), 0, 0 );
   identicalSnapshot.add( FilePath( "/root/assets/c.txt" ), 0, 0 );
   
   FilesystemSnapshot differentSnapshot( fs );
   differentSnapshot.add( FilePath( "/x.txt" ), 1, 0 );
   differentSnapshot.add( FilePath( "/root/code/c.lua" ), 0, 0 );
   differentSnapshot.add( FilePath( "/root/assets" ), 0, 0 );
   differentSnapshot.add( FilePath( "/root/assets/a.txt" ), 4, 0 );
   differentSnapshot.add( FilePath( "/root/otherDir" ), 5, 0 );
   differentSnapshot.add( FilePath( "/root/assets/c.txt" ), 0, 0 );

   // identical snapshots
   FilesystemChanges changes;
   originalSnapshot.compare( identicalSnapshot, changes );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, changes.m_addedFiles.size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, changes.m_removedFiles.size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, changes.m_modifiedFiles.size() );

   // different snapshots
   originalSnapshot.compare( differentSnapshot, changes );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, changes.m_addedFiles.size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, changes.m_removedFiles.size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, changes.m_modifiedFiles.size() );
    
   List< const FilesystemEntry* >::iterator it = changes.m_addedFiles.begin();
   CPPUNIT_ASSERT_EQUAL( std::string( "/x.txt" ), it->m_path.getRelativePath() ); ++it;
   CPPUNIT_ASSERT_EQUAL( std::string( "/root/otherDir" ), it->m_path.getRelativePath() );

   it = changes.m_removedFiles.begin();
   CPPUNIT_ASSERT_EQUAL( std::string( "/root/assets/b.txt" ), it->m_path.getRelativePath() );

   it = changes.m_modifiedFiles.begin();
   CPPUNIT_ASSERT_EQUAL( std::string( "/root/assets/a.txt" ), it->m_path.getRelativePath() );
}

///////////////////////////////////////////////////////////////////////////////
