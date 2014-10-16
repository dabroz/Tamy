#include "core-TestFramework\TestFramework.h"
#include "core\ResourceDependenciesGraph.h"
#include "core\OutArrayStream.h"
#include "core\InArrayStream.h"


///////////////////////////////////////////////////////////////////////////////

TEST( ResourceDependenciesGraph, basics )
{
   ResourceDependenciesGraph graph;

   FilePath res1( "a.res" );
   FilePath res2( "b.res" );
   graph.addDependency( res1, res2 );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( res1 ).size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( res2 ).size() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ResourceDependenciesGraph, renamingFile )
{
   ResourceDependenciesGraph graph;

   FilePath file( "a.res" );
   FilePath renamedFile( "a_renamed.res" );
   FilePath res( "b.tmat" );
   FilePath renamedRes( "b_renamed.tmat" );
   graph.addDependency( file, res );

   graph.onFileRenamed( file, renamedFile );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( renamedFile ).size() );
   CPPUNIT_ASSERT_EQUAL( res.getRelativePath(), graph.getAffectedResources( renamedFile ).front().getRelativePath() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( res ).size() );

   graph.onFileRenamed( res, renamedRes );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( renamedFile ).size() );
   CPPUNIT_ASSERT_EQUAL( renamedRes.getRelativePath(), graph.getAffectedResources( renamedFile ).front().getRelativePath() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( res ).size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( renamedRes ).size() );

}

///////////////////////////////////////////////////////////////////////////////

TEST( ResourceDependenciesGraph, removingFile )
{
   ResourceDependenciesGraph graph;

   FilePath file( "a.res" );
   FilePath res1( "b.tmat" );
   FilePath res2( "c.tmat" );
   graph.addDependency( file, res1 );
   graph.addDependency( file, res2 );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, graph.getAffectedResources( file ).size() );

   graph.onFileRemoved( res1 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( file ).size() );

   graph.onFileRemoved( res2 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );

   graph.addDependency( file, res1 );
   graph.addDependency( file, res2 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, graph.getAffectedResources( file ).size() );

   graph.onFileRemoved( file );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( ResourceDependenciesGraph, removingDirectory )
{
   ResourceDependenciesGraph graph;

   FilePath file( "/directory1/a.res" );
   FilePath renamedFile( "/renamedDirectory1/a.res" );
   FilePath res( "/directory2/b.tmat" );
   FilePath renamedRes( "/renamedDirectory2/b.tmat" );
   FilePath dir1( "/directory1" );
   FilePath renmamedDir1( "/renamedDirectory1" );
   FilePath dir2( "/directory2" );
   FilePath renmamedDir2( "/renamedDirectory2" );

   graph.addDependency( file, res );

   graph.onDirRenamed( dir1, renmamedDir1 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( renamedFile ).size() );
   CPPUNIT_ASSERT_EQUAL( res.getRelativePath(), graph.getAffectedResources( renamedFile ).front().getRelativePath() );

   graph.onDirRenamed( dir2, renmamedDir2 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( renamedFile ).size() );
   CPPUNIT_ASSERT_EQUAL( renamedRes.getRelativePath(), graph.getAffectedResources( renamedFile ).front().getRelativePath() );
}


///////////////////////////////////////////////////////////////////////////////

TEST( ResourceDependenciesGraph, removingDir )
{
   ResourceDependenciesGraph graph;

   FilePath file( "/directory1/a.res" );
   FilePath res1( "/directory2/b.tmat" );
   FilePath res2( "/directory3/c.tmat" );
   FilePath dir1( "/directory1" );
   FilePath dir2( "/directory2" );
   FilePath dir3( "/directory3" );
   graph.addDependency( file, res1 );
   graph.addDependency( file, res2 );

   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, graph.getAffectedResources( file ).size() );

   graph.onDirRemoved( dir2 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 1, graph.getAffectedResources( file ).size() );
   CPPUNIT_ASSERT_EQUAL( res2.getRelativePath(), graph.getAffectedResources( file ).front().getRelativePath() );

   graph.onDirRemoved( dir3 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );

   graph.addDependency( file, res1 );
   graph.addDependency( file, res2 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 2, graph.getAffectedResources( file ).size() );

   graph.onDirRemoved( dir1 );
   CPPUNIT_ASSERT_EQUAL( ( uint ) 0, graph.getAffectedResources( file ).size() );
}

///////////////////////////////////////////////////////////////////////////////
