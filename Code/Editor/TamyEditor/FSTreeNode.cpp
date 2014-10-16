#include "FSTreeNode.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

FSTreeNode::FSTreeNode( FSTreeNode* parent )
: QTreeWidgetItem( parent )
{
}

///////////////////////////////////////////////////////////////////////////////

FSTreeNode::FSTreeNode( QTreeWidget* parent )
: QTreeWidgetItem( parent )
{
}

///////////////////////////////////////////////////////////////////////////////

FSTreeNode* FSTreeNode::find( const std::string& nodeName )
{
   int count = childCount();
   for( int i = 0; i < count; ++i )
   {
      FSTreeNode* entry = static_cast< FSTreeNode* >( child( i ) );
      if ( entry && entry->compareNodeName( nodeName ) )
      {
         return entry;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeNode::setName( const QString& newName )
{
   FilePath oldPath( getRelativePath() );
   FilePath elemNewName( newName.toStdString().c_str() );

   FilePath newPath;
   oldPath.extractDir( newPath );
   newPath += elemNewName;

   if ( newPath == oldPath )
   {
      // nothing changes
      return;
   }

   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   fs.rename( oldPath, newPath );
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeNode::removeNode( FSTreeNode* child )
{
   FilePath path( child->getRelativePath() );

   ResourcesManager& rm = TSingleton< ResourcesManager >::getInstance();
   rm.deleteResource( path );
   
   removeChild( child );
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeNode::clearNodes()
{
   int count = childCount();
   for( int i = 0; i < count; ++i )
   {
      removeNode( static_cast< FSTreeNode* >( child( 0 ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeNode::clear()
{
   int count = childCount();
   for( int i = 0; i < count; ++i )
   {
      removeChild( child( 0 ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
