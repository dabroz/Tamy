#include "core\ResourceDependenciesGraph.h"
#include "core\ListUtils.h"

// resources
#include "core\Resource.h"

// logging
#include "core\Log.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

ResourceDependenciesGraph::~ResourceDependenciesGraph()
{
   clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::clear()
{
   for ( List< Entry* >::iterator it = m_files.begin(); !it.isEnd(); ++it )
   {
      Entry* entry = *it;
      delete entry;
   }
   m_files.clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::clearDependencies( const FilePath& file )
{
   List< Entry* >::iterator it = ListUtils::find< Entry*, FilePath, Entry >( m_files, file );
   if ( !it.isEnd() )
   {
      Entry* entry = *it;
      entry->m_affectedResources.clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::addDependency( const FilePath& file, const FilePath& affectedResource )
{
   List< Entry* >::iterator filesIt = ListUtils::find< Entry*, FilePath, Entry >( m_files, file );

   Entry* entry = NULL;
   if ( filesIt.isEnd() )
   {
      // this is a brand new entry
      entry = new Entry( file );
      m_files.pushBack( entry );
   }
   else
   {
      // the entry already exists - check if it's up to date
      entry = *filesIt;
   }

   // check if the entry doesn't already contain this dependency
   List< FilePath >::iterator entryIt = ListUtils::find( entry->m_affectedResources, affectedResource );
   if ( entryIt.isEnd() )
   {
      entry->m_affectedResources.pushBack( affectedResource );
   }

}

///////////////////////////////////////////////////////////////////////////////

const List< FilePath >& ResourceDependenciesGraph::getAffectedResources( const FilePath& file ) const
{
   List< Entry* >::const_iterator it = ListUtils::find< Entry*, FilePath, Entry >( m_files, file );

   if ( it.isEnd() )
   {
      // there's no entry for the requested file - it probably doesn't exist, but just in case, let's log it
      LOG( "ResourceDependenciesTree: Requested file '%s' hasn't been mapped", file.getRelativePath().c_str() );
      return m_noDependencies;
   }
   else
   {
      return it->m_affectedResources;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::onFileRemoved( const FilePath& file )
{
   if ( Resource::isResource( file.extractExtension() ) )
   {
      // scan the remaining entries in search for the file if the removed file is a resource
      for ( List< Entry* >::iterator filesIt = m_files.begin(); !filesIt.isEnd(); ++filesIt )
      {
         Entry* entry = *filesIt;
         List< FilePath >::iterator entryIt = ListUtils::find( entry->m_affectedResources, file );
         if ( !entryIt.isEnd() )
         {
            entryIt.markForRemoval();
         }
      }
   }
   else
   {
      // this is not a resource, so there may exist a dedicated entry that needs to be removed
      List< Entry* >::iterator it = ListUtils::find< Entry*, FilePath, Entry >( m_files, file );
      if ( !it.isEnd() )
      {
         delete *it;
         it.markForRemoval();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::onFileRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   if ( Resource::isResource( oldPath.extractExtension() ) )
   {
      // scan the remaining entries in search for the file if the removed file is a resource
      for ( List< Entry* >::iterator filesIt = m_files.begin(); !filesIt.isEnd(); ++filesIt )
      {
         Entry* entry = *filesIt;
         List< FilePath >::iterator entryIt = ListUtils::find( entry->m_affectedResources, oldPath );
         if ( !entryIt.isEnd() )
         {
            *entryIt = newPath;
         }
      }
   }
   else
   {
      // this is not a resource, so there may exist a dedicated entry that needs to be removed
      List< Entry* >::iterator it = ListUtils::find< Entry*, FilePath, Entry >( m_files, oldPath );
      if ( !it.isEnd() )
      {
         it->m_file = newPath;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::onDirRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   // we need to go through all entries and check which may be affected
   for ( List< Entry* >::iterator filesIt = m_files.begin(); !filesIt.isEnd(); ++filesIt )
   {
      Entry* entry = *filesIt;
      entry->m_file.swapPart( oldPath.getRelativePath(), newPath.getRelativePath(), entry->m_file );
   
      for ( List< FilePath >::iterator entryIt = entry->m_affectedResources.begin(); !entryIt.isEnd(); ++entryIt )
      {
         FilePath& dependencyPath = *entryIt;
         dependencyPath.swapPart( oldPath.getRelativePath(), newPath.getRelativePath(), dependencyPath );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDependenciesGraph::onDirRemoved( const FilePath& path )
{
   // we need to go through all entries and check which may be affected
   for ( List< Entry* >::iterator filesIt = m_files.begin(); !filesIt.isEnd(); ++filesIt )
   {
      Entry* entry = *filesIt;
      if ( entry->m_file.isLocatedInDir( path ) )
      {
         // remove the entire entry
         delete entry;
         filesIt.markForRemoval();

         continue;
      }
      
      // the entry stays, but check the dependencies
      for ( List< FilePath >::iterator entryIt = entry->m_affectedResources.begin(); !entryIt.isEnd(); ++entryIt )
      {
         if ( ( *entryIt ).isLocatedInDir( path ) )
         {
            entryIt.markForRemoval();
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ResourceDependenciesGraph::Entry::isEqual( const Entry* entry, const FilePath& file )
{
   return entry->m_file == file;
}

///////////////////////////////////////////////////////////////////////////////
