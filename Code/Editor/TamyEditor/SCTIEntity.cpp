#include "SCTIEntity.h"
#include "core-MVC\Entity.h"
#include "core\Filesystem.h"
#include <QtGui\QPainter>


///////////////////////////////////////////////////////////////////////////////

SCTIEntity::SCTIEntity( Entity& entity )
   : SceneTreeItem( true, true, true )
   , m_entity( entity )
{
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

SCTIEntity::~SCTIEntity()
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNode* SCTIEntity::getNode() const
{
   return &m_entity;
}

///////////////////////////////////////////////////////////////////////////////

bool SCTIEntity::canAccommodate( SceneNode* node ) const
{
   return true;
}

///////////////////////////////////////////////////////////////////////////////

void SCTIEntity::addNode( SceneNode* newNode )
{
   m_entity.addChild( newNode );
}

///////////////////////////////////////////////////////////////////////////////

void SCTIEntity::removeNode( SceneTreeItem* removedChildItem )
{
   SceneNode* removedNode = removedChildItem->getNode();
   m_entity.removeChild( removedNode );
}

///////////////////////////////////////////////////////////////////////////////

void SCTIEntity::clear()
{
   m_entity.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SCTIEntity::updateDescription()
{
   SceneNode* node = getNode();
   setText( 0, node->getSceneNodeName().c_str() );

   if ( m_entity.m_prefab != NULL )
   {
      // memorize original icon
      m_originalIcon = icon( 0 ).pixmap( 16, 16 );

      // create the altered icon
      QPixmap newIconPixmap = m_originalIcon;
      QPixmap addonPixmap( ":/TamyEditor/Resources/Icons/Engine/PrefabAddon.png" );

      QPainter painter( &newIconPixmap );
      painter.drawPixmap( 0, 0, addonPixmap ); 

      // set the icon
      setIcon( 0, QIcon( newIconPixmap ) );
   }
   else
   {
      setIcon( 0, QIcon( m_originalIcon ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
