#include "FSDirNode.h"
#include "FilesystemTree.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

FSDirNode::FSDirNode( FSTreeNode* parent, const std::string& nodeName )
   : FSTreeNode( parent )
   , m_fsNodeName( nodeName )
{
   // strip the name of the last slash
   std::string label;
   if ( !m_fsNodeName.empty() )
   {
      label = m_fsNodeName.substr( 0, m_fsNodeName.length() - 1 );
   }

   // set the icon
   setIcon( 0, QIcon( ":/TamyEditor/Resources/Icons/Editor/dirIcon.png" ) );

   // set the description
   setText( 0, m_fsNodeName.c_str() );
   setText( 1, "DIR" );
}

///////////////////////////////////////////////////////////////////////////////

FilePath FSDirNode::getRelativePath() const
{
   ASSERT_MSG ( parent(), "Directory node has to have a parent" );

   std::string path = dynamic_cast< FSTreeNode* >( parent() )->getRelativePath();
   path += std::string( "/" ) + m_fsNodeName;

   return FilePath( path );
}

///////////////////////////////////////////////////////////////////////////////

TreeWidgetDescFactory* FSDirNode::getDescFactory( FilesystemTree& resourcesFactory )
{
   return resourcesFactory.getDescFactory();
}

///////////////////////////////////////////////////////////////////////////////

void FSDirNode::addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory )
{
   std::string path = getRelativePath() + std::string("/");
   return resourcesFactory.addNode( typeIdx, path );
}

///////////////////////////////////////////////////////////////////////////////

void FSDirNode::insertNodes( int insertionIndex, const Array< FilePath >& paths )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   const uint count = paths.size();
   for ( uint i = 0; i < count; ++i )
   {
      const FilePath& oldPath = paths[i];   
      FilePath newPath = getRelativePath() + std::string( "/" );

      newPath += oldPath.extractNodeName();
      fs.rename( oldPath, newPath );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FSDirNode::openItem( FilesystemTree& hostTree )
{
}

///////////////////////////////////////////////////////////////////////////////

bool FSDirNode::compareNodeName( const std::string& name ) const
{
   uint len1 = m_fsNodeName.length();
   uint len2 = name.length();

   const char* name1 = m_fsNodeName.c_str();
   const char* name2 = name.c_str();

   if ( len1 != len2 )
   {
      return false;
   }

   return strncmp( name1, name2, len1 ) == 0;
}

///////////////////////////////////////////////////////////////////////////////
