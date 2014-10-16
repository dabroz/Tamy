/// @file   core/FilesystemUtils.h
/// @brief  filesystem utilities toolkit
#pragma once

#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * This class contains methods for operating files and directories
 * on your hard drive, not necessarily located in your active filesystem.
 *
 * Just a generic file & directory handling tool.
 */
class FilesystemUtils
{
public:
   // -------------------------------------------------------------------------
   // Paths manipulations
   // -------------------------------------------------------------------------
   /**
    * Normalizes the file path - setting proper directories separators etc.
    *
    * @param fileName
    * @param outFileName
    */
   static void normalize( const std::string& fileName, std::string& outFileName );

   /**
    * Extracts the name of a directory in which the specified file is located.
    *
    * @param fileName
    */
   static std::string extractDir( const std::string& fileName );


   /**
    * The method allows to move up in the directories hierarchy by the specified
    * amount of levels.
    *
    * i.e.: A call like:
    *    leaveDir( "d:/dirA/dirB/dirC/dirD", 2, outDir );
    * initializes outDir with:
    *    "d:/dirA/dirB"
    *
    * @param dir
    * @param levels
    * @param outDirectory
    */
   static void leaveDir( const std::string& dir, unsigned int levels, std::string& outDirectory );

   /**
    * Just as the 'extractDir' method extracts a name of a directory
    * in which the specified file is located, this method extracts
    * the sole name of the file.
    *
    * @param fileName   path to a file along with that file name.
    */
   static std::string extractNodeName( const std::string& fileName );

   /**
    * Extracts an extension of the specified filename.
    *
    * @param fileName
    */
   static std::string extractExtension( const std::string& fileName );

   // -------------------------------------------------------------------------
   // Files manipulations
   // -------------------------------------------------------------------------
   /**
    * Creates the specified directory ( and all the directories necessary to create this one ).
    * An absolute path should be specified.
    *
    * @param path
    */
   static void mkdir( const char* path );

   /**
    * Copies a file or files from to the specified destination. Absolute paths should be specified.
    *
    * @param sourcePath
    * @param destPath
    * @param createDir     should the method automatically create any missing directories?
    *
    * @return  operation status
    */
   static bool copyFile( const char* sourcePath, const char* destPath, bool createDir = true );

   /**
    * Removes the specified file or directory. An absolute path should be specified.
    *
    * @param path
    *
    * @return operation status
    */
   static bool removeFile( const char* path );

   /**
   * Renames a file or a directory. Absolute paths should be specified.
   *
   * @param oldPath
   * @param newPath
   *
   * @return  operation status
   */
   static bool renameFileDir( const char* oldPath, const char* newPath );

   /**
    * Returns the last modification timestamp of the specified file/dir.
    *
    * @param path
    * @param outTimestampLo
    * @param outTimestampHi
    *
    * @return  operation status
    */
   static bool getTimestamp( const char* path, ulong& outTimestampLo, ulong& outTimestampHi );
};

///////////////////////////////////////////////////////////////////////////////
