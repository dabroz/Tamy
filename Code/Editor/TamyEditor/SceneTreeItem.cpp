#include "SceneTreeItem.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

SceneTreeItem::SceneTreeItem( bool canStoreEntities, bool canStoreComponents, bool canBeTurnedIntoPrefab )
   : m_canStoreEntities( canStoreEntities )
   , m_canStoreComponents( canStoreComponents )
   , m_canBeTurnedIntoPrefab( canBeTurnedIntoPrefab )
{
}

///////////////////////////////////////////////////////////////////////////////

SceneTreeItem::~SceneTreeItem()
{
}

///////////////////////////////////////////////////////////////////////////////

void SceneTreeItem::updateDescription()
{
   SceneNode* node = getNode();
   setText( 0, node->getSceneNodeName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

bool SceneTreeItem::canBeTurnedIntoPrefab() const 
{ 
   if ( !m_canBeTurnedIntoPrefab )
   {
      return false;
   }
   
   const Entity* entity = static_cast< const Entity* >( getNode() );
   bool doesntUsePrefab = ( entity->m_prefab == NULL );

   return doesntUsePrefab;
}

///////////////////////////////////////////////////////////////////////////////
