#ifndef _ENTITY_UTILS_H
#error "This file can only be included in EntityUtils.h"
#else

#include "core-MVC\SceneNode.h"
#include "core\LocalStack.h"


///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
NodeType* EntityUtils::getFirstParent( SceneNode* node )
{
   SceneNode* checkedEntity = node;
   while( checkedEntity != NULL )
   {
      if ( checkedEntity->isA< NodeType >() )
      {
         // found it
         return static_cast< NodeType* >( checkedEntity );
      }
      else
      {
         checkedEntity = checkedEntity->getParent();
      }
   }

   // we didn't find a match
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
NodeType* EntityUtils::getFirstChild( SceneNode* node )
{
   LocalStack< SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         return static_cast< NodeType* >( currNode );
      }

      currNode->collectChildren( nodes );
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
const NodeType* EntityUtils::getFirstChild( const SceneNode* node )
{
   LocalStack< const SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      const SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         return static_cast< const NodeType* >( currNode );
      }

      currNode->collectChildren( nodes );
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void EntityUtils::collectNodesByType( SceneNode* node, List< NodeType* >& outNodes )
{
   LocalStack< SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         outNodes.pushBack( static_cast< NodeType* >( currNode ) );
      }

      currNode->collectChildren( nodes );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void EntityUtils::collectNodesByType( SceneNode* node, Array< NodeType* >& outNodes )
{
   LocalStack< SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         outNodes.push_back( static_cast< NodeType* >( currNode ) );
      }

      currNode->collectChildren( nodes );
   }
}

///////////////////////////////////////////////////////////////////////////////


template< typename NodeType >
void EntityUtils::collectNodesByType( const SceneNode* node, List< const NodeType* >& outNodes )
{
   LocalStack< const SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      const SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         outNodes.pushBack( static_cast< const NodeType* >( currNode ) );
      }

      currNode->collectChildren( nodes );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
void EntityUtils::collectNodesByType( const SceneNode* node, Array< const NodeType* >& outNodes )
{
   LocalStack< const SceneNode* > nodes;
   nodes.push( node );

   while ( nodes.empty() == false )
   {
      const SceneNode* currNode = nodes.pop();
      if ( currNode->isA< NodeType >() )
      {
         outNodes.push_back( static_cast< const NodeType* >( currNode ) );
      }

      currNode->collectChildren( nodes );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _ENTITY_UTILS_H
