/// @file   core/FilesystemSnapshot.h
/// @brief  a snapshot of the filesystem's state
#pragma once

#include "core\MemoryRouter.h"
#include <string>
#include "core\List.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class FilePath;

///////////////////////////////////////////////////////////////////////////////

struct FilesystemEntry
{
   DECLARE_ALLOCATOR( FilesystemEntry, AM_DEFAULT );

   std::string                   m_name;
   FilePath                      m_path;
   bool                          m_isDir;
   ulong                         m_timestampLo;
   ulong                         m_timestampHi;

   List< FilesystemEntry* >      m_children;

   /**
   * Constructor.
   *
   * @param path
   * @param isDir
   * @param timestampLo
   * @param timestampHi
   */
   FilesystemEntry( const FilePath& path = FilePath(), bool isDir = false, ulong timestampLo = 0, ulong timestampHi = 0 );
   ~FilesystemEntry();

   // ----------------------------------------------------------------------
   // Comparator implementation
   // ----------------------------------------------------------------------
   static bool isEqual( FilesystemEntry* entry, const std::string& name );
   static bool isEqual( const FilesystemEntry* entry, const FilePath& path );
   static bool isLesser( FilesystemEntry* rhs, FilesystemEntry* lhs );
};

///////////////////////////////////////////////////////////////////////////////

struct FilesystemChanges
{
   DECLARE_ALLOCATOR( FilesystemChanges, AM_DEFAULT );

   List< const FilesystemEntry* >      m_addedFiles;
   List< const FilesystemEntry* >      m_removedFiles;
   List< const FilesystemEntry* >      m_modifiedFiles;
};

///////////////////////////////////////////////////////////////////////////////

class FilesystemSnapshot
{
   DECLARE_ALLOCATOR( FilesystemSnapshot, AM_DEFAULT );

private:
   FilesystemEntry*        m_root;
   Filesystem&             m_fs;

public:
   /**
    * Constructor.
    */
   FilesystemSnapshot( Filesystem& fs );
   ~FilesystemSnapshot();

   /**
    * Adds a new path to the snapshot.
    *
    * @param path
    * @param timestampLo
    * @param timestampHi
    */
   void add( const FilePath& path, ulong timestampLo, ulong timestampHi );

   /**
    * Removes a path from the snapshot.
    *
    * @param path
    */
   void remove( const FilePath& path );

   /**
    * Returns an entry that corresponds to the specified path.
    *
    * @param path
    */
   const FilesystemEntry* getEntry( const FilePath& path ) const;

   /**
    * Returns an entry that corresponds to the specified path ( non-const).
    *
    * @param path
    */
   FilesystemEntry* accessEntry( const FilePath& path );

   /**
    * Compares two snapshots, returning the differences.
    *
    * @param lhs
    * @param outChanges
    */
   void compare( const FilesystemSnapshot& lhs, FilesystemChanges& outChanges ) const;

   /**
    * Swaps the contents of two snapshots, making it easier to apply the changes
    * from the fresh snapshot to the one we are keeping for reference.
    *
    * @param rhs
    */
   void swapContents( FilesystemSnapshot& rhs );

   /**
    * Returns a text representation of the snapshot.
    */
   std::string toString() const;

private:
   void deleteEntry( FilesystemEntry* parentEntry, FilesystemEntry* toRemove );

   void addSubtree( const FilesystemEntry* entry, List< const FilesystemEntry* >& outEntries ) const;
};

///////////////////////////////////////////////////////////////////////////////
