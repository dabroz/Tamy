/// @file   core\HierarchicalFilesDeleter.h
/// @brief  a scanner used internally by the file system to perform hierarchical deletion of the entire directory structure
#pragma once

#include "core\FilesystemScanner.h"
#include <string>
#include <list>


///////////////////////////////////////////////////////////////////////////////

class HierarchicalFilesDeleter : public FilesystemScanner
{
private:
   struct DeletionDesc
   {
      std::string path;
      bool        isDir;

      DeletionDesc( const std::string& _path, bool _isDir )
         : path( _path )
         , isDir( _isDir )
      {}
   };

private:
   std::string                m_rootDir;
   std::list< DeletionDesc >  m_deletionList;

public:
   HierarchicalFilesDeleter( const std::string& rootDir );

   void perform();

   // ----------------------------------------------------------------------
   // FilesystemScanner
   // ----------------------------------------------------------------------
   void onDirectory( const FilePath& name ) override;
   void onFile( const FilePath& name ) override;
};

///////////////////////////////////////////////////////////////////////////////
