/// @file   core\FilesystemScanner.h
/// @brief  a filesystem scanner interface
#pragma once 


///////////////////////////////////////////////////////////////////////////////

class FilePath;

///////////////////////////////////////////////////////////////////////////////

class FilesystemScanner
{
public:
   virtual ~FilesystemScanner() {}

   /**
   * Called when a directory is encountered during the scanning process.
   * The name will always end with a '/' character.
   *
   * @param name    relative path to the directory (along with that directory name)
   */
   virtual void onDirectory( const FilePath& name ) {}

   /**
   * Called when a file is encountered during the scanning process.
   *
   * @param name    relative path to the file (along with that file name)
   */
   virtual void onFile( const FilePath& name ) {}
};

///////////////////////////////////////////////////////////////////////////////
