/// @file   core\ResourceDependenciesGraph.h
/// @brief  a structure that holds a graph of dependencies between files
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\FilePath.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class ResourceDependenciesGraph
{
   DECLARE_ALLOCATOR( ResourceDependenciesGraph, AM_DEFAULT );

public:
   struct Entry
   {
      DECLARE_ALLOCATOR( Entry, AM_DEFAULT );

      FilePath                m_file;

      List< FilePath >        m_affectedResources;

      Entry( const FilePath& file = FilePath() )
         : m_file( file )
      {
      }

      // ----------------------------------------------------------------------
      // Comparator implementation
      // ----------------------------------------------------------------------
      static bool isEqual( const Entry* entry, const FilePath& file );
   };

private:
   List< Entry* >             m_files;
   List< FilePath >           m_noDependencies; // an empty list to return in case an entry for the file requested in a call to getAffectedResources doesn't exist

public:
   ~ResourceDependenciesGraph();

   /**
   * Removes all entries from the tree.
   */
   void clear();

   /**
    * Removes the dependencies for the specified file.
    *
    * @param file
    */
   void clearDependencies( const FilePath& file );

   /**
    * Adds a new dependency between the files.
    *
    * @param file
    * @param affectedResource
    */
   void addDependency( const FilePath& file, const FilePath& affectedResource );

   /**
    * Returns a list of resources affected by a change in the specified file.
    *
    * @param file
    */
   const List< FilePath >& getAffectedResources( const FilePath& file ) const;

   /**
    * Called when a file ( that doesn't necessarily belong to this graph ) is removed.
    *
    * @param file
    */
   void onFileRemoved( const FilePath& file );

   /**
    * Called when a file ( that doesn't necessarily belong to this graph ) has changed its name.
    *
    * @param oldPath
    * @param newPath
    */
   void onFileRenamed( const FilePath& oldPath, const FilePath& newPath );

   /**
    * Called when a directory that may store some of the files that belong to this graph has changed its name.
    *
    * @param oldPath
    * @param newPath
    */
   void onDirRenamed( const FilePath& oldPath, const FilePath& newPath );

   /**
   * Called when a directory that may store some of the files that belong to this graph is removed.
   *
   * @param path
   */
   void onDirRemoved( const FilePath& path );
};

///////////////////////////////////////////////////////////////////////////////
