/// @file   core\FilesystemSectionScanner.h
/// @brief  a filesystem scanner used internally by the FilesystemSection class
#pragma once

#include "core\FilesystemScanner.h"


///////////////////////////////////////////////////////////////////////////////

class FilesystemSection;

///////////////////////////////////////////////////////////////////////////////

/**
* A special scanner that allows you to wrap your custom scanner interface and scan
* an arbitrary filesystem with it while filtering out anything that doesn't isn't
* a part of the specified filesystem section.
*/
class FilesystemSectionScanner : public FilesystemScanner
{
private:
   const FilesystemSection&   m_scannedSection;

public:
   /**
   * Constructor.
   *
   * @param scannedSection
   */
   FilesystemSectionScanner( const FilesystemSection& scannedSection );

   // -------------------------------------------------------------------------
   // FilesystemScanner implementation
   // -------------------------------------------------------------------------
   void onDirectory( const FilePath& path ) override;
   void onFile( const FilePath& path ) override;

protected:
   /**
   * Called when a directory that's part of the scanned section is encountered.
   *
   * @param path
   */
   virtual void onSectionDirectory( const FilePath& path ) {}

   /**
   * Called when a file that's part of the scanned section is encountered.
   *
   * @param path
   */
   virtual void onSectionFile( const FilePath& path ) {}
};

///////////////////////////////////////////////////////////////////////////////
