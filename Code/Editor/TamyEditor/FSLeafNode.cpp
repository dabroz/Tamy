#include "FSLeafNode.h"
#include "FilesystemTree.h"

// tools
#include <QtWidgets\QFileIconProvider>
#include "ResourceManagementUtil.h"
#include "TreeWidgetDescFactory.h"
#include "core/Assert.h"

// project management
#include "Project.h"
#include "tamyeditor.h"
#include "ResourcesBrowser.h"


///////////////////////////////////////////////////////////////////////////////

FSLeafNode::FSLeafNode( FSTreeNode* parent, const std::string& nodeName, TreeWidgetDescFactory& itemsFactory )
   : FSTreeNode( parent )
   , m_fsNodeName( nodeName )
{
   Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();

   ASSERT_MSG( m_fsNodeName.empty() ? true : m_fsNodeName.c_str()[ m_fsNodeName.length() - 1 ] != '/', "This is a leaf node, dedicated to a file, thus it can't end with a slash" );

   // set icon
   setEntryIcon( fs, itemsFactory );

   // set description
   setText( 0, m_fsNodeName.c_str() );
   setEntrySize( fs );

}

///////////////////////////////////////////////////////////////////////////////

void FSLeafNode::setEntryIcon( Filesystem& fs, TreeWidgetDescFactory& itemsFactory )
{
   std::string extension = FilesystemUtils::extractExtension( m_fsNodeName );

   QIcon icon;

   const SerializableReflectionType* resourceType = Resource::findResourceClass( extension.c_str() );
   if ( resourceType )
   {
      // as the first shot, try creating an icon corresponding to the type of the resource
      QString typeName, groupName;
      itemsFactory.getDesc( *resourceType, typeName, groupName, icon );
   }

   if ( icon.isNull() )
   {
      // as a last resort, try using a system icon
      QFileIconProvider iconProvider;

      std::string absolutePath = fs.toAbsolutePath( getRelativePath() );
      QFileInfo fileInfo( absolutePath.c_str() );
      icon = iconProvider.icon( fileInfo );
   }

   setIcon( 0, icon );
}

///////////////////////////////////////////////////////////////////////////////

void FSLeafNode::setEntrySize( Filesystem& fs )
{
   std::string relativePath = getRelativePath();

   std::size_t fileSize = 0;
   File* file = fs.open( relativePath );
   if ( file )
   {
      fileSize = file->size();
      delete file;
   }
   else
   {
      fileSize = 0;
   }

   char fileSizeStr[ 32 ];
   sprintf_s( fileSizeStr, "%d", fileSize );
   setText( 1, fileSizeStr );
}

///////////////////////////////////////////////////////////////////////////////

FilePath FSLeafNode::getRelativePath() const
{
   ASSERT_MSG ( parent(), "Leaf node has to have a parent" );

   std::string path = static_cast< FSTreeNode* >( parent() )->getRelativePath();
   path += m_fsNodeName;

   return FilePath( path );
}

///////////////////////////////////////////////////////////////////////////////

TreeWidgetDescFactory* FSLeafNode::getDescFactory( FilesystemTree& resourcesFactory )
{
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void FSLeafNode::addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory )
{
   ASSERT_MSG( false, "This operation should never be performed" );
}

///////////////////////////////////////////////////////////////////////////////

void FSLeafNode::insertNodes( int insertionIndex, const Array< FilePath >& paths )
{
   ASSERT_MSG( false, "This operation should never be performed" );
}

///////////////////////////////////////////////////////////////////////////////

bool FSLeafNode::compareNodeName( const std::string& name ) const
{
   return name == m_fsNodeName;
}

///////////////////////////////////////////////////////////////////////////////

void FSLeafNode::openItem( FilesystemTree& hostTree )
{
   bool isProjectItem = ResourceManagementUtil::isResourceOfType< Project >( m_fsNodeName );
   if ( isProjectItem )
   {
      FilePath path( getRelativePath() );
      Project* project = TSingleton< ResourcesManager >::getInstance().create< Project >( path );

      // we only open projects here, by setting them as the active projects
      TamyEditor::getInstance().setActiveProject( project );
   }
   else
   {
      std::string pathToResource = getRelativePath();
      ResourceManagementUtil::editResource( pathToResource, icon( 0 ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
