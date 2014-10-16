#include "core-TestFramework\TestFramework.h"
#include "core\Filesystem.h"
#include "core\FilesystemUtils.h"
#include "core\StreamBuffer.h"


///////////////////////////////////////////////////////////////////////////////

TEST(Filesystem, loadingFilesUnderSpecificRoot)
{
   Filesystem filesystem;

   filesystem.changeRootDir("../Data/");
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "ala.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(false, filesystem.doesExist( FilePath( "ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "family\\ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "family/ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(false, filesystem.doesExist( FilePath( "family/ala.txt" ) ) );

   // same thing will work if we don't specify the closing backslash
   filesystem.changeRootDir("../Data");
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "ala.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(false, filesystem.doesExist( FilePath( "ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "family\\ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(true, filesystem.doesExist( FilePath( "family/ola.txt" ) ) );
   CPPUNIT_ASSERT_EQUAL(false, filesystem.doesExist( FilePath( "family/ala.txt" ) ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Filesystem, accessingFileLikeAMemoryBuffer)
{
   Filesystem filesystem;
   filesystem.changeRootDir( "../Data/" );

   File* ala = filesystem.open( FilePath( "ala.txt" ) );
   StreamBuffer<char> charBuf(*ala);
   CPPUNIT_ASSERT_EQUAL((std::size_t)5, charBuf.size());
   CPPUNIT_ASSERT_EQUAL('1', charBuf[0]);
   CPPUNIT_ASSERT_EQUAL('2', charBuf[1]);
   CPPUNIT_ASSERT_EQUAL('3', charBuf[2]);
   CPPUNIT_ASSERT_EQUAL('4', charBuf[3]);
   CPPUNIT_ASSERT_EQUAL('5', charBuf[4]);

   StreamBuffer<int> intTextBuf(*ala);
   CPPUNIT_ASSERT_EQUAL((std::size_t)2, intTextBuf.size());
   CPPUNIT_ASSERT_EQUAL(875770417, intTextBuf[0]);
   CPPUNIT_ASSERT_EQUAL(53, intTextBuf[1]);

   delete ala;
}

///////////////////////////////////////////////////////////////////////////////

TEST(FilesystemUtils, extractingPathParts)
{
   std::string fileName( "/ola/ula/pies.txt" );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola/ula/" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "pies.txt" ), FilesystemUtils::extractNodeName( fileName ) );

   fileName = "/ola/ula/";
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola/" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "ula/" ), FilesystemUtils::extractNodeName( fileName ) );

   fileName = "/ola/";
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "ola/" ), FilesystemUtils::extractNodeName( fileName ) );

   fileName = "/.svn/";
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( ".svn/" ), FilesystemUtils::extractNodeName( fileName ) );

   fileName = "/";
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), FilesystemUtils::extractNodeName( fileName ) );

   fileName = "";
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), FilesystemUtils::extractDir( fileName ) );
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), FilesystemUtils::extractNodeName( fileName ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemUtils, leaveDir )
{
   std::string dirWithSlash( "/ola/ula/" );
   std::string outDir;

   FilesystemUtils::leaveDir( dirWithSlash, 0, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola/ula" ), outDir );

   FilesystemUtils::leaveDir( dirWithSlash, 1, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola" ), outDir );

   FilesystemUtils::leaveDir( dirWithSlash, 2, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), outDir );

   FilesystemUtils::leaveDir( dirWithSlash, 5, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), outDir );


   std::string dirWithoutSlash( "/ola/ula" );
   FilesystemUtils::leaveDir( dirWithoutSlash, 0, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola/ula" ), outDir );

   FilesystemUtils::leaveDir( dirWithoutSlash, 1, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ola" ), outDir );

   FilesystemUtils::leaveDir( dirWithoutSlash, 2, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), outDir );

   FilesystemUtils::leaveDir( dirWithoutSlash, 5, outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "" ), outDir );
}

///////////////////////////////////////////////////////////////////////////////

TEST( FilesystemUtils, normalize )
{
   std::string outDir;

   FilesystemUtils::normalize( "", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), outDir );

   FilesystemUtils::normalize( "/", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), outDir );

   FilesystemUtils::normalize( "/dupa", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/dupa" ), outDir );

   FilesystemUtils::normalize( "/ula/ala.txt", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ula/ala.txt" ), outDir );

   FilesystemUtils::normalize( "\\", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/" ), outDir );

   FilesystemUtils::normalize( "\\dupa", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/dupa" ), outDir );

   FilesystemUtils::normalize( "\\ula\\ala.txt", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ula/ala.txt" ), outDir );

   FilesystemUtils::normalize( "ala\\ula\\ala.txt", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ala/ula/ala.txt" ), outDir );

   FilesystemUtils::normalize( "\\ala\\ula\\ala.txt", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ala/ula/ala.txt" ), outDir );

   FilesystemUtils::normalize( "/ala//ula/ala.txt", outDir );
   CPPUNIT_ASSERT_EQUAL( std::string( "/ala/ula/ala.txt" ), outDir );
}

///////////////////////////////////////////////////////////////////////////////
