#include "core\FilesystemSectionScanner.h"
#include "core\FilesystemSection.h"


///////////////////////////////////////////////////////////////////////////////

FilesystemSectionScanner::FilesystemSectionScanner( const FilesystemSection& scannedSection )
   : m_scannedSection( scannedSection )
{}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSectionScanner::onDirectory( const FilePath& path )
{
   // check if the directory is allowed
   if ( m_scannedSection.isMember( path ) )
   {
      onSectionDirectory( path );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSectionScanner::onFile( const FilePath& path )
{
   // check if the directory is allowed
   if ( m_scannedSection.isMember( path ) )
   {
      onSectionFile( path );
   }
}

///////////////////////////////////////////////////////////////////////////////
