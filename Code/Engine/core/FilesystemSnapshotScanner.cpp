#include "core\FilesystemSnapshotScanner.h"
#include "core\FilesystemSnapshot.h"
#include "core\FilesystemUtils.h"
#include "core\Log.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

FilesystemSnapshotScanner::FilesystemSnapshotScanner( Filesystem& fs, FilesystemSnapshot& snapshot )
   : m_fs( fs )
   , m_snapshot( snapshot )
{
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshotScanner::onDirectory( const FilePath& name )
{
   addFilesystemPath( name );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshotScanner::onFile( const FilePath& name )
{
   addFilesystemPath( name );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshotScanner::addFilesystemPath( const FilePath& name )
{
   ulong timestampLo, timestampHi;
   std::string absolutePath = name.toAbsolutePath( m_fs );
   FilesystemUtils::getTimestamp( absolutePath.c_str(), timestampLo, timestampHi );
   m_snapshot.add( name, timestampLo, timestampHi );
}

///////////////////////////////////////////////////////////////////////////////
