#include "FSRootNode.h"
#include "FilesystemTree.h"


///////////////////////////////////////////////////////////////////////////////

FSRootNode::FSRootNode( QTreeWidget* parent, const char* rootDirectory )
   : FSTreeNode( parent )
   , m_fsNodeName( rootDirectory )
{
   setIcon( 0, QIcon( ":/TamyEditor/Resources/Icons/Editor/dirIcon.png" ) );

   setText( 0, "Root" );
   setText( 1, "" );
}

///////////////////////////////////////////////////////////////////////////////

FilePath FSRootNode::getRelativePath() const
{
   return FilePath( m_fsNodeName );
}

///////////////////////////////////////////////////////////////////////////////

TreeWidgetDescFactory* FSRootNode::getDescFactory( FilesystemTree& resourcesFactory )
{
   return resourcesFactory.getDescFactory();
}

///////////////////////////////////////////////////////////////////////////////

void FSRootNode::addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory )
{
   std::string path = getRelativePath();
   return resourcesFactory.addNode( typeIdx, path );
}

///////////////////////////////////////////////////////////////////////////////

void FSRootNode::insertNodes( int insertionIndex, const Array< FilePath >& paths )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   const uint count = paths.size();
   for ( uint i = 0; i < count; ++i )
   {
      const FilePath& oldPath = paths[i];
      FilePath newPath = getRelativePath();

      newPath += oldPath.extractNodeName();
      fs.rename( oldPath, newPath );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool FSRootNode::compareNodeName( const std::string& name ) const
{
   return name.empty() || name == m_fsNodeName;
}

///////////////////////////////////////////////////////////////////////////////
