#include "SCTIRoot.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core\Filesystem.h"


///////////////////////////////////////////////////////////////////////////////

SCTIRoot::SCTIRoot( Model& scene )
   : SceneTreeItem( true, false, true )
   , m_scene( scene )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

SCTIRoot::~SCTIRoot()
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNode* SCTIRoot::getNode() const
{
   return m_scene.getRoot();
}

///////////////////////////////////////////////////////////////////////////////

bool SCTIRoot::canAccommodate( SceneNode* node ) const
{
   bool isEntity = node->isA< Entity >();
   return isEntity;
}

///////////////////////////////////////////////////////////////////////////////

void SCTIRoot::addNode( SceneNode* newNode )
{
   m_scene.addChild( newNode );
}

///////////////////////////////////////////////////////////////////////////////

void SCTIRoot::removeNode( SceneTreeItem* removedChildItem )
{
   SceneNode* removedNode = removedChildItem->getNode();
   if ( removedNode )
   {
      removedNode->remove();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SCTIRoot::clear()
{
   m_scene.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SCTIRoot::updateDescription()
{
   const FilePath& scenePath = m_scene.getFilePath();
   setText( 0, scenePath.extractNodeName().c_str() );

   QIcon icon( ":/TamyEditor/Resources/Icons/Engine/SceneRootIcon.png" );
   if ( m_scene.getRoot()->m_prefab != NULL )
   {    
      icon.addPixmap( QPixmap( ":/TamyEditor/Resources/Icons/Engine/PrefabAddon.png" ) );
      setIcon( 0, icon );
   }
   else
   {
      setIcon( 0, icon );
   }
}

///////////////////////////////////////////////////////////////////////////////
