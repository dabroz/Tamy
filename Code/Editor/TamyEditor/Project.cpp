#include "Project.h"
#include "core/FilesystemSection.h"
#include "core/StringUtils.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

FilePath Project::s_engineDir( "/Renderer/" );

///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Project, tpr, AM_BINARY )
   PROPERTY( std::string, m_projectPaths )
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

Project::Project( const FilePath& filePath )
   : Resource( filePath )
   , m_fsSection( NULL )
{
   m_fsSection = new FilesystemSection( TSingleton< ResourcesManager >::getInstance().getFilesystem() );
}

///////////////////////////////////////////////////////////////////////////////

Project::~Project()
{
   delete m_fsSection;
   m_fsSection = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Project::replaceContents( Resource& rhs )
{
   Project& rhsProject = static_cast< Project& >( rhs );
   m_projectPaths = rhsProject.m_projectPaths;
}

///////////////////////////////////////////////////////////////////////////////

void Project::onResourceSave( ResourcesManager& mgr )
{
   if ( m_fsSection )
   {
      set( *m_fsSection );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Project::onResourceLoaded( ResourcesManager& mgr )
{
   if ( m_fsSection )
   {
      delete m_fsSection;
      m_fsSection = NULL;
   }

   m_fsSection = new FilesystemSection( mgr.getFilesystem() );
   store( *m_fsSection );
}

///////////////////////////////////////////////////////////////////////////////

void Project::set( const FilesystemSection& section )
{
   m_projectPaths = "";

   const std::vector< FilePath >& dirs = section.getSectionDirectories();
   uint count = dirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      // just make sure not to add the outstanding, runtime dirs
      if ( isRuntimeDir( dirs[i] ) )
      {
         continue;
      }

      m_projectPaths += dirs[i].c_str();
      m_projectPaths += ";";
   }
}

///////////////////////////////////////////////////////////////////////////////

void Project::store( FilesystemSection& outSection )
{
   Array< std::string > dirs;
   StringUtils::tokenize( m_projectPaths, ";", dirs );

   uint count = dirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      outSection.addDirectory( FilePath( dirs[i] ) );
   }

   ensureMainDirectoriesPresence();
}

///////////////////////////////////////////////////////////////////////////////

bool Project::isMember( const FilePath& path ) const
{
   return m_fsSection->isMember( path );
}

///////////////////////////////////////////////////////////////////////////////

void Project::addDirectory( const FilePath& dir )
{
   m_fsSection->addDirectory( dir );
}

///////////////////////////////////////////////////////////////////////////////

void Project::removeDirectory( const FilePath& dir )
{
   m_fsSection->removeDirectory( dir );

   // one of the required directories, or its parent might have been removed - so just re-add them
   ensureMainDirectoriesPresence();
}

///////////////////////////////////////////////////////////////////////////////

void Project::collectDirectories( std::vector< FilePath >& outProjectDirectories )
{
   const std::vector< FilePath >& dirs = m_fsSection->getSectionDirectories();
   uint count = dirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      outProjectDirectories.push_back( dirs[i] );
   }

   // add the outstanding directories
   {
      outProjectDirectories.push_back( s_engineDir );

      FilePath projectDir;
      getFilePath().extractDir( projectDir );
      outProjectDirectories.push_back( projectDir );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Project::ensureMainDirectoriesPresence()
{
   // and make sure the engine dir and the project dir is there as well
   m_fsSection->addDirectory( s_engineDir );

   FilePath projectDir;
   getFilePath().extractDir( projectDir );
   m_fsSection->addDirectory( projectDir );
}

///////////////////////////////////////////////////////////////////////////////

bool Project::isRuntimeDir( const std::string& dir ) const
{
   if ( s_engineDir == dir )
   {
      return true;
   }

   FilePath projectDir;
   getFilePath().extractDir( projectDir );

   if ( projectDir == dir )
   {
      return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
