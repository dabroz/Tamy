#include "core\HierarchicalFilesDeleter.h"
#include "core\FilePath.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

HierarchicalFilesDeleter::HierarchicalFilesDeleter( const std::string& rootDir )
   : m_rootDir( rootDir )
{}

///////////////////////////////////////////////////////////////////////////////

void HierarchicalFilesDeleter::perform()
{
   for ( std::list< DeletionDesc >::const_iterator it = m_deletionList.begin();
         it != m_deletionList.end(); ++it )
   {
      const DeletionDesc& desc = *it;
      if ( desc.isDir )
      {
         RemoveDirectory( desc.path.c_str() );
      }
      else
      {
         DeleteFile( desc.path.c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void HierarchicalFilesDeleter::onDirectory( const FilePath& name )
{
   m_deletionList.push_front( DeletionDesc( m_rootDir + name.getRelativePath(), true ) );
}

///////////////////////////////////////////////////////////////////////////////

void HierarchicalFilesDeleter::onFile( const FilePath& name )
{
   m_deletionList.push_front( DeletionDesc( m_rootDir + name.getRelativePath(), false ) );
}

///////////////////////////////////////////////////////////////////////////////
