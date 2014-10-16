/// @file   core\FilesystemSnapshotScanner.h
/// @brief  a scanner that creates a snapshot of the filesystem
#pragma once

#include "core\MemoryRouter.h"
#include "core\FilesystemScanner.h"


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class FilesystemSnapshot;

///////////////////////////////////////////////////////////////////////////////

class FilesystemSnapshotScanner : public FilesystemScanner
{
   DECLARE_ALLOCATOR( FilesystemSnapshotScanner, AM_DEFAULT );

private:
   Filesystem&                m_fs;
   FilesystemSnapshot&        m_snapshot;

public:
   /**
    * Constructor.
    *
    * @param fs
    * @param snapshot
    */
   FilesystemSnapshotScanner( Filesystem& fs, FilesystemSnapshot& snapshot );

   // -------------------------------------------------------------------------
   // FilesystemScanner implementation
   // -------------------------------------------------------------------------
   void onDirectory( const FilePath& name ) override;
   void onFile( const FilePath& name ) override;

private:
   void addFilesystemPath( const FilePath& name );
};

///////////////////////////////////////////////////////////////////////////////
