#include "core-MVC\EntityUtils.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Entity.h"
#include "core-MVC\Model.h"
#include "core\LocalStack.h"


///////////////////////////////////////////////////////////////////////////////

void EntityUtils::collectChildren( Entity* entity, List< SceneNode* >& outChildren )
{
   List< SceneNode* > nodesQueue;

   // fill the list with this node's immediate children - we'll start the search with them
   uint childrenCount = entity->m_children.size();
   for ( uint i = 0; i < childrenCount; ++i )
   {
      nodesQueue.pushBack( entity->m_children[i] );
   }

   // BFS through the children tree to collect all attached entities
   while( !nodesQueue.empty() )
   {
      SceneNode* node = nodesQueue.front();
      nodesQueue.popFront();

      // store the child in the output array
      outChildren.pushBack( node );

      // get the children of the child and put them down for further analysis
      node->collectChildren( nodesQueue );
   }
}

///////////////////////////////////////////////////////////////////////////////

SceneNode* EntityUtils::findNodeByName( SceneNode* node, const std::string& name )
{
   // notify the view about all entities
   LocalStack< SceneNode* > nodes;
   nodes.push( node );

   while ( !nodes.empty() )
   {
      SceneNode* currNode = nodes.pop();
      if ( currNode->getSceneNodeName() == name )
      {
         return currNode;
      }

      currNode->collectChildren( nodes );
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

Entity* EntityUtils::getFirstPrefabbedParent( SceneNode* node )
{
   Entity* checkedEntity = NULL;
   if ( node->isA< Entity >() )
   {
      checkedEntity = static_cast< Entity* >( node );
   }
   else
   {
      checkedEntity = node->getParent();
   }

   while ( checkedEntity != NULL )
   {
      if ( checkedEntity->m_prefab != NULL )
      {
         // found it
         return checkedEntity;
      }
      checkedEntity = checkedEntity->getParent();
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

Entity* EntityUtils::getTopmostPrefabbedParent( SceneNode* node )
{
   Entity* checkedEntity = NULL;
   if ( node->isA< Entity >() )
   {
      checkedEntity = static_cast< Entity* >( node );
   }
   else
   {
      checkedEntity = node->getParent();
   }

   Entity* topmostPrefabbedEntity = NULL;
   while ( checkedEntity != NULL )
   {
      if ( checkedEntity->m_prefab != NULL )
      {
         topmostPrefabbedEntity = checkedEntity;
      }
      checkedEntity = checkedEntity->getParent();
   }

   return topmostPrefabbedEntity;
}

///////////////////////////////////////////////////////////////////////////////
