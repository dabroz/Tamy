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

   bool engineDirStored = false;

   const std::vector< FilePath >& dirs = section.getSectionDirectories();
   uint count = dirs.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_projectPaths += dirs[i].c_str();
      m_projectPaths += ";";

      if ( dirs[i] == s_engineDir )
      {
         engineDirStored = true;
      }
   }

   // make sure the engine directory and the project directory always make it to project settings
   if ( !engineDirStored )
   {
      m_projectPaths += s_engineDir;
      m_projectPaths += ";";

      FilePath projectDir;
      getFilePath().extractDir( projectDir );
      m_projectPaths += projectDir;
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
