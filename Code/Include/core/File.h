/// @file   core\File.h
/// @brief  a single file in a filesystem
#pragma once

#include <string>
#include <iostream>
#include <windows.h>
#include "core/MemoryRouter.h"
#include "core/FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class FilePath;

///////////////////////////////////////////////////////////////////////////////

/**
 * Representation of a file.
 */
class File
{
   DECLARE_ALLOCATOR( File, AM_DEFAULT );

private:
   Filesystem&                m_hostFS;
   FILE*                      m_file;
   FilePath                   m_name;
   std::ios_base::open_mode   m_openMode;

public:
   ~File();

   /**
    * Checks if the file was successfully opened.
    */
   inline bool isOpened() const { return m_file != NULL; }

   /**
    * Returns the name of the file.
    *
    * @return     name of the file
    */
   inline const FilePath& getName() const { return m_name; }

   /**
    * The method sets new offset from which file operation will be performed.
    *
    * @param offset  position in a file
    * @param dir     position with respect to which the offset should be set
    */
   void seek( DWORD offset, std::ios_base::seekdir dir = std::ios_base::beg );

   /**
    * The method returns current offset in the file.
    *
    * @return        current position in a file
    */
   std::size_t tell() const;

   /**
    * The method reads a block of data from the file.
    *
    * @param buffer  buffer where the read data will be stored
    * @param size    amount of data (in bytes) that should be read
    * @param return  amount of bytes read
    */
   std::size_t read( byte* buffer, std::size_t size );

   /**
    * The method writes a block of data to the file.
    *
    * @param buffer  buffer data from which will be written to file
    * @param size    amount of data (in bytes) that should be written 
    *                from the buffer
    * @param return  amount of bytes stored in the file
    */
   std::size_t write( byte* buffer, std::size_t size );

   /**
    * Reads a null-terminated string from the file.
    *
    * @param outStrData    this is where the read string will be stored. This
    *                      buffer needs to exist and be ready to store
    *                      at least 'size' characters.
    * @param size          Maximum number of characters to be read 
    *                      (including the final null-character). Usually,
    *                      the length of the array passed as str is used.
    */
   void readString( char*  outStrData, std::size_t size );

   /**
    * Writes a null-terminated string to a file.
    *
    * @param strData                string we want to store in a file
    * @throws std::runtime_error    if the operation failed
    */
   void writeString( const char* strData );

   /**
    * The method flushes all buffered data to the underlying disc file.
    */
   void flush();

   /**
    * The method checks if we reached the end of file.
    *
    * @return     'true' if the end of file's been reached, 'false' otherwise
    */
   bool eof() const;

   /**
    * The method returns the size of file (in bytes).
    * 
    * @return     file size (in bytes)
    */
   std::size_t size() const;

   /**
    * The method resizes the file to the specified size.
    *
    * @param newSize    requested file size
    * @throws std::runtime_error    if the operation failed
    */
   void setSize( std::size_t newSize );

protected:
   friend class Filesystem;

   /**
    * Constructor.
    *
    * @param hostFS     host file system
    * @param name       file name
    * @param openMode   file access privileges
    */
   File( Filesystem& hostFS, const FilePath& name, const std::ios_base::open_mode openMode = std::ios_base::in );

};

///////////////////////////////////////////////////////////////////////////////
